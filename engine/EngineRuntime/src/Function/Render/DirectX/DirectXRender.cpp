#include <iostream>
#ifdef _WIN64
#include <d3dcompiler.h>
#include "EngineRuntime/include/Core/Math/Color.h"
#include "EngineRuntime/include/Function/Render/DirectX/d3dx12.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXRender.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Render/DirectX/GeometryGenerator.h"
#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Vector4.h"

namespace Engine
{
	const int gNumFrameResources = 3;

	struct Vertex
	{
		Vector3 Pos;
		Vector4 Color;
	};

	DirectXRender::~DirectXRender()
	{

	}

	bool DirectXRender::Initialize(InitConfig* info)
	{
		if (!DirectXRenderBase::Initialize(info))
		{
			return false;
		}

		ThrowIfFailed(mCommandList->Reset(mCMDListAlloc.Get(), nullptr), "");

		BuildRootSignature();

		BuildShadersAndInputLayout();

		BuildShapeGeometry();

		BuildRenderItems();

		BuildFrameResources();

		BuildDescriptorHeaps();

		BuildConstantBufferViews();

		BuildPSOs();

		ThrowIfFailed(mCommandList->Close(), "");
		ID3D12CommandList* cmdList[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);

		FlushCommandQueue();

		WindowSystem* pWindow = (WindowSystem*)info->windowHandle;
		pWindow->RegisterMouseMoveCallback([this](double x, double y)
			{
				if (this->WindowFlag & (1 << GLFW_MOUSE_BUTTON_LEFT))
				{
					float dx = XMConvertToRadians(0.25f * static_cast<float>(x - this->mLastMousePos.x));
					float dy = XMConvertToRadians(0.25f * static_cast<float>(y - this->mLastMousePos.y));

					// Update angles based on input to orbit camera around box.
					this->mTheta += dx;
					this->mPhi += dy;

					// Restrict the angle mPhi.
					this->mPhi = this->mPhi < 0.1f ? 0.1f : (this->mPhi > XM_PI - 0.1f ? XM_PI - 0.1f : this->mPhi);
				}
				if (this->WindowFlag & (1 << GLFW_MOUSE_BUTTON_RIGHT))
				{
					// Make each pixel correspond to 0.005 unit in the scene.
					float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
					float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);

					// Update the camera radius based on input.
					this->mRadius += dx - dy;

					// Restrict the radius.
					this->mRadius = Math::Clamp(this->mRadius, 5.0f, 150.0f);
				}
				this->mLastMousePos.x = x;
				this->mLastMousePos.y = y;
			});
		pWindow->RegisterMouseKeyInputCallback([this](int button, int action, int mods)
			{
				if (action == GLFW_PRESS)
				{
					this->WindowFlag |= (1 << button);
				}
				else if (action == GLFW_RELEASE)
				{
					this->WindowFlag &= ~(1 << button);
				}
			});

		return true;
	}

	bool DirectXRender::Tick(float deltaTime)
	{
		Updata();
		Draw();

		return true;
	}

	void DirectXRender::Finalize()
	{

	}

	void DirectXRender::OnResize()
	{
		DirectXRenderBase::OnResize();

		mProj = Math::MakePerspectiveMatrix(Radian(0.25f * Math_PI), AspectRatio(), 1.0f, 1000.0f);
	}

	void DirectXRender::Updata()
	{
		UpdateCamera();

		mCurrentFrameResourceIndex = (mCurrentFrameResourceIndex + 1) % gNumFrameResources;
		mCurrentFrameResource = mFrameResources[mCurrentFrameResourceIndex].get();
		if (mCurrentFrameResource->mFance != 0 && mFence->GetCompletedValue() < mCurrentFrameResource->mFance)
		{
			//HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFrameResource->mFance, eventHandle), "");
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}

		UpdateObjectCBs();

		UpdateMainPassCB();
	}

	void DirectXRender::UpdateCamera()
	{
		// Convert Spherical to Cartesian coordinates.
		mEyePos[0] = mRadius * sinf(mPhi) * cosf(mTheta);
		mEyePos[2] = mRadius * sinf(mPhi) * sinf(mTheta);
		mEyePos[1] = mRadius * cosf(mPhi);

		mView = Math::MakeLookAtMatrix(mEyePos, Vector3::ZERO, Vector3(0.0f, 1.0f, 0.0f));
	}

	void DirectXRender::UpdateObjectCBs()
	{
		auto currentObjectUpload = mCurrentFrameResource->mObjectUpload.get();
		for (auto& item : mAllRenderitems)
		{
			if (item->NumFramesDirty > 0)
			{
				ObjectConstants objConstans;
				objConstans.World = item->World;

				currentObjectUpload->CopyData(item->ObjCBIndex, objConstans);

				--item->NumFramesDirty;
			}
		}
	}

	void DirectXRender::UpdateMainPassCB()
	{
		mMainPassCB.View = mView;
		mMainPassCB.InvView = mView.Inverse();
		mMainPassCB.Proj = mProj;
		mMainPassCB.InvProj = mProj.Inverse();
		mMainPassCB.ViewProj = (mProj * mView);
		mMainPassCB.InvViewProj = mMainPassCB.ViewProj.Inverse();
		mMainPassCB.EyePosW = mEyePos;
		mMainPassCB.RenderTargetSize = Vector2((float)mClientWidth, (float)mClientHeight);;
		mMainPassCB.InvRenderTargetSize = Vector2(1.0f / mClientWidth, 1.0f / mClientHeight);
		mMainPassCB.NearZ = 1.0f;
		mMainPassCB.FarZ = 1000.0f;
		mMainPassCB.TotalTime = 0.0f;
		mMainPassCB.DeltaTime = 0.0f;

		auto currPassCB = mCurrentFrameResource->mPassUpload.get();
		currPassCB->CopyData(0, mMainPassCB);
	}

	void DirectXRender::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const TArray<std::unique_ptr<RenderItem>>& items)
	{
		for (const auto& item : items)
		{
			cmdList->IASetVertexBuffers(0, 1, &item->Geo->VectexBufferView());

			cmdList->IASetIndexBuffer(&item->Geo->IndexBufferView());

			cmdList->IASetPrimitiveTopology(item->PrimitiveType);

			UINT cbvIndex = mCurrentFrameResourceIndex * (UINT)items.size() + item->ObjCBIndex;
			auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCBVHeap->GetGPUDescriptorHandleForHeapStart());
			cbvHandle.Offset(cbvIndex, mCBV_SRV_UAVDescriptorSize);

			cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);

			cmdList->DrawIndexedInstanced(item->IndexCount, 1, item->StartIndexLocation, item->BaseVertexLocation, 0);
		}
	}

	void DirectXRender::Draw()
	{
		// 重复使用记录命令的相关内存
		// 只有当与GPU关联的命令列表执行完成时，我们才将其重置
		auto cmdListAlloc = mCurrentFrameResource->mCmdListAlloc;
		ThrowIfFailed(cmdListAlloc->Reset(), "分配器重置失败");

		// 通过 ExecuteCommandLists 方法将某个命令列表加入命令队列后，我们便可以重置该命令列表。以此来复用命令列表的内存
		ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSO.Get()), "命令列表重置失败");

		// 设置视口和剪裁矩形。它们需要随着命令列表的重置而重置
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		// 对资源的状态进行转换，将资源从呈现状态转换为渲染目标状态
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// 清除后台缓冲区和深度缓冲区
		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue.ptr(), 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// 指定将要渲染的缓冲区
		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		ID3D12DescriptorHeap* descriptorHeaps[] = { mCBVHeap.Get() };
		mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		mCommandList->SetGraphicsRootSignature(mRootSignature.Get());


		int passCBVIndex = mPassCbvOffset + mCurrentFrameResourceIndex;
		auto passCBVHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCBVHeap->GetGPUDescriptorHandleForHeapStart());
		passCBVHandle.Offset(passCBVIndex, mCBV_SRV_UAVDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(1, passCBVHandle);

		DrawRenderItems(mCommandList.Get(), mAllRenderitems);

		// 再次对资源状态进行转换，将资源状态由渲染目标状态转换回呈现状态
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		// 完成命令的记录
		ThrowIfFailed(mCommandList->Close(), "命令列表关闭失败");

		// 将待执行的命令列表加入命令队列
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// 交换后台缓冲区和前台缓冲区
		ThrowIfFailed(mSwapChain->Present(0, 0), "前后缓冲区互换失败");
		mCurrBackBufferIndex = (mCurrBackBufferIndex + 1) % mSwapChainBufferCount;

		mCurrentFrameResource->mFance = ++mCurrentFence;
		mCommandQueue->Signal(mFence.Get(), mCurrentFence);
	}

	void DirectXRender::BuildRootSignature()
	{
		CD3DX12_DESCRIPTOR_RANGE cbvTable0;
		cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE cbvTable1;
		cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

		CD3DX12_ROOT_PARAMETER slotRootParameter[2];
		slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
		slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr, "根签名序列化失败");

		ThrowIfFailed(mD3DDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(mRootSignature.GetAddressOf())), "根签名创建失败");
	}

	void DirectXRender::BuildShadersAndInputLayout()
	{
		mVSByteCode = D3DUtil::CompileShader(L"./Shaders/color.hlsl", nullptr, "VS", "vs_5_1");
		mPSByteCode = D3DUtil::CompileShader(L"./Shaders/color.hlsl", nullptr, "PS", "ps_5_1");

		mInputLayout =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
	}

	void DirectXRender::BuildShapeGeometry()
	{
		GeometryGenerator geoGen;
		GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
		GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
		GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
		GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

		UINT boxVertexOffset = 0;
		UINT gridVertexOffset = boxVertexOffset + (UINT)box.Vertices.size();
		UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
		UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

		UINT boxIndexOffset = 0;
		UINT gridIndexOffset = boxIndexOffset + (UINT)box.Indices32.size();
		UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
		UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

		SubmeshGeometry boxSubmesh;
		boxSubmesh.IndexCount = (UINT)box.Indices32.size();
		boxSubmesh.BaseVertexLocation = boxVertexOffset;
		boxSubmesh.StartIndexLocation = boxIndexOffset;

		SubmeshGeometry gridSubmesh;
		gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
		gridSubmesh.BaseVertexLocation = gridVertexOffset;
		gridSubmesh.StartIndexLocation = gridIndexOffset;

		SubmeshGeometry sphereSubmesh;
		sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
		sphereSubmesh.BaseVertexLocation = sphereVertexOffset;
		sphereSubmesh.StartIndexLocation = sphereIndexOffset;

		SubmeshGeometry cylinderSubmesh;
		cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
		cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;
		cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;


		auto totalVertexCount = box.Vertices.size() + grid.Vertices.size() + sphere.Vertices.size() + cylinder.Vertices.size();

		TArray<Vertex> vertices(totalVertexCount);
		UINT k = 0;
		for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos = box.Vertices[i].Position;
			vertices[k].Color = Colors::AliceBlue;
		}

		for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos = grid.Vertices[i].Position;
			vertices[k].Color = Colors::ForestGreen;
		}

		for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos = sphere.Vertices[i].Position;
			vertices[k].Color = Colors::Crimson;
		}

		for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos = cylinder.Vertices[i].Position;
			vertices[k].Color = Colors::SteelBlue;
		}

		TArray<UINT32> indices;
		indices.insert(indices.end(), box.Indices32.begin(), box.Indices32.end());
		indices.insert(indices.end(), grid.Indices32.begin(), grid.Indices32.end());
		indices.insert(indices.end(), sphere.Indices32.begin(), sphere.Indices32.end());
		indices.insert(indices.end(), cylinder.Indices32.begin(), cylinder.Indices32.end());

		const UINT vbByteSize = vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = indices.size() * sizeof(UINT32);


		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = "shapeGeo";

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU), "创建顶点缓冲区副本失败");
		memcpy(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
		ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU), "创建索引缓冲区副本失败");
		memcpy(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = D3DUtil::CreateDefaultBuffer(mD3DDevice.Get(),
			mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
		geo->IndexBufferGPU = D3DUtil::CreateDefaultBuffer(mD3DDevice.Get(),
			mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(Vertex);
		geo->VertexBufferByteSize = vbByteSize;

		geo->IndexFormat = DXGI_FORMAT_R32_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		geo->DrawArgs["box"] = boxSubmesh;
		geo->DrawArgs["grid"] = gridSubmesh;
		geo->DrawArgs["sphere"] = sphereSubmesh;
		geo->DrawArgs["cylinder"] = cylinderSubmesh;

		mGeometries[geo->Name] = std::move(geo);

	}

	void DirectXRender::BuildRenderItems()
	{
		auto boxRitem = std::make_unique<RenderItem>();
		//XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
		boxRitem->World.SetTrans(Vector3(0.0f, 0.5f, 0.0f));
		boxRitem->World.SetScale(Vector3(2.0f, 2.0f, 2.0f));
		boxRitem->NumFramesDirty = gNumFrameResources;
		boxRitem->ObjCBIndex = 0;
		boxRitem->Geo = mGeometries["shapeGeo"].get();
		boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRitem->IndexCount = boxRitem->Geo->DrawArgs.at("box").IndexCount;
		boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs.at("box").StartIndexLocation;
		boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs.at("box").BaseVertexLocation;
		mAllRenderitems.push_back(std::move(boxRitem));

		auto gridRitem = std::make_unique<RenderItem>();
		gridRitem->World = Matrix4x4::IDENTITY;
		gridRitem->NumFramesDirty = gNumFrameResources;
		gridRitem->ObjCBIndex = 1;
		gridRitem->Geo = mGeometries["shapeGeo"].get();
		gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridRitem->IndexCount = gridRitem->Geo->DrawArgs.at("grid").IndexCount;
		gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs.at("grid").StartIndexLocation;
		gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs.at("grid").BaseVertexLocation;
		mAllRenderitems.push_back(std::move(gridRitem));

		UINT objCBIndex = 2;
		for (int i = 0; i < 5; ++i)
		{
			auto leftCylRitem = std::make_unique<RenderItem>();
			auto rightCylRitem = std::make_unique<RenderItem>();
			auto leftSphereRitem = std::make_unique<RenderItem>();
			auto rightSphereRitem = std::make_unique<RenderItem>();

			Matrix4x4 leftCylWorld;
			leftCylWorld.MakeTrans(Vector3(-5.0f, 1.5f, -10.0f + i * 5.0f));
			Matrix4x4 rightCylWorld;
			rightCylWorld.MakeTrans(Vector3(+5.0f, 1.5f, -10.0f + i * 5.0f));
			Matrix4x4 leftSphereWorld;
			leftSphereWorld.MakeTrans(Vector3(-5.0f, 3.5f, -10.0f + i * 5.0f));
			Matrix4x4 rightSphereWorld;
			rightSphereWorld.MakeTrans(Vector3(+5.0f, 3.5f, -10.0f + i * 5.0f));

			leftCylRitem->World = leftCylWorld;
			leftCylRitem->NumFramesDirty = gNumFrameResources;
			leftCylRitem->ObjCBIndex = objCBIndex++;
			leftCylRitem->Geo = mGeometries["shapeGeo"].get();
			leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs.at("cylinder").IndexCount;
			leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs.at("cylinder").StartIndexLocation;
			leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs.at("cylinder").BaseVertexLocation;

			rightCylRitem->World = rightCylWorld;
			rightCylRitem->NumFramesDirty = gNumFrameResources;
			rightCylRitem->ObjCBIndex = objCBIndex++;
			rightCylRitem->Geo = mGeometries["shapeGeo"].get();
			rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs.at("cylinder").IndexCount;
			rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs.at("cylinder").StartIndexLocation;
			rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs.at("cylinder").BaseVertexLocation;

			leftSphereRitem->World = leftSphereWorld;
			leftSphereRitem->NumFramesDirty = gNumFrameResources;
			leftSphereRitem->ObjCBIndex = objCBIndex++;
			leftSphereRitem->Geo = mGeometries["shapeGeo"].get();
			leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs.at("sphere").IndexCount;
			leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs.at("sphere").StartIndexLocation;
			leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs.at("sphere").BaseVertexLocation;

			rightSphereRitem->World = rightSphereWorld;
			rightSphereRitem->NumFramesDirty = gNumFrameResources;
			rightSphereRitem->ObjCBIndex = objCBIndex++;
			rightSphereRitem->Geo = mGeometries["shapeGeo"].get();
			rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs.at("sphere").IndexCount;
			rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs.at("sphere").StartIndexLocation;
			rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs.at("sphere").BaseVertexLocation;

			mAllRenderitems.push_back(std::move(leftCylRitem));
			mAllRenderitems.push_back(std::move(rightCylRitem));
			mAllRenderitems.push_back(std::move(leftSphereRitem));
			mAllRenderitems.push_back(std::move(rightSphereRitem));
		}
	}

	void DirectXRender::BuildFrameResources()
	{
		for (int i = 0; i < gNumFrameResources; ++i)
		{
			mFrameResources.push_back(std::make_unique<FrameResource>(mD3DDevice.Get(), mAllRenderitems.size(), 1));
		}
	}

	void DirectXRender::BuildDescriptorHeaps()
	{
		UINT objCount = (UINT)mAllRenderitems.size();

		UINT numDescriptors = (objCount + 1) * gNumFrameResources;

		mPassCbvOffset = objCount * gNumFrameResources;

		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.NodeMask = 0;
		cbvHeapDesc.NumDescriptors = numDescriptors;
		ThrowIfFailed(mD3DDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCBVHeap)), "创建CBV堆失败");
	}

	void DirectXRender::BuildConstantBufferViews()
	{
		UINT objCBBtyeSize = D3DUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		UINT objCount = (UINT)mAllRenderitems.size();

		for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
		{
			auto objectCB = mFrameResources[frameIndex]->mObjectUpload->Resource();
			for (int i = 0; i < objCount; ++i)
			{
				D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

				cbAddress += i * objCBBtyeSize;

				int heapIndex = frameIndex * objCount + i;
				auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCBVHeap->GetCPUDescriptorHandleForHeapStart());
				handle.Offset(heapIndex, mCBV_SRV_UAVDescriptorSize);

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
				cbvDesc.BufferLocation = cbAddress;
				cbvDesc.SizeInBytes = objCBBtyeSize;

				mD3DDevice->CreateConstantBufferView(&cbvDesc, handle);
			}
		}

		UINT passCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

		for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
		{
			auto passCB = mFrameResources[frameIndex]->mPassUpload->Resource();
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

			int heapIndex = mPassCbvOffset + frameIndex;

			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCBVHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, mCBV_SRV_UAVDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = passCBByteSize;

			mD3DDevice->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	void DirectXRender::BuildPSOs()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc = {};
		opaquePsoDesc.InputLayout.NumElements = (UINT)mInputLayout.size();
		opaquePsoDesc.InputLayout.pInputElementDescs = mInputLayout.data();
		opaquePsoDesc.pRootSignature = mRootSignature.Get();
		opaquePsoDesc.VS =
		{
			reinterpret_cast<char*>(mVSByteCode->GetBufferPointer()),
			mVSByteCode->GetBufferSize()
		};
		opaquePsoDesc.PS =
		{
			reinterpret_cast<char*>(mPSByteCode->GetBufferPointer()),
			mPSByteCode->GetBufferSize()
		};
		opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		opaquePsoDesc.SampleMask = UINT_MAX;
		opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		opaquePsoDesc.NumRenderTargets = 1;
		opaquePsoDesc.RTVFormats[0] = mBackBufferFormat;
		opaquePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
		opaquePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
		opaquePsoDesc.DSVFormat = mDepthStencilFormat;
		ThrowIfFailed(mD3DDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSO)), "创建PSO失败");

	}
}

#endif

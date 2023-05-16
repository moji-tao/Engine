#include <dxgidebug.h>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12ImGuiDevice.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12DeferredRenderPipeline.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RenderResource.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"

namespace Engine
{
#define InstalledDebugLayers true

	using Microsoft::WRL::ComPtr;

	D3D12RHI::D3D12RHI(WindowSystem* windowSystem)
		:RHI(windowSystem)
	{
		Initialize((HWND)windowSystem->GetWindowHandle(true), windowSystem->GetWindowWidth(), windowSystem->GetWindowHeight());
	}

	D3D12RHI::~D3D12RHI()
	{
		Destroy();
	}

	void D3D12RHI::Initialize(HWND WindowHandle, int WindowWidth, int WindowHeight)
	{
		unsigned DxgiFactoryFlags = 0;

#if (defined(DEBUG) || defined(_DEBUG)) && InstalledDebugLayers
		{
			ComPtr<ID3D12Debug> DebugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(DebugController.GetAddressOf())), "");
			DebugController->EnableDebugLayer();
		}

		ComPtr<IDXGIInfoQueue> DxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(DxgiInfoQueue.GetAddressOf()))))
		{
			DxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

			DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
#endif

		// Create DxgiFactory
		ThrowIfFailed(CreateDXGIFactory2(DxgiFactoryFlags, IID_PPV_ARGS(DxgiFactory.GetAddressOf())), "");

		// Create Device
		Device = std::make_unique<D3D12Device>(this);

		// Create Viewport
		ViewportInfo.WindowHandle = WindowHandle;
		ViewportInfo.BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		ViewportInfo.DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ViewportInfo.bEnable4xMSAA = false;
		ViewportInfo.QualityOf4xMSAA = GetSupportMSAAQuality(ViewportInfo.BackBufferFormat);
		
		Viewport = std::make_unique<D3D12Viewport>(this, ViewportInfo, WindowWidth, WindowHeight);

#ifdef _DEBUG
		LogAdapters();
#endif

	}

	void D3D12RHI::Destroy()
	{
		EndFrame();

		Viewport.reset();

		Device.reset();
	}

	void D3D12RHI::InitializeRenderPipeline(std::unique_ptr<RenderPipeline>& renderPipeline, std::unique_ptr<RenderResource>& renderResource)
	{
		ResetCommandAllocator();
		ResetCommandList();

		renderResource = std::make_unique<D3D12RenderResource>(this);
		
		renderPipeline = std::make_unique<D3D12DeferredRenderPipeline>(this, dynamic_cast<D3D12RenderResource*>(renderResource.get()));

		ExecuteCommandLists();
		FlushCommandQueue();
	}

	D3D12Device* D3D12RHI::GetDevice()
	{
		return Device.get();
	}

	D3D12Viewport* D3D12RHI::GetViewport()
	{
		return Viewport.get();
	}

	const D3D12ViewportInfo& D3D12RHI::GetViewportInfo()
	{
		return ViewportInfo;
	}

	IDXGIFactory4* D3D12RHI::GetDxgiFactory()
	{
		return DxgiFactory.Get();
	}

	void D3D12RHI::InitEditorUI(std::unique_ptr<ImGuiDevice>& editorUI, WindowUI* windowUI)
	{
		editorUI = std::make_unique<D3D12ImGuiDevice>(windowUI);

		ID3D12GraphicsCommandList* imguiCommandList = GetDevice()->GetCommandContext()->CreateImGuiCommand();

		D3D12ImGuiDevice* d3dEditorUI = dynamic_cast<D3D12ImGuiDevice*>(editorUI.get());

		ASSERT(d3dEditorUI != nullptr);

		d3dEditorUI->Initialize(GetDevice()->GetNativeDevice(), imguiCommandList, GetViewport());
	}

	void D3D12RHI::FlushCommandQueue()
	{
		GetDevice()->GetCommandContext()->FlushCommandQueue();
	}

	void D3D12RHI::ExecuteCommandLists()
	{
		GetDevice()->GetCommandContext()->ExecuteCommandLists();
	}

	void D3D12RHI::ResetCommandList()
	{
		GetDevice()->GetCommandContext()->ResetCommandList();
	}

	void D3D12RHI::ResetCommandAllocator()
	{
		GetDevice()->GetCommandContext()->ResetCommandAllocator();
	}

	void D3D12RHI::Present()
	{
		GetViewport()->Present();
	}

	void D3D12RHI::ResizeViewport(int width, int height)
	{
		GetViewport()->OnResize(width, height);
	}

	void D3D12RHI::TransitionResource(D3D12Resource* Resource, D3D12_RESOURCE_STATES StateAfter)
	{
		D3D12_RESOURCE_STATES StateBefore = Resource->CurrentState;

		if(StateBefore != StateAfter)
		{
			GetDevice()->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource->D3DResource.Get(), StateBefore, StateAfter));

			Resource->CurrentState = StateAfter;
		}
	}

	void D3D12RHI::CopyResource(D3D12Resource* DstResource, D3D12Resource* SrcResource)
	{
		GetDevice()->GetCommandList()->CopyResource(DstResource->D3DResource.Get(), SrcResource->D3DResource.Get());
	}

	void D3D12RHI::CopyBufferRegion(D3D12Resource* DstResource, uint64_t DstOffset, D3D12Resource* SrcResource, uint64_t SrcOffset, uint64_t Size)
	{
		GetDevice()->GetCommandList()->CopyBufferRegion(DstResource->D3DResource.Get(), DstOffset, SrcResource->D3DResource.Get(), SrcOffset, Size);
	}

	void D3D12RHI::CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION* Dst, UINT DstX, UINT DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* Src, const D3D12_BOX* SrcBox)
	{
		GetDevice()->GetCommandList()->CopyTextureRegion(Dst, DstX, DstY, DstZ, Src, SrcBox);
	}

	D3D12ConstantBufferRef D3D12RHI::CreateConstantBuffer(const void* Contents, uint32_t Size)
	{
		D3D12ConstantBufferRef ConstantBufferRef = std::make_shared<D3D12ConstantBuffer>();

		auto UploadBufferAllocator = GetDevice()->GetUploadBufferAllocator();
		void* MappedData = UploadBufferAllocator->AllocUploadResource(Size, UPLOAD_RESOURCE_ALIGNMENT, ConstantBufferRef->ResourceLocation);

		memcpy(MappedData, Contents, Size);

		return ConstantBufferRef;
	}

	D3D12StructuredBufferRef D3D12RHI::CreateStructuredBuffer(const void* Contents, uint32_t ElementSize, uint32_t ElementCount)
	{
		ASSERT(Contents != nullptr && ElementSize > 0 && ElementCount > 0);

		D3D12StructuredBufferRef StructuredBufferRef = std::make_shared<D3D12StructuredBuffer>();

		auto UploadBufferAllocator = GetDevice()->GetUploadBufferAllocator();
		uint32_t DataSize = ElementCount * ElementSize;

		void* MappedData = UploadBufferAllocator->AllocUploadResource(DataSize, ElementSize, StructuredBufferRef->ResourceLocation);

		memcpy(MappedData, Contents, DataSize);

		{
			D3D12ResourceLocation& Location = StructuredBufferRef->ResourceLocation;
			const uint64_t Offset = Location.OffsetFromBaseOfResource;
			ID3D12Resource* BufferResource = Location.UnderlyingResource->D3DResource.Get();

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			srvDesc.Buffer.StructureByteStride = ElementSize;
			srvDesc.Buffer.NumElements = ElementCount;
			srvDesc.Buffer.FirstElement = Offset / ElementSize;
			
			StructuredBufferRef->SetSRV(std::make_unique<D3D12ShaderResourceView>(GetDevice(), srvDesc, BufferResource));
		}

		return StructuredBufferRef;
	}

	D3D12RWStructuredBufferRef D3D12RHI::CreateRWStructuredBuffer(uint32_t ElementSize, uint32_t ElementCount)
	{
		D3D12RWStructuredBufferRef RWStructuredBuffer = std::make_shared<D3D12RWStructuredBuffer>();

		uint32_t DataSize = ElementSize * ElementCount;

		CreateDefaultBuffer(DataSize, ElementSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, RWStructuredBuffer->ResourceLocation);

		D3D12ResourceLocation& Location = RWStructuredBuffer->ResourceLocation;
		const uint64_t Offset = Location.OffsetFromBaseOfResource;
		ID3D12Resource* BufferResource = Location.UnderlyingResource->D3DResource.Get();

		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			srvDesc.Buffer.StructureByteStride = ElementSize;
			srvDesc.Buffer.NumElements = ElementCount;
			srvDesc.Buffer.FirstElement = Offset / ElementSize;

			RWStructuredBuffer->SetSRV(std::make_unique<D3D12ShaderResourceView>(GetDevice(), srvDesc, BufferResource));
		}

		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
			uavDesc.Buffer.StructureByteStride = ElementSize;
			uavDesc.Buffer.NumElements = ElementCount;
			uavDesc.Buffer.FirstElement = Offset / ElementSize;
			uavDesc.Buffer.CounterOffsetInBytes = 0;

			RWStructuredBuffer->SetUAV(std::make_unique<D3D12UnorderedAccessView>(GetDevice(), uavDesc, BufferResource));
		}

		return RWStructuredBuffer;
	}

	D3D12VertexBufferRef D3D12RHI::CreateVertexBuffer(const void* Contents, uint32_t Size)
	{
		D3D12VertexBufferRef VertexBufferRef = std::make_shared<D3D12VertexBuffer>();

		CreateAndInitDefaultBuffer(Contents, Size, DEFAULT_RESOURCE_ALIGNMENT, VertexBufferRef->ResourceLocation);

		return VertexBufferRef;
	}

	D3D12IndexBufferRef D3D12RHI::CreateIndexBuffer(const void* Contents, uint32_t Size)
	{
		D3D12IndexBufferRef IndexBufferRef = std::make_shared<D3D12IndexBuffer>();

		CreateAndInitDefaultBuffer(Contents, Size, DEFAULT_RESOURCE_ALIGNMENT, IndexBufferRef->ResourceLocation);

		return IndexBufferRef;

	}

	D3D12ReadBackBufferRef D3D12RHI::CreateReadBackBuffer(uint32_t Size)
	{
		D3D12ReadBackBufferRef ReadBackBufferRef = std::make_shared<D3D12ReadBackBuffer>();

		Microsoft::WRL::ComPtr<ID3D12Resource> Resource;

		ThrowIfFailed(Device->GetNativeDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(Size),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&Resource)), "");

		D3D12Resource* NewResource = new D3D12Resource(Resource, D3D12_RESOURCE_STATE_COPY_DEST);
		ReadBackBufferRef->ResourceLocation.UnderlyingResource = NewResource;
		ReadBackBufferRef->ResourceLocation.SetType(D3D12ResourceLocation::ResourceLocationType::StandAlone);

		return ReadBackBufferRef;
	}

	D3D12TextureRef D3D12RHI::CreateTexture(const D3D12TextureInfo& TextureInfo, uint32_t CreateFlags, Vector4 RTVClearValue)
	{
		// 创建纹理资源
		D3D12TextureRef textureRef = CreateTextureResource(TextureInfo, CreateFlags, RTVClearValue);

		// 为纹理资源添加描述符
		CreateTextureView(textureRef, TextureInfo, CreateFlags);

		return textureRef;
	}

	D3D12TextureRef D3D12RHI::CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> D3DResource, D3D12TextureInfo& TextureInfo, uint32_t CreateFlags)
	{
		D3D12TextureRef TextureRef = std::make_shared<D3D12Texture>();

		D3D12Resource* NewResource = new D3D12Resource(D3DResource, TextureInfo.InitState);
		TextureRef->ResourceLocation.UnderlyingResource = NewResource;
		TextureRef->ResourceLocation.SetType(D3D12ResourceLocation::ResourceLocationType::StandAlone);

		CreateTextureView(TextureRef, TextureInfo, CreateFlags);

		return TextureRef;
	}

	void D3D12RHI::UploadCubeTextureData(D3D12TextureRef Texture, const std::array<const TextureData*, 6>& textureData)
	{
		std::array<D3D12_SUBRESOURCE_DATA, 6> InitData;

		for (int i = 0; i < 6; ++i)
		{
			InitData[i].pData = textureData[i]->mPixels;
			InitData[i].RowPitch = textureData[i]->Info.mRowPitch;
			InitData[i].SlicePitch = textureData[i]->Info.mSlicePitch;
		}

		auto TextureResource = Texture->GetResource();
		D3D12_RESOURCE_DESC TexDesc = TextureResource->D3DResource->GetDesc();

		//GetCopyableFootprints
		const UINT NumSubresources = (UINT)InitData.size();
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> Layouts(NumSubresources);
		std::vector<uint32_t> NumRows(NumSubresources);
		std::vector<uint64_t> RowSizesInBytes(NumSubresources);

		uint64_t RequiredSize = 0;
		Device->GetNativeDevice()->GetCopyableFootprints(&TexDesc, 0, NumSubresources, 0, &Layouts[0], &NumRows[0], &RowSizesInBytes[0], &RequiredSize);

		//Create upload resource
		D3D12ResourceLocation UploadResourceLocation;
		auto UploadBufferAllocator = GetDevice()->GetUploadBufferAllocator();
		void* MappedData = UploadBufferAllocator->AllocUploadResource((uint32_t)RequiredSize, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT, UploadResourceLocation);
		ID3D12Resource* UploadBuffer = UploadResourceLocation.UnderlyingResource->D3DResource.Get();

		//Copy contents to upload resource
		for (uint32_t i = 0; i < NumSubresources; ++i)
		{
			if (RowSizesInBytes[i] > SIZE_T(-1))
			{
				assert(0);
			}
			D3D12_MEMCPY_DEST DestData = { (BYTE*)MappedData + Layouts[i].Offset, Layouts[i].Footprint.RowPitch, SIZE_T(Layouts[i].Footprint.RowPitch) * SIZE_T(NumRows[i]) };
			MemcpySubresource(&DestData, &(InitData[i]), static_cast<SIZE_T>(RowSizesInBytes[i]), NumRows[i], Layouts[i].Footprint.Depth);
		}

		//Copy data from upload resource to default resource
		TransitionResource(TextureResource, D3D12_RESOURCE_STATE_COPY_DEST);

		for (UINT i = 0; i < NumSubresources; ++i)
		{
			Layouts[i].Offset += UploadResourceLocation.OffsetFromBaseOfResource;

			CD3DX12_TEXTURE_COPY_LOCATION Src;
			Src.pResource = UploadBuffer;
			Src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			Src.PlacedFootprint = Layouts[i];

			CD3DX12_TEXTURE_COPY_LOCATION Dst;
			Dst.pResource = TextureResource->D3DResource.Get();
			Dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			Dst.SubresourceIndex = i;

			CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
		}

		TransitionResource(TextureResource, D3D12_RESOURCE_STATE_COMMON);
	}

	void D3D12RHI::UploadTextureData(D3D12TextureRef Texture, const TextureData* textureData)
	{
		D3D12_SUBRESOURCE_DATA data = { textureData->mPixels, textureData->Info.mRowPitch, textureData->Info.mSlicePitch };
		auto TextureResource = Texture->GetResource();
		D3D12_RESOURCE_DESC TexDesc = TextureResource->D3DResource->GetDesc();

		//GetCopyableFootprints
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT Layout;
		uint32_t NumRow;
		uint64_t RowSizesInByte;
		
		uint64_t RequiredSize = 0;
		Device->GetNativeDevice()->GetCopyableFootprints(&TexDesc, 0, 1, 0, &Layout, &NumRow, &RowSizesInByte, &RequiredSize);

		// 从分配器获取上传堆
		D3D12ResourceLocation uploadResourceLocation;
		auto UploadBufferAllocator = GetDevice()->GetUploadBufferAllocator();
		void* MappedData = UploadBufferAllocator->AllocUploadResource((uint32_t)RequiredSize, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT, uploadResourceLocation);
		ID3D12Resource* UploadBuffer = uploadResourceLocation.UnderlyingResource->D3DResource.Get();

		// 拷贝数据到上传堆
		D3D12_MEMCPY_DEST DestData;
		DestData.pData = (unsigned char*)MappedData + Layout.Offset;
		DestData.RowPitch = Layout.Footprint.RowPitch;
		DestData.SlicePitch = SIZE_T(Layout.Footprint.RowPitch) * NumRow;
		MemcpySubresource(&DestData, &data, RowSizesInByte, NumRow, Layout.Footprint.Depth);

		// 设置资源屏障
		TransitionResource(TextureResource, D3D12_RESOURCE_STATE_COPY_DEST);

		Layout.Offset += uploadResourceLocation.OffsetFromBaseOfResource;

		CD3DX12_TEXTURE_COPY_LOCATION Src;
		Src.pResource = UploadBuffer;
		Src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		Src.PlacedFootprint = Layout;

		CD3DX12_TEXTURE_COPY_LOCATION Dst;
		Dst.pResource = TextureResource->D3DResource.Get();
		Dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		Dst.SubresourceIndex = 0;

		CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);

		//设置资源屏障
		TransitionResource(TextureResource, D3D12_RESOURCE_STATE_COMMON);
	}

	void D3D12RHI::SetVertexBuffer(const D3D12VertexBufferRef& VertexBuffer, UINT Offset, UINT Stride, UINT Size)
	{
		const D3D12ResourceLocation& ResourceLocation = VertexBuffer->ResourceLocation;
		D3D12Resource* Resource = ResourceLocation.UnderlyingResource;
		TransitionResource(Resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);

		// Set vertex buffer
		D3D12_VERTEX_BUFFER_VIEW VBV;
		VBV.BufferLocation = ResourceLocation.GPUVirtualAddress + Offset;
		VBV.StrideInBytes = Stride;
		VBV.SizeInBytes = Size;
		GetDevice()->GetCommandList()->IASetVertexBuffers(0, 1, &VBV);

	}

	void D3D12RHI::SetIndexBuffer(const D3D12IndexBufferRef& IndexBuffer, UINT Offset, DXGI_FORMAT Format, UINT Size)
	{
		const D3D12ResourceLocation& ResourceLocation = IndexBuffer->ResourceLocation;
		D3D12Resource* Resource = ResourceLocation.UnderlyingResource;
		TransitionResource(Resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);

		// Set vertex buffer
		D3D12_INDEX_BUFFER_VIEW IBV;
		IBV.BufferLocation = ResourceLocation.GPUVirtualAddress + Offset;
		IBV.Format = Format;
		IBV.SizeInBytes = Size;
		GetDevice()->GetCommandList()->IASetIndexBuffer(&IBV);

	}

	void D3D12RHI::EndFrame()
	{
		GetDevice()->GetUploadBufferAllocator()->CleanUpAllocations();

		GetDevice()->GetDefaultBufferAllocator()->CleanUpAllocations();

		GetDevice()->GetTextureResourceAllocator()->CleanUpAllocations();

		GetDevice()->GetCommandContext()->EndFrame();
	}

	void D3D12RHI::CreateDefaultBuffer(uint32_t Size, uint32_t Alignment, D3D12_RESOURCE_FLAGS Flags, D3D12ResourceLocation& ResourceLocation)
	{
		D3D12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(Size, Flags);
		auto DefaultBufferAllocation = GetDevice()->GetDefaultBufferAllocator();
		DefaultBufferAllocation->AllocDefaultResource(ResourceDesc, Alignment, ResourceLocation);
	}

	void D3D12RHI::CreateAndInitDefaultBuffer(const void* Contents, uint32_t Size, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation)
	{
		CreateDefaultBuffer(Size, Alignment, D3D12_RESOURCE_FLAG_NONE, ResourceLocation);

		D3D12ResourceLocation UploadResourceLocation;
		auto UploadBufferAllocation = GetDevice()->GetUploadBufferAllocator();
		void* MappedData = UploadBufferAllocation->AllocUploadResource(Size, UPLOAD_RESOURCE_ALIGNMENT, UploadResourceLocation);

		memcpy(MappedData, Contents, Size);

		D3D12Resource* DefaultBuffer = ResourceLocation.UnderlyingResource;
		D3D12Resource* UploadBuffer = UploadResourceLocation.UnderlyingResource;

		TransitionResource(DefaultBuffer, D3D12_RESOURCE_STATE_COPY_DEST);
		CopyBufferRegion(DefaultBuffer, ResourceLocation.OffsetFromBaseOfResource, UploadBuffer, UploadResourceLocation.OffsetFromBaseOfResource, Size);
	}

	D3D12TextureRef D3D12RHI::CreateTextureResource(const D3D12TextureInfo& TextureInfo, uint32_t CreateFlags, Vector4 RTVClearValue)
	{
		D3D12TextureRef textureRef = std::make_unique<D3D12Texture>();

		D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON;

		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		
		texDesc.Dimension = TextureInfo.Dimension;
		texDesc.Alignment = 0;
		texDesc.Width = TextureInfo.Width;
		texDesc.Height = (uint32_t)TextureInfo.Height;
		if (TextureInfo.Type == IMAGE_TYPE::IMAGE_TYPE_CUBE)
		{
			texDesc.DepthOrArraySize = 6;
		}
		else
		{
			texDesc.DepthOrArraySize = (TextureInfo.Depth > 1) ? (uint16_t)TextureInfo.Depth : (uint16_t)TextureInfo.ArraySize;
		}
		texDesc.MipLevels = (uint16_t)TextureInfo.MipCount;
		texDesc.Format = TextureInfo.Format;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		
		bool bCreateRTV = CreateFlags & (TexCreate_RTV | TexCreate_CubeRTV);
		bool bCreateDSV = CreateFlags & (TexCreate_DSV | TexCreate_CubeDSV);
		bool bCreateUAV = CreateFlags & TexCreate_UAV;
		
		if (bCreateRTV)
		{
			texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
		else if (bCreateDSV)
		{
			texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}
		else if (bCreateUAV)
		{
			texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
		else
		{
			texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		}

		bool bReadOnlyTexture = !(bCreateRTV | bCreateDSV | bCreateUAV);
		if (bReadOnlyTexture)
		{
			auto TextureResourceAllocator = GetDevice()->GetTextureResourceAllocator();
			TextureResourceAllocator->AllocTextureResource(resourceState, texDesc, textureRef->ResourceLocation);

			auto TextureResource = textureRef->GetD3DResource();
			ASSERT(TextureResource);
		}
		else
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> Resource;

			CD3DX12_CLEAR_VALUE ClearValue = {};
			CD3DX12_CLEAR_VALUE* ClearValuePtr = nullptr;

			// Set clear value for RTV and DSV
			if (bCreateRTV)
			{
				ClearValue = CD3DX12_CLEAR_VALUE(texDesc.Format, (float*)&RTVClearValue);
				ClearValuePtr = &ClearValue;

				textureRef->SetRTVClearValue(RTVClearValue);
			}
			else if (bCreateDSV)
			{
				FLOAT Depth = 1.0f;
				UINT8 Stencil = 0;
				ClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D24_UNORM_S8_UINT, Depth, Stencil);
				ClearValuePtr = &ClearValue;
			}

			GetDevice()->GetNativeDevice()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&texDesc,
				TextureInfo.InitState,
				ClearValuePtr,
				IID_PPV_ARGS(&Resource));

			D3D12Resource* NewResource = new D3D12Resource(Resource, TextureInfo.InitState);
			textureRef->ResourceLocation.UnderlyingResource = NewResource;
			textureRef->ResourceLocation.SetType(D3D12ResourceLocation::ResourceLocationType::StandAlone);
		}

		return textureRef;
	}

	void D3D12RHI::CreateTextureView(D3D12TextureRef TextureRef, const D3D12TextureInfo& TextureInfo, uint32_t CreateFlags)
	{
		auto textureResource = TextureRef->GetD3DResource();

		// 如果纹理可以作为着色器目标 创建SRV
		if (CreateFlags & TexCreate_SRV)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			if (TextureInfo.SRVFormat == DXGI_FORMAT_UNKNOWN)
			{
				srvDesc.Format = TextureInfo.Format;
			}
			else
			{
				srvDesc.Format = TextureInfo.SRVFormat;
			}
			
			if (TextureInfo.Type == IMAGE_TYPE::IMAGE_TYPE_2D)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			}
			else if (TextureInfo.Type == IMAGE_TYPE::IMAGE_TYPE_CUBE)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			}

			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = (uint16_t)TextureInfo.MipCount;

			TextureRef->AddSRV(std::make_unique<D3D12ShaderResourceView>(GetDevice(), srvDesc, textureResource));
		}

		// 如果纹理可以作为渲染目标 创建RTV
		if (CreateFlags & TexCreate_RTV)
		{
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;

			if (TextureInfo.RTVFormat == DXGI_FORMAT_UNKNOWN)
			{
				rtvDesc.Format = TextureInfo.Format;
			}
			else
			{
				rtvDesc.Format = TextureInfo.RTVFormat;
			}

			TextureRef->AddRTV(std::make_unique<D3D12RenderTargetView>(GetDevice(), rtvDesc, textureResource));
		}
		else if (CreateFlags & TexCreate_CubeRTV)
		{
			for (size_t i = 0; i < 6; i++)
			{
				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtvDesc.Texture2DArray.MipSlice = 0;
				rtvDesc.Texture2DArray.PlaneSlice = 0;
				rtvDesc.Texture2DArray.FirstArraySlice = (UINT)i;
				rtvDesc.Texture2DArray.ArraySize = 1;

				if (TextureInfo.RTVFormat == DXGI_FORMAT_UNKNOWN)
				{
					rtvDesc.Format = TextureInfo.Format;
				}
				else
				{
					rtvDesc.Format = TextureInfo.RTVFormat;
				}

				TextureRef->AddRTV(std::make_unique<D3D12RenderTargetView>(GetDevice(), rtvDesc, textureResource));
			}

			{
				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtvDesc.Texture2DArray.MipSlice = 0;
				rtvDesc.Texture2DArray.PlaneSlice = 0;
				rtvDesc.Texture2DArray.FirstArraySlice = 0;
				rtvDesc.Texture2DArray.ArraySize = 6;

				if (TextureInfo.RTVFormat == DXGI_FORMAT_UNKNOWN)
				{
					rtvDesc.Format = TextureInfo.Format;
				}
				else
				{
					rtvDesc.Format = TextureInfo.RTVFormat;
				}

				TextureRef->AddRTV(std::make_unique<D3D12RenderTargetView>(GetDevice(), rtvDesc, textureResource));
			}
		}

		// 创建DSV
		if (CreateFlags & TexCreate_DSV)
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;

			if (TextureInfo.DSVFormat == DXGI_FORMAT_UNKNOWN)
			{
				dsvDesc.Format = TextureInfo.Format;
			}
			else
			{
				dsvDesc.Format = TextureInfo.DSVFormat;
			}

			TextureRef->AddDSV(std::make_unique<D3D12DepthStencilView>(GetDevice(), dsvDesc, textureResource));
		}
		else if (CreateFlags & TexCreate_CubeDSV)
		{
			for (size_t i = 0; i < 6; i++)
			{
				D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
				DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
				DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				DSVDesc.Texture2DArray.MipSlice = 0;
				DSVDesc.Texture2DArray.FirstArraySlice = (UINT)i;
				DSVDesc.Texture2DArray.ArraySize = 1;

				if (TextureInfo.DSVFormat == DXGI_FORMAT_UNKNOWN)
				{
					DSVDesc.Format = TextureInfo.Format;
				}
				else
				{
					DSVDesc.Format = TextureInfo.DSVFormat;
				}

				TextureRef->AddDSV(std::make_unique<D3D12DepthStencilView>(GetDevice(), DSVDesc, textureResource));
			}

			{
				D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc = {}; 
				DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
				DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				DSVDesc.Texture2DArray.MipSlice = 0;
				DSVDesc.Texture2DArray.FirstArraySlice = 0;
				DSVDesc.Texture2DArray.ArraySize = 6;

				if (TextureInfo.DSVFormat == DXGI_FORMAT_UNKNOWN)
				{
					DSVDesc.Format = TextureInfo.Format;
				}
				else
				{
					DSVDesc.Format = TextureInfo.DSVFormat;
				}

				TextureRef->AddDSV(std::make_unique<D3D12DepthStencilView>(GetDevice(), DSVDesc, textureResource));
			}
		}

		// 创建UAV
		if (CreateFlags & TexCreate_UAV)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = 0;
			
			if (TextureInfo.UAVFormat == DXGI_FORMAT_UNKNOWN)
			{
				uavDesc.Format = TextureInfo.Format;
			}
			else
			{
				uavDesc.Format = TextureInfo.UAVFormat;
			}
			
			TextureRef->AddUAV(std::make_unique<D3D12UnorderedAccessView>(GetDevice(), uavDesc, textureResource));
		}
	}

	void D3D12RHI::LogAdapters()
	{
		unsigned i = 0;
		IDXGIAdapter* adapter = nullptr;
		std::vector<IDXGIAdapter*> adapterList;
		while (DxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);

			std::wstring text = L"***Adapter***";
			text += desc.Description;
			text += L"\n";

			OutputDebugString(text.c_str());

			adapterList.push_back(adapter);

			++i;
		}

		for(size_t i = 0; i < adapterList.size(); ++i)
		{
			LogAdapterOutputs(adapterList[i]);
			ReleaseCom(adapterList[i]);
		}
	}

	void D3D12RHI::LogAdapterOutputs(IDXGIAdapter* adapter)
	{
		unsigned i = 0;
		IDXGIOutput* output = nullptr;
		while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC desc;
			output->GetDesc(&desc);

			std::wstring text = L"***Output: ";
			text += desc.DeviceName;
			text += L"\n";
			OutputDebugString(text.c_str());

			LogOutputDisplayModes(output, ViewportInfo.BackBufferFormat);

			ReleaseCom(output);

			++i;
		}
	}

	void D3D12RHI::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
	{
		unsigned count = 0;
		unsigned flags = 0;

		output->GetDisplayModeList(format, flags, &count, nullptr);

		std::vector<DXGI_MODE_DESC> modeList(count);
		output->GetDisplayModeList(format, flags, &count, &modeList[0]);

		for (auto& x : modeList)
		{
			unsigned n = x.RefreshRate.Numerator;
			unsigned d = x.RefreshRate.Denominator;
			std::wstring text =
				L"Width = " + std::to_wstring(x.Width) + L" " +
				L"Height = " + std::to_wstring(x.Height) + L" " +
				L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
				L"\n";

			::OutputDebugString(text.c_str());
		}
	}

	unsigned D3D12RHI::GetSupportMSAAQuality(DXGI_FORMAT BackBufferFormat)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = BackBufferFormat;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(Device->GetNativeDevice()->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)), "");

		UINT QualityOf4xMsaa = msQualityLevels.NumQualityLevels;
		ASSERT(QualityOf4xMsaa > 0 && "Unexpected MSAA quality level.");

		return QualityOf4xMsaa;
	}
}

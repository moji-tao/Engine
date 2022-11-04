#pragma once
#include "FrameResource.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#ifdef _WIN64
#include "EngineRuntime/include/Function/Render/DirectX/DirectXRenderBase.h"
#include "UploadBuffer.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include <DirectXMath.h>
using namespace DirectX;

namespace Engine
{
	struct RenderItem
	{
		RenderItem() = default;

		Matrix4x4 World = Matrix4x4::IDENTITY;

		int NumFramesDirty;

		UINT ObjCBIndex = -1;

		MeshGeometry* Geo = nullptr;

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

	class DirectXRender : public DirectXRenderBase
	{
	public:
		DirectXRender() = default;

		virtual ~DirectXRender() override;

	public:
		virtual bool Initialize(InitConfig* info) override;

		virtual bool Tick(float deltaTime) override;

		virtual void Finalize() override;

		virtual void OnResize() override;

	private:
		void Updata();

		void Draw();

	private:
		void UpdateCamera();

		void UpdateObjectCBs();

		void UpdateMainPassCB();

		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const TArray<std::unique_ptr<RenderItem>>& items);

	private:
		void BuildRootSignature();

		void BuildShadersAndInputLayout();

		void BuildShapeGeometry();

		void BuildRenderItems();

		void BuildFrameResources();

		void BuildDescriptorHeaps();

		void BuildConstantBufferViews();

		void BuildPSOs();

	private:
		ComPtr<ID3D12DescriptorHeap> mCBVHeap = nullptr;
		ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		ComPtr<ID3DBlob> mVSByteCode;
		ComPtr<ID3DBlob> mPSByteCode;

		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;

		TArray<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

		std::unique_ptr<MeshGeometry> mBoxGeo;

		ComPtr<ID3D12PipelineState> mPSO;

		TArray<std::unique_ptr<RenderItem>> mAllRenderitems;

		TArray<std::unique_ptr<FrameResource>> mFrameResources;

		FrameResource* mCurrentFrameResource = nullptr;

		int mCurrentFrameResourceIndex = 0;

		UINT mPassCbvOffset;

		PassConstants mMainPassCB;

	private:
		Vector3 mEyePos = Vector3::ZERO;
		Matrix4x4 mView = Matrix4x4::IDENTITY;
		Matrix4x4 mProj = Matrix4x4::IDENTITY;

		float mTheta = 1.5f * XM_PI;
		float mPhi = 0.2f * XM_PI;
		float mRadius = 15.0f;

		POINT mLastMousePos;

	private:
		UINT WindowFlag = 0;
	};
}

#endif
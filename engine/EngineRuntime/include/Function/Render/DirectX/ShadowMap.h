#pragma once
#include "EngineRuntime/include/Function/Render/DirectX/RenderTarget.h"

namespace Engine
{
	class ShadowMap2D
	{
	public:
		ShadowMap2D(uint32_t shadowSize, D3D12RHI* rhi);

		RenderTarget2D* GetTarget();

		const D3D12_VIEWPORT& GetViewport();

		const D3D12_RECT& GetScissorRect();

	private:
		std::unique_ptr<RenderTarget2D> mRenderTarget = nullptr;

	private:
		D3D12_VIEWPORT mViewport;

		D3D12_RECT mScissorRect;
	};

	class ShadowMapCube
	{
	public:
		ShadowMapCube(uint32_t shadowSize, D3D12RHI* rhi);

		void SetView(const Vector3& eyePosition);

		Matrix4x4 GetSceneView(uint32_t i);

		RenderTargetCube* GetTarget();

		const D3D12_VIEWPORT& GetViewport();

		const D3D12_RECT& GetScissorRect();

	private:
		std::unique_ptr<RenderTargetCube> mRenderTarget = nullptr;

		Matrix4x4 mViewMat[6];

		D3D12_VIEWPORT mViewport;

		D3D12_RECT mScissorRect;
	};
}

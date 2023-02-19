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

		uint32_t GetShadowMapSize();

	private:
		std::unique_ptr<RenderTarget2D> mRenderTarget = nullptr;

	private:
		uint32_t mSize;

		D3D12_VIEWPORT mViewport;

		D3D12_RECT mScissorRect;
	};

	class ShadowMapCube
	{
	public:
		ShadowMapCube(uint32_t shadowSize, D3D12RHI* rhi);

		void SetView(const Vector3& eyePosition, float zNear, float zFar);

		Matrix4x4 GetSceneView(uint32_t i);

		Matrix4x4 GetSceneProj();

		RenderTargetCube* GetTarget();

		const D3D12_VIEWPORT& GetViewport();

		const D3D12_RECT& GetScissorRect();

		uint32_t GetShadowMapSize();

	private:
		std::unique_ptr<RenderTargetCube> mRenderTarget = nullptr;

		uint32_t mSize;

		Matrix4x4 mViewMat[6];

		Matrix4x4 mProjMat;

		D3D12_VIEWPORT mViewport;

		D3D12_RECT mScissorRect;
	};
}

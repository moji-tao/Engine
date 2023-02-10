#pragma once
#include "EngineRuntime/include/Function/Render/DirectX/RenderTarget.h"

namespace Engine
{
	struct SceneView
	{
		Vector3 EyePos = Vector3::ZERO;
		Matrix4x4 View = Matrix4x4::IDENTITY;
		Matrix4x4 Proj = Matrix4x4::IDENTITY;

		float Near;
		float Far;
	};

	class SceneCaptureCube
	{
	public:
		SceneCaptureCube(bool renderDepth, uint32_t size, DXGI_FORMAT format, D3D12RHI* rhi);

		RenderTargetCube* GetRTCube();

		const SceneView& GetSceneView(UINT index) const;

		D3D12_VIEWPORT GetViewport();

		D3D12_RECT GetScissorRect();

		void CreatePerspectiveViews(const Vector3& eye, float nearPlane, float farPlane);

		uint32_t GetCubeMapSize();

	private:
		void SetViewportAndScissorRect(uint32_t CubeMapSize);

	private:
		D3D12RHI* mRHI = nullptr;

		uint32_t CubeMapSize;

		std::unique_ptr<RenderTargetCube> RTCube = nullptr;

		SceneView SceneViews[6];

		D3D12_VIEWPORT Viewport;

		D3D12_RECT ScissorRect;
	};
}

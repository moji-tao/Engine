#include "EngineRuntime/include/Function/Render/DirectX/SceneCaptureCube.h"

#include "EngineRuntime/include/Core/Math/Math.h"

namespace Engine
{
	SceneCaptureCube::SceneCaptureCube(bool renderDepth, uint32_t size, DXGI_FORMAT format, D3D12RHI* rhi)
		:mRHI(rhi), CubeMapSize(size)
	{
		RTCube = std::make_unique<RenderTargetCube>(mRHI, renderDepth, size, format);

		SetViewportAndScissorRect(size);
	}

	RenderTargetCube* SceneCaptureCube::GetRTCube()
	{
		return RTCube.get();
	}

	const SceneView& SceneCaptureCube::GetSceneView(UINT index) const
	{
		return SceneViews[index];
	}

	D3D12_VIEWPORT SceneCaptureCube::GetViewport()
	{
		return Viewport;
	}

	D3D12_RECT SceneCaptureCube::GetScissorRect()
	{
		return ScissorRect;
	}

	void SceneCaptureCube::CreatePerspectiveViews(const Vector3& Eye, float NearPlane, float FarPlane)
	{
		// Look along each coordinate axis.
		/*
		Vector3 Targets[6] =
		{
			Eye + Vector3(0.0f, 0.0f, +1.0f),	// +X 
			Eye + Vector3(0.0f, 0.0f, -1.0f),	// -X
			Eye + Vector3(0.0f, -1.0f, 0.0f),	// +Y
			Eye + Vector3(0.0f, +1.0f, 0.0f),	// -Y 
			Eye + Vector3(+1.0f, 0.0f, 0.0f),	// +X 
			Eye + Vector3(-1.0f, 0.0f, 0.0f),	// -X 
		};

		Vector3 Ups[6] =
		{
			{0.0f, -1.0f, 0.0f},	// +X 
			{0.0f, -1.0f, 0.0f},	// -X 
			{0.0f, 0.0f, -1.0f},	// +Y 
			{0.0f, 0.0f, +1.0f},	// -Y 
			{0.0f, -1.0f, 0.0f},	// +Z 
			{0.0f, -1.0f, 0.0f}		// -Z 
		};

		for (int i = 0; i < 6; ++i)
		{
			SceneViews[i].EyePos = Eye;
			SceneViews[i].View = Math::MakeLookAtMatrix(Eye, Targets[i], Ups[i]).Transpose();
			
			float Fov = 0.5f * Math_PI;
			float AspectRatio = 1.0f; //Square
			SceneViews[i].Proj = Math::MakePerspectiveMatrix(Radian(Fov), AspectRatio, NearPlane, FarPlane).Transpose();
			
			SceneViews[i].Near = NearPlane;
			SceneViews[i].Far = FarPlane;
		}
		*/
		Vector3 Targets[6] =
		{
			Eye + Vector3(+1.0f, 0.0f, 0.0f),	// +X 
			Eye + Vector3(-1.0f, 0.0f, 0.0f),	// -X
			Eye + Vector3(0.0f, +1.0f, 0.0f),	// +Y
			Eye + Vector3(0.0f, -1.0f, 0.0f),	// -Y 
			Eye + Vector3(0.0f, 0.0f, +1.0f),	// +X 
			Eye + Vector3(0.0f, 0.0f, -1.0f),	// -X 
		};

		Vector3 Ups[6] =
		{
			{0.0f, +1.0f, 0.0f},	// +X 
			{0.0f, +1.0f, 0.0f},	// -X 
			{0.0f, 0.0f, -1.0f},	// +Y 
			{0.0f, 0.0f, +1.0f},	// -Y 
			{0.0f, +1.0f, 0.0f},	// +Z 
			{0.0f, +1.0f, 0.0f}		// -Z 
		};

		for (int i = 0; i < 6; ++i)
		{
			SceneViews[i].EyePos = Eye;
			SceneViews[i].View = Math::MakeLookAtMatrix(Eye, Targets[i], Ups[i]);

			float Fov = 0.5f * Math_PI;
			float AspectRatio = 1.0f; //Square
			SceneViews[i].Proj = Math::MakePerspectiveMatrix(Radian(Fov), AspectRatio, NearPlane, FarPlane);

			SceneViews[i].Near = NearPlane;
			SceneViews[i].Far = FarPlane;
		}

	}

	uint32_t SceneCaptureCube::GetCubeMapSize()
	{
		return CubeMapSize;
	}

	void SceneCaptureCube::SetViewportAndScissorRect(uint32_t CubeMapSize)
	{
		Viewport = { 0.0f, 0.0f, (float)CubeMapSize, (float)CubeMapSize, 0.0f, 1.0f };

		ScissorRect = { 0, 0, (int)(Viewport.Width), (int)(Viewport.Height) };
	}
}

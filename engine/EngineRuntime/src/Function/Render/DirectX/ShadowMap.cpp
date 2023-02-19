#include "EngineRuntime/include/Function/Render/DirectX/ShadowMap.h"

#include "EngineRuntime/include/Core/Math/Math.h"

namespace Engine
{
	ShadowMap2D::ShadowMap2D(uint32_t shadowSize, D3D12RHI* rhi)
		:mSize(shadowSize)
	{
		mRenderTarget = std::make_unique<RenderTarget2D>(rhi, true, shadowSize, shadowSize, DXGI_FORMAT_R24G8_TYPELESS);

		mViewport = { 0.0f, 0.0f, (float)shadowSize, (float)shadowSize, 0.0f, 1.0f };

		mScissorRect = { 0, 0, (int)(shadowSize), (int)(shadowSize) };
	}

	RenderTarget2D* ShadowMap2D::GetTarget()
	{
		return mRenderTarget.get();
	}

	const D3D12_VIEWPORT& ShadowMap2D::GetViewport()
	{
		return mViewport;
	}

	const D3D12_RECT& ShadowMap2D::GetScissorRect()
	{
		return mScissorRect;
	}

	uint32_t ShadowMap2D::GetShadowMapSize()
	{
		return mSize;
	}

	ShadowMapCube::ShadowMapCube(uint32_t shadowSize, D3D12RHI* rhi)
		:mSize(shadowSize)
	{
		mRenderTarget = std::make_unique<RenderTargetCube>(rhi, true, shadowSize, DXGI_FORMAT_R24G8_TYPELESS);

		mViewport = { 0.0f, 0.0f, (float)shadowSize, (float)shadowSize, 0.0f, 1.0f };

		mScissorRect = { 0, 0, (int)(shadowSize), (int)(shadowSize) };
	}

	void ShadowMapCube::SetView(const Vector3& eyePosition, float zNear, float zFar)
	{
		/*
		Vector3 Targets[6] =
		{
			eyePosition + Vector3(+1.0f, 0.0f, 0.0f),	// +X 
			eyePosition + Vector3(-1.0f, 0.0f, 0.0f),	// -X
			eyePosition + Vector3(0.0f, +1.0f, 0.0f),	// +Y
			eyePosition + Vector3(0.0f, -1.0f, 0.0f),	// -Y 
			eyePosition + Vector3(0.0f, 0.0f, +1.0f),	// +X 
			eyePosition + Vector3(0.0f, 0.0f, -1.0f),	// -X 
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
		*/
		
		Vector3 Targets[6] =
		{
			eyePosition + Vector3(1.0f,  0.0f,  0.0f), // +X 
			eyePosition + Vector3(-1.0f, 0.0f,  0.0f), // -X 
			eyePosition + Vector3(0.0f,  1.0f,  0.0f), // +Y 
			eyePosition + Vector3(0.0f,  -1.0f, 0.0f), // -Y 
			eyePosition + Vector3(0.0f,  0.0f,  1.0f), // +Z 
			eyePosition + Vector3(0.0f,  0.0f, -1.0f)  // -Z 
		};

		Vector3 Ups[6] =
		{
			{0.0f, 1.0f, 0.0f},  // +X 
			{0.0f, 1.0f, 0.0f},  // -X 
			{0.0f, 0.0f, -1.0f}, // +Y 
			{0.0f, 0.0f, +1.0f}, // -Y 
			{0.0f, 1.0f, 0.0f},	 // +Z 
			{0.0f, 1.0f, 0.0f}	 // -Z 
		};
		
		for (int i = 0; i < 6; ++i)
		{
			mViewMat[i] = Math::MakeLookAtMatrix(eyePosition, Targets[i], Ups[i]);
		}
		float Fov = 0.5f * Math_PI;
		float AspectRatio = 1.0f; //Square
		mProjMat = Math::MakePerspectiveMatrix(Radian(Fov), AspectRatio, zNear, zFar);
	}

	Matrix4x4 ShadowMapCube::GetSceneView(uint32_t i)
	{
		return mViewMat[i];
	}

	Matrix4x4 ShadowMapCube::GetSceneProj()
	{
		return mProjMat;
	}

	RenderTargetCube* ShadowMapCube::GetTarget()
	{
		return mRenderTarget.get();
	}

	const D3D12_VIEWPORT& ShadowMapCube::GetViewport()
	{
		return mViewport;
	}

	const D3D12_RECT& ShadowMapCube::GetScissorRect()
	{
		return mScissorRect;
	}

	uint32_t ShadowMapCube::GetShadowMapSize()
	{
		return mSize;
	}
}

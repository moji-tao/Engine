#include "EngineRuntime/include/Function/Render/RenderResource.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Function/Render/TAASampler.h"

namespace Engine
{
	void RenderResource::UploadGameObjectRenderResource(const GUID& refMesh, std::vector<GUID> refMaterials, const ObjectConstants& gConstance)
	{
		mResource.emplace_back(refMesh, refMaterials, gConstance);
	}

	void RenderResource::UploadDirectionalLight(DirectionalLight& info)
	{
		info.ShadowIndex = -1;
		mDirectionalLightResource.emplace_back(info);
	}

	void RenderResource::UploadPointLight(PointLight& info)
	{
		info.ShadowIndex = -1;
		mPointLightResource.emplace_back(info);
	}

	void RenderResource::UploadSpotLight(SpotLight& info)
	{
		info.ShadowIndex = -1;
		mSpotLightResource.emplace_back(info);
	}

	void RenderResource::UploadDirectionalLightAndShadow(DirectionalLight& info, ShadowParameter shadowParameter)
	{
		if (m2DShadowMapCount == MAX_2DSHADOWMAP)
		{
			UploadDirectionalLight(info);
		}
		else
		{
			info.ShadowIndex = (int32_t)m2DShadowMapCount;
			mDirectionalLightResource.emplace_back(info);
			shadowParameter.Type = ShadowParameter::emShadowMap2D;
			mShaderParameter.push_back(shadowParameter);
			++m2DShadowMapCount;
		}
	}

	void RenderResource::UploadPointLightAndShadow(PointLight& info, ShadowParameter shadowParameter)
	{
		if (mCubeShadowMapCount == MAX_CubeSHADOWMAP)
		{
			UploadPointLight(info);
		}
		else
		{
			info.ShadowIndex = (int32_t)mCubeShadowMapCount;
			mPointLightResource.emplace_back(info);
			shadowParameter.Type = ShadowParameter::emShadowMapCube;
			mShaderParameter.push_back(shadowParameter);
			++mCubeShadowMapCount;
		}
	}

	void RenderResource::UploadSpotLightAndShadow(SpotLight& info, ShadowParameter shadowParameter)
	{
		if (m2DShadowMapCount == MAX_2DSHADOWMAP)
		{
			UploadSpotLight(info);
		}
		else
		{
			info.ShadowIndex = (int32_t)m2DShadowMapCount;
			mSpotLightResource.emplace_back(info);
			shadowParameter.Type = ShadowParameter::emShadowMap2D;
			mShaderParameter.push_back(shadowParameter);
			++m2DShadowMapCount;
		}
	}

	void RenderResource::PerFrameBuffer(const RenderCamera* camera, float deltaTile)
	{
		UpdateMainPassCB(camera, deltaTile);

		UpdateObjectCBs();
	}

	std::vector<ShadowParameter>& RenderResource::GetShadowParameters()
	{
		return mShaderParameter;
	}

	void RenderResource::EndFrameBuffer()
	{
		mResource.clear();
		mDirectionalLightResource.clear();
		mPointLightResource.clear();
		mSpotLightResource.clear();
		mShaderParameter.clear();
		m2DShadowMapCount = 0;
		mCubeShadowMapCount = 0;
	}

	const CameraPassConstants* RenderResource::GetCameraPass()
	{
		return &mMainCameraPassCB;
	}

	void RenderResource::UpdateObjectCBs()
	{

	}

	void RenderResource::UpdateMainPassCB(const RenderCamera* camera, float deltaTile)
	{
		mMainCameraPassCB.RenderTargetSize = camera->GetRenderSize();

		mMainCameraPassCB.View = camera->GetViewMatrix();
		mMainCameraPassCB.InvView = mMainCameraPassCB.View.Inverse();

		mMainCameraPassCB.Proj = camera->GetPersProjMatrix();
		mMainCameraPassCB.InvProj = mMainCameraPassCB.Proj.Inverse();

		if (RenderSystem::GetInstance()->mEnableTAA)
		{
			mMainCameraPassCB.Jitter = Vector2(Halton_2[mMainCameraPassCB.FrameCount % TAA_SAMPLER_COUNT], Halton_3[mMainCameraPassCB.FrameCount % TAA_SAMPLER_COUNT]);
		}
		else
		{
			mMainCameraPassCB.Jitter = Vector2::ZERO;
		}

		mMainCameraPassCB.JitterProj = mMainCameraPassCB.Proj;
		mMainCameraPassCB.JitterProj[2][0] += mMainCameraPassCB.Jitter.x / mMainCameraPassCB.RenderTargetSize.x;
		mMainCameraPassCB.JitterProj[2][1] += mMainCameraPassCB.Jitter.y / mMainCameraPassCB.RenderTargetSize.y;
		mMainCameraPassCB.InvJitterProj = mMainCameraPassCB.JitterProj.Inverse();

		mMainCameraPassCB.PrevViewProj = mMainCameraPassCB.ViewProj;

		mMainCameraPassCB.ViewProj = mMainCameraPassCB.View * mMainCameraPassCB.Proj;
		mMainCameraPassCB.InvViewProj = mMainCameraPassCB.ViewProj.Inverse();

		mMainCameraPassCB.JitterViewProj = mMainCameraPassCB.View * mMainCameraPassCB.JitterProj;
		mMainCameraPassCB.InvJitterViewProj	 = mMainCameraPassCB.JitterViewProj.Inverse();

		mMainCameraPassCB.PrevEyePosW = mMainCameraPassCB.EyePosW;
		mMainCameraPassCB.EyePosW = camera->GetPosition();
		
		mMainCameraPassCB.InvRenderTargetSize = 1.0f / mMainCameraPassCB.RenderTargetSize;
		mMainCameraPassCB.NearZ = camera->mZNear;
		mMainCameraPassCB.FarZ = camera->mZFar;
		mMainCameraPassCB.TotalTime += deltaTile;
		mMainCameraPassCB.DeltaTime = deltaTile;
		++mMainCameraPassCB.FrameCount;
	}

}

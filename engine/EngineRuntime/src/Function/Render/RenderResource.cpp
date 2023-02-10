#include "EngineRuntime/include/Function/Render/RenderResource.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Function/Render/TAASampler.h"

namespace Engine
{
	void RenderResource::UploadGameObjectRenderResource(const GUID& refMesh, std::vector<GUID> refMaterials, const ObjectConstants& gConstance)
	{
		mResource.emplace_back(refMesh, refMaterials, gConstance);
	}

	void RenderResource::UploadDirectionalLight(const DirectionalLight& info)
	{
		mDirectionalLightResource.emplace_back(info);
	}

	void RenderResource::UploadPointLight(const PointLight& info)
	{
		mPointLightResource.emplace_back(info);
	}

	void RenderResource::UploadSpotLight(const SpotLight& info)
	{
		mSpotLightResource.emplace_back(info);
	}

	void RenderResource::PerFrameBuffer(const RenderCamera* camera, float deltaTile)
	{
		UpdateMainPassCB(camera, deltaTile);

		UpdateObjectCBs();
	}

	void RenderResource::EndFrameBuffer()
	{
		mResource.clear();
		mDirectionalLightResource.clear();
		mPointLightResource.clear();
		mSpotLightResource.clear();
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

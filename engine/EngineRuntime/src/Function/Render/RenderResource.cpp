#include "EngineRuntime/include/Function/Render/RenderResource.h"

namespace Engine
{
	void RenderResource::UploadGlobalRenderResource(std::shared_ptr<RHI> rhi)
	{

	}

	void RenderResource::UploadGameObjectRenderResource(std::shared_ptr<RHI> rhi)
	{

	}

	/*
	void RenderResource::UploadPerFrameBuffer(std::shared_ptr<RHI> rhi, std::shared_ptr<RenderCamera> camera)
	{
		UpdateMainPassCB(camera);
		rhi->UploadMainPassData(mMainPassCB);

		UpdateObjectCBs();
	}
	*/

	void RenderResource::UpdateObjectCBs()
	{

	}

	void RenderResource::UpdateMainPassCB(std::shared_ptr<RenderCamera> camera)
	{
		mMainPassCB.View = camera->GetViewMatrix();
		mMainPassCB.InvView = mMainPassCB.View.Inverse();
		mMainPassCB.Proj = camera->GetPersProjMatrix();
		mMainPassCB.InvProj = mMainPassCB.Proj.Inverse();
		mMainPassCB.ViewProj = (mMainPassCB.Proj * mMainPassCB.View);
		mMainPassCB.InvViewProj = mMainPassCB.ViewProj.Inverse();
		mMainPassCB.EyePosW = camera->GetPosition();
		//mMainPassCB.RenderTargetSize = Vector2((float)mClientWidth, (float)mClientHeight);;
		//mMainPassCB.InvRenderTargetSize = Vector2(1.0f / mClientWidth, 1.0f / mClientHeight);
		mMainPassCB.NearZ = camera->mZNear;
		mMainPassCB.FarZ = camera->mZFar;
		mMainPassCB.TotalTime = 0.0f;
		mMainPassCB.DeltaTime = 0.0f;
	}

}

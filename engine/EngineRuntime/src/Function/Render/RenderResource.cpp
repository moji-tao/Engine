#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Function/Render/RenderResource.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Function/Render/TAASampler.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

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

	void RenderResource::UploadPointLightAndShadow(PointLight& info)
	{
		if (mCubeShadowMapCount == MAX_CubeSHADOWMAP)
		{
			UploadPointLight(info);
		}
		else
		{
			info.ShadowIndex = (int32_t)mCubeShadowMapCount;
			mPointLightResource.emplace_back(info);

			ShadowParameter shadowParameter;
			shadowParameter.LightPosition = info.Position;
			shadowParameter.LightRange = info.Range;
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

		UpdateCameraFrustum();

		UpdateObjectCBs();


	}

	std::vector<ShadowParameter>& RenderResource::GetShadowParameters()
	{
		return mShaderParameter;
	}

	void RenderResource::EndFrameBuffer()
	{
		mCameraRenderResource.clear();
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
		for (auto& [refMesh, refMaterials, constant] : mResource)
		{
			if (!refMesh.IsVaild())
			{
				continue;
			}
			Mesh* mesh = AssetManager::GetInstance()->LoadResource<Mesh>(refMesh);

			if (mesh == nullptr)
			{
				LOG_ERROR("引用的Mesh资源不存在 {0}", refMesh.Data());
				continue;
			}

			std::vector<SubMesh>& subMeshes = mesh->Meshes;

			int end = Math::Min(subMeshes.size(), refMaterials.size());

			for (int i = 0; i < end; ++i)
			{
				if (!refMaterials[i].IsVaild())
				{
					continue;
				}

				MaterialData* material = AssetManager::GetInstance()->LoadResource<MaterialData>(refMaterials[i]);

				if (material == nullptr)
				{
					LOG_ERROR("引用的Material资源不存在 {0}", refMaterials[i].Data());
					continue;
				}

				// 视锥体剔除

				//subMeshes[i].

				//subMeshes[i].mBoundingBox.mMin

				BoundingBox box;
				subMeshes[i].mBoundingBox.Transform(box, constant.World);

				if (mCameraFrustum.Contains(box) == BoundingFrustum::DISJOINT)
				{
					continue;
				}

				// End

				mCameraRenderResource[material][&subMeshes[i]].push_back(constant);
			}
		}

		mResource.clear();
	}

	void RenderResource::UpdateCameraFrustum()
	{
		Vector4 homogenousPoints[8] =
		{
			Vector4(-1.0f, +1.0f, 1.0f, 1.0f),	// 远平面左上点 0
			Vector4(-1.0f, -1.0f, 1.0f, 1.0f),	// 远平面左下点 1
			Vector4(+1.0f, +1.0f, 1.0f, 1.0f),	// 远平面右上点 2
			Vector4(+1.0f, -1.0f, 1.0f, 1.0f),	// 远平面右下点 3
			Vector4(-1.0f, +1.0f, 0.0f, 1.0f),	// 近平面左上点 4
			Vector4(-1.0f, -1.0f, 0.0f, 1.0f),	// 近平面左下点 5
			Vector4(+1.0f, +1.0f, 0.0f, 1.0f),	// 近平面右上点 6
			Vector4(+1.0f, -1.0f, 0.0f, 1.0f),	// 近平面右下点 7
		};

		for (int i = 0; i < 8; ++i)
		{
			homogenousPoints[i] = homogenousPoints[i] * mMainCameraPassCB.InvViewProj;
			homogenousPoints[i] /= homogenousPoints[i].w;
		}

		mCameraFrustum.mRight.mNormal = Vector3(homogenousPoints[3] - homogenousPoints[2]).CrossProduct(Vector3(homogenousPoints[2] - homogenousPoints[6])).GetNormalised();
		mCameraFrustum.mRight.mPoint = Vector3(homogenousPoints[2]);
		mCameraFrustum.mLeft.mNormal = Vector3(homogenousPoints[5] - homogenousPoints[4]).CrossProduct(Vector3(homogenousPoints[4] - homogenousPoints[0])).GetNormalised();
		mCameraFrustum.mLeft.mPoint = Vector3(homogenousPoints[4]);
		mCameraFrustum.mTop.mNormal = Vector3(homogenousPoints[2] - homogenousPoints[0]).CrossProduct(Vector3(homogenousPoints[0] - homogenousPoints[4])).GetNormalised();
		mCameraFrustum.mTop.mPoint = Vector3(homogenousPoints[0]);
		mCameraFrustum.mBottom.mNormal = Vector3(homogenousPoints[5] - homogenousPoints[1]).CrossProduct(Vector3(homogenousPoints[1] - homogenousPoints[3])).GetNormalised();
		mCameraFrustum.mBottom.mPoint = Vector3(homogenousPoints[1]);
		mCameraFrustum.mNear.mNormal = Vector3(homogenousPoints[6] - homogenousPoints[4]).CrossProduct(Vector3(homogenousPoints[4] - homogenousPoints[5])).GetNormalised();
		mCameraFrustum.mNear.mPoint = Vector3(homogenousPoints[4]);
		mCameraFrustum.mFar.mNormal = Vector3(homogenousPoints[1] - homogenousPoints[0]).CrossProduct(Vector3(homogenousPoints[0] - homogenousPoints[2])).GetNormalised();
		mCameraFrustum.mFar.mPoint = Vector3(homogenousPoints[0]);

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

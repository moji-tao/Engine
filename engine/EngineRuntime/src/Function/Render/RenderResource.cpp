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

	/*
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
			mShadowParameter.push_back(shadowParameter);
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
			mShadowParameter.push_back(shadowParameter);
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
			mShadowParameter.push_back(shadowParameter);
			++m2DShadowMapCount;
		}
	}
	*/

	void RenderResource::UploadDirectionalLight(const DirectionalLightActorParameter& parameter)
	{
		mCurrentTickDirectionalLight.push_back(parameter);
	}

	void RenderResource::UploadPointLight(const PointLightActorParameter& parameter)
	{
		mCurrentTickPointLight.push_back(parameter);
	}

	void RenderResource::PerFrameBuffer(const RenderCamera* camera, float deltaTile)
	{
		UpdateMainPassCB(camera, deltaTile);

		UpdateCameraFrustum();

		UpdateObjectCBs();

		UpdateLight();
	}

	std::vector<ShadowParameter>& RenderResource::GetShadowParameters()
	{
		return mShadowParameter;
	}

	void RenderResource::EndFrameBuffer()
	{
		mCameraRenderResource.clear();
		mDirectionalLightResource.clear();
		mPointLightResource.clear();
		mSpotLightResource.clear();
		mShadowParameter.clear();
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

				if (RenderSystem::GetInstance()->mEnableCulling)
				{
					BoundingBox box;
					subMeshes[i].mBoundingBox.Transform(box, constant.World);

					if (mCameraFrustum.Contains(box) == BoundingFrustum::DISJOINT)
					{
						continue;
					}
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
			mCameraFrustum.mPoint[i] = Vector3(homogenousPoints[i]);
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

	void RenderResource::UpdateLight()
	{
		// 平行光
		// 级联阴影划分
		std::vector<float> layer = { 0.0f, 0.07f, 0.14f, 0.26f, 0.53f };
		std::vector<uint32_t> shadowMapSize = { 0, 4096, 2048, 1024, 512 };
		for (int i = 0; i < mCurrentTickDirectionalLight.size(); ++i)
		{
			if (!mCurrentTickDirectionalLight[i].ShowShadow || (m2DShadowMapCount + 4) >= MAX_2DSHADOWMAP)
			{
				DirectionalLight light;
				light.Direction = mCurrentTickDirectionalLight[i].Direction;
				light.Intensity = mCurrentTickDirectionalLight[i].Intensity;
				light.ShadowIndex[0] = -1;
				light.Color = mCurrentTickDirectionalLight[i].Color;
				mDirectionalLightResource.push_back(light);
				continue;
			}


			DirectionalLight light;
			light.Direction = mCurrentTickDirectionalLight[i].Direction;
			light.Intensity = mCurrentTickDirectionalLight[i].Intensity;
			light.Color = mCurrentTickDirectionalLight[i].Color;

			for (int j = 1; j < layer.size(); ++j)
			{
				/*
				Vector3 leftTopFar = Math::Lerp(mCameraFrustum.mLeftTopNear, mCameraFrustum.mLeftTopFar, layer[i]);
				Vector3 leftBottomFar = Math::Lerp(mCameraFrustum.mLeftBottomNear, mCameraFrustum.mLeftBottomFar, layer[i]);
				Vector3 rightTopFar = Math::Lerp(mCameraFrustum.mRightTopNear, mCameraFrustum.mRightTopFar, layer[i]);
				Vector3 rightBottomFar = Math::Lerp(mCameraFrustum.mRightBottomNear, mCameraFrustum.mRightBottomFar, layer[i]);
				Vector3 leftTopNear = Math::Lerp(mCameraFrustum.mLeftTopNear, mCameraFrustum.mLeftTopFar, layer[i - 1]);
				Vector3 leftBottomNear = Math::Lerp(mCameraFrustum.mLeftBottomNear, mCameraFrustum.mLeftBottomFar, layer[i - 1]);
				Vector3 rightTopNear = Math::Lerp(mCameraFrustum.mRightTopNear, mCameraFrustum.mRightTopFar, layer[i - 1]);
				Vector3 rightBottomNear = Math::Lerp(mCameraFrustum.mRightBottomNear, mCameraFrustum.mRightBottomFar, layer[i - 1]);
				*/
				Vector3 point[8];
				for (int i = 0; i < 4; ++i)
				{
					point[i] = Math::Lerp(mCameraFrustum.mPoint[i + 4], mCameraFrustum.mPoint[i], layer[j]);
					point[i + 4] = Math::Lerp(mCameraFrustum.mPoint[i + 4], mCameraFrustum.mPoint[i], layer[j - 1]);
				}

				Matrix4x4 shadowView = Math::MakeLookAtMatrix(Vector3::ZERO, mCurrentTickDirectionalLight[i].Direction, Vector3::UNIT_Y);
				Matrix4x4 shadowViewInv = shadowView.Inverse();

				for (int i = 0; i < 4; ++i)
				{
					point[i] = Vector3(Vector4(point[i], 1.0f) * shadowView);
					point[i + 4] = Vector3(Vector4(point[i + 4], 1.0f) * shadowView);
				}

				float xMin = FLT_MAX;
				float yMin = FLT_MAX;
				float zMin = FLT_MAX;
				float xMax = FLT_MIN;
				float yMax = FLT_MIN;
				float zMax = FLT_MIN;
				for (int i = 0; i < 8; ++i)
				{
					xMin = Math::Min(point[i].x, xMin);
					xMax = Math::Max(point[i].x, xMax);
					yMin = Math::Min(point[i].y, yMin);
					yMax = Math::Max(point[i].y, yMax);
					zMin = Math::Min(point[i].z, zMin);
					zMax = Math::Max(point[i].z, zMax);
				}

				// 光锥AABB
				point[0] = Vector3(Vector4(xMin, yMin, zMin, 1.0f) * shadowViewInv); // 左下近
				point[1] = Vector3(Vector4(xMin, yMin, zMax, 1.0f) * shadowViewInv); // 左下远
				point[2] = Vector3(Vector4(xMin, yMax, zMin, 1.0f) * shadowViewInv); // 左上近
				point[3] = Vector3(Vector4(xMin, yMax, zMax, 1.0f) * shadowViewInv); // 左上远
				point[4] = Vector3(Vector4(xMax, yMin, zMin, 1.0f) * shadowViewInv); // 右下近
				point[5] = Vector3(Vector4(xMax, yMin, zMax, 1.0f) * shadowViewInv); // 右下远
				point[6] = Vector3(Vector4(xMax, yMax, zMin, 1.0f) * shadowViewInv); // 右上近
				point[7] = Vector3(Vector4(xMax, yMax, zMax, 1.0f) * shadowViewInv); // 右上远

				{
					float width = point[1].Distance(point[5]);
					float height = point[1].Distance(point[3]);
					float zFar = point[0].Distance(point[1]);

					Vector3 lightTarget = (point[0] + point[7]) * 0.5f;
					Vector3 lightPos = (point[0] + point[6]) * 0.5f;

					ShadowParameter shadowParameter;
					shadowParameter.Type = ShadowParameter::emDirectionalShadowMap;
					shadowParameter.LightPosition = lightPos;
					shadowParameter.ShadowMapSize = shadowMapSize[j];
					shadowParameter.ViewProj = Math::MakeLookAtMatrix(lightPos, lightTarget, Vector3::UNIT_Y) * 
						Math::MakeOrthographicProjectionMatrix(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, 0.001f, zFar);
					mShadowParameter.push_back(shadowParameter);
					
					light.ShadowIndex[j - 1] = (int32_t)m2DShadowMapCount;
					light.LightViewProj[j - 1] = shadowParameter.ViewProj;
					++m2DShadowMapCount;
				}
			}
			light.Layer[0] = layer[1];
			light.Layer[1] = layer[2];
			light.Layer[2] = layer[3];
			light.Layer[3] = layer[4];
			mDirectionalLightResource.push_back(light);
		}

		// 取视锥体的前一部分为投影矩阵

		// 点光源

		for (int i = 0; i < mCurrentTickPointLight.size(); ++i)
		{
			BoundingSphere bounding;
			bounding.mCenter = mCurrentTickPointLight[i].Position;
			bounding.mRadius = mCurrentTickPointLight[i].Range;
			if (mCameraFrustum.Contains(bounding) == BoundingFrustum::DISJOINT)
			{
				continue;
			}

			PointLight light;
			light.ShadowIndex = -1;

			if (mCurrentTickPointLight[i].ShowShadow || mCubeShadowMapCount >= MAX_CubeSHADOWMAP)
			{
				ShadowParameter shadowParameter;
				shadowParameter.Type = ShadowParameter::emPointShadowMap;
				shadowParameter.LightPosition = mCurrentTickPointLight[i].Position;
				shadowParameter.LightRange = mCurrentTickPointLight[i].Range;
				shadowParameter.ShadowMapSize = SHADOWMAPCUBE_SIZE;
				mShadowParameter.push_back(shadowParameter);

				light.ShadowIndex = (int32_t)mCubeShadowMapCount;
				++mCubeShadowMapCount;
			}

			light.Position = mCurrentTickPointLight[i].Position;
			light.Range = mCurrentTickPointLight[i].Range;
			light.Color = mCurrentTickPointLight[i].Color;
			light.Intensity = mCurrentTickPointLight[i].Intensity;

			mPointLightResource.push_back(light);
		}

		mCurrentTickDirectionalLight.clear();

		mCurrentTickPointLight.clear();
	}

}

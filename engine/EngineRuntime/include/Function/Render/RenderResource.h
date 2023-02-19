#pragma once
#include <unordered_map>
#include "EngineRuntime/include/Core/Random/GUID.h"
#include "EngineRuntime/include/Function/Render/RenderCamera.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"

#define MAX_2DSHADOWMAP 10
#define MAX_CubeSHADOWMAP 5
#define SHADOWMAP2D_SIZE 4096
#define SHADOWMAPCUBE_SIZE 1024

namespace Engine
{
	struct ObjectConstants
	{
		Matrix4x4 World = Matrix4x4::IDENTITY;
		Matrix4x4 InvWorld = Matrix4x4::IDENTITY;
		Matrix4x4 PreWorld = World;
	};

	struct CameraPassConstants
	{
		Matrix4x4 View = Matrix4x4::IDENTITY;
		Matrix4x4 InvView = Matrix4x4::IDENTITY;
		Matrix4x4 Proj = Matrix4x4::IDENTITY;
		Matrix4x4 InvProj = Matrix4x4::IDENTITY;
		Matrix4x4 JitterProj = Matrix4x4::IDENTITY;
		Matrix4x4 InvJitterProj = Matrix4x4::IDENTITY;
		Matrix4x4 ViewProj = Matrix4x4::IDENTITY;
		Matrix4x4 InvViewProj = Matrix4x4::IDENTITY;
		Matrix4x4 PrevViewProj = Matrix4x4::IDENTITY;
		Matrix4x4 JitterViewProj = Matrix4x4::IDENTITY;
		Matrix4x4 InvJitterViewProj = Matrix4x4::IDENTITY;
		Vector2 Jitter = Vector2::ZERO;
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		Vector3 EyePosW = Vector3::ZERO;
		float TotalTime = 0.0f;
		Vector3 PrevEyePosW = Vector3::ZERO;
		float DeltaTime = 0.0f;
		Vector2 RenderTargetSize = Vector2::ZERO;
		Vector2 InvRenderTargetSize = Vector2::ZERO;
		uint32_t FrameCount = 0;
	};

	struct DirectionalLight
	{
		Matrix4x4 LightViewProj[4];
		Vector4 Color;
		Vector3 Direction;
		float Intensity;
		int32_t ShadowIndex[4] = { -1,-1,-1,-1 };
		float Layer[4] = { 0.07f, 0.14f, 0.26f, 0.53f };
	};

	struct PointLight
	{
		Vector4 Color;
		Vector3 Position;
		float Range;
		float Intensity;
		int32_t ShadowIndex;
	};

	struct SpotLight
	{
		Matrix4x4 LightViewProj;
		Vector4 Color;
		Vector3 Position;
		float Light;
		float Angle;
		float Intensity;
		int32_t ShadowIndex;
	};

	struct ShadowParameter
	{
		enum EMShadowMapType : uint32_t
		{
			emDirectionalShadowMap,
			emSpotShadowMap,
			emPointShadowMap,
		};
		Matrix4x4 ViewProj;
		Vector3 LightPosition;
		float LightRange;
		uint32_t ShadowMapSize;
		EMShadowMapType Type;
	};

	struct LightActorParameter
	{
		Vector4 Color;
		float Intensity;
		bool ShowShadow;
	};

	struct DirectionalLightActorParameter : public LightActorParameter
	{
		Vector3 Direction;
	};

	struct PointLightActorParameter : public LightActorParameter
	{
		Vector3 Position;
		float Range;
	};

	struct OmnidirectionalShadowParameter
	{
		Matrix4x4 View[6];
		Matrix4x4 Proj;
		Vector3 LightPosition;
		float LightRange;
	};

	struct DirectionalShadowParameter
	{
		Matrix4x4 ViewProj;
	};

	struct LightCommonData
	{
		uint32_t DirectionalLightCount = 0;
		uint32_t PointLightCount = 0;
		uint32_t SpotLightCount = 0;
		uint32_t EnableSSAO = 0;
		uint32_t EnableAmbientLighting = 0;
	};

	struct BlurSettingsConstants
	{
		int gBlurRadius;

		float w[11];
	};

	class RenderResource
	{
	public:
		RenderResource() = default;

		virtual ~RenderResource() = default;

	public:
		void UploadGameObjectRenderResource(const GUID& refMesh, std::vector<GUID> refMaterials, const ObjectConstants& gConstance);

		//void UploadDirectionalLight(DirectionalLight& info);

		//void UploadPointLight(PointLight& info);

		//void UploadSpotLight(SpotLight& info);

		//void UploadDirectionalLightAndShadow(DirectionalLight& info, ShadowParameter shadowParameter);

		//void UploadPointLightAndShadow(PointLight& info);

		//void UploadSpotLightAndShadow(SpotLight& info, ShadowParameter shadowParameter);

		void UploadDirectionalLight(const DirectionalLightActorParameter& parameter);

		void UploadPointLight(const PointLightActorParameter& parameter);

		void PerFrameBuffer(const RenderCamera* camera, float deltaTile);

		std::vector<ShadowParameter>& GetShadowParameters();

		virtual void EndFrameBuffer();

		const CameraPassConstants* GetCameraPass();

	private:
		void UpdateObjectCBs();

		void UpdateCameraFrustum();

		void UpdateMainPassCB(const RenderCamera* camera, float deltaTile);

		void UpdateLight();

	protected:
		CameraPassConstants mMainCameraPassCB;

		// Mesh Material Constants
		std::vector<std::tuple<GUID, std::vector<GUID>, ObjectConstants>> mResource;

		std::unordered_map<MaterialData*, std::unordered_map<SubMesh*, std::vector<ObjectConstants>>> mCameraRenderResource;

	protected:
		LightCommonData mLightCommonData;

		std::vector<DirectionalLight> mDirectionalLightResource;

		std::vector<PointLight> mPointLightResource;

		std::vector<SpotLight> mSpotLightResource;

		std::vector<ShadowParameter> mShadowParameter;

		uint32_t m2DShadowMapCount = 0;
		uint32_t mCubeShadowMapCount = 0;

	private:
		BoundingFrustum mCameraFrustum;

		std::vector<DirectionalLightActorParameter> mCurrentTickDirectionalLight;
		std::vector<PointLightActorParameter> mCurrentTickPointLight;
	};
}

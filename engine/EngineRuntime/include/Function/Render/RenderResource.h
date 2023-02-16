#pragma once
#include "EngineRuntime/include/Core/Random/GUID.h"
#include "EngineRuntime/include/Function/Render/RenderCamera.h"

#define MAX_2DSHADOWMAP 10
#define MAX_CubeSHADOWMAP 5
#define SHADOWMAP_SIZE 4096

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
		Matrix4x4 LightViewProj;
		Vector4 Color;
		Vector3 Direction;
		float Intensity;
		int32_t ShadowIndex;
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
			emShadowMap2D,
			emShadowMapCube,
		};
		Matrix4x4 View;
		Matrix4x4 Proj;
		Vector3 LightPosition;
		float LightRange;
		EMShadowMapType Type;
	};

	struct LightCommonData
	{
		uint32_t DirectionalLightCount = 0;
		uint32_t PointLightCount = 0;
		uint32_t SpotLightCount = 0;
		uint32_t EnableSSAO = 0;
		uint32_t EnableAmbientLighting = 0;
	};

	class RenderResource
	{
	public:
		RenderResource() = default;

		virtual ~RenderResource() = default;

	public:
		void UploadGameObjectRenderResource(const GUID& refMesh, std::vector<GUID> refMaterials, const ObjectConstants& gConstance);

		void UploadDirectionalLight(DirectionalLight& info);

		void UploadPointLight(PointLight& info);

		void UploadSpotLight(SpotLight& info);

		void UploadDirectionalLightAndShadow(DirectionalLight& info, ShadowParameter shadowParameter);

		void UploadPointLightAndShadow(PointLight& info, ShadowParameter shadowParameter);

		void UploadSpotLightAndShadow(SpotLight& info, ShadowParameter shadowParameter);

		void PerFrameBuffer(const RenderCamera* camera, float deltaTile);

		std::vector<ShadowParameter>& GetShadowParameters();

		virtual void EndFrameBuffer();

		const CameraPassConstants* GetCameraPass();

	private:
		void UpdateObjectCBs();

		void UpdateMainPassCB(const RenderCamera* camera, float deltaTile);

	protected:
		CameraPassConstants mMainCameraPassCB;

		std::vector<std::tuple<GUID, std::vector<GUID>, ObjectConstants>> mResource;

	protected:
		LightCommonData mLightCommonData;

		std::vector<DirectionalLight> mDirectionalLightResource;

		std::vector<PointLight> mPointLightResource;

		std::vector<SpotLight> mSpotLightResource;

		std::vector<ShadowParameter> mShaderParameter;

		uint32_t m2DShadowMapCount = 0;
		uint32_t mCubeShadowMapCount = 0;
	};
}

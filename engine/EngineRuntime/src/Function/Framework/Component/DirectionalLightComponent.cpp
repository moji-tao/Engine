#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Function/Framework/Component/DirectionalLightComponent.h"
#include "EngineRuntime/include/Function/Render/RenderResource.h"

namespace Engine
{
	DirectionalLightComponent::DirectionalLightComponent()
		:LightComponent()
	{ }

	DirectionalLightComponent::~DirectionalLightComponent()
	{ }

	void DirectionalLightComponent::Tick(float deltaTime)
	{
		auto transform_component = mParentObject->GetComponent<TransformComponent>();
		
		RenderResource* resource = RenderSystem::GetInstance()->GetRenderResource();

		ASSERT(resource != nullptr && transform_component != nullptr);

		DirectionalLight info;

		Quaternion globalQuaternion = transform_component->GetGlobalQuaternion();

		info.Color = mColor;
		info.Direction = globalQuaternion * Vector3::UNIT_Z;
		info.Intensity = mIntensity;

		if (mShadow)
		{
			ShadowParameter shadowParameter;

			float rad = 5.0f;
			Vector3 lightPos = -info.Direction * rad;
			Vector3 target = Vector3::ZERO;
			Vector3 lightUp = Vector3::UNIT_Y;
			
			shadowParameter.View = Math::MakeLookAtMatrix(lightPos, target, lightUp);
			Vector4 sphereCL = Vector4(lightPos, 1.0f) * shadowParameter.View;
			float l = sphereCL.x - rad;
			float b = sphereCL.y - rad;
			float n = 0.01f;
			float r = sphereCL.x + rad;
			float t = sphereCL.y + rad;
			float f = sphereCL.z + rad - (sphereCL.z - rad);
			
			shadowParameter.Proj = Math::MakeOrthographicProjectionMatrix(l, r, b, t, n, f);

			info.LightViewProj = shadowParameter.View * shadowParameter.Proj;
			
			
			resource->UploadDirectionalLightAndShadow(info, shadowParameter);
		}
		else
		{
			resource->UploadDirectionalLight(info);
		}
	}

	void DirectionalLightComponent::Serialize(SerializerDataFrame& stream) const
	{
		LightComponent::Serialize(stream);

	}

	bool DirectionalLightComponent::Deserialize(SerializerDataFrame& stream)
	{
		LightComponent::Deserialize(stream);

		return true;
	}

	void DirectionalLightComponent::CloneData(GameObject* node)
	{
		LightComponent::CloneData(node);

	}

}

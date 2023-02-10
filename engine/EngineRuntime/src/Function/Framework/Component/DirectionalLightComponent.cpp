#include "EngineRuntime/include/Function/Framework/Component/DirectionalLightComponent.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
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

		info.Color = mColor;
		info.Direction = transform_component->GetGlobalQuaternion() * Vector3::UNIT_Z;
		info.Intensity = mIntensity;

		resource->UploadDirectionalLight(info);
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

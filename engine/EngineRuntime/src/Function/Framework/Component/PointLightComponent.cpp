#include "EngineRuntime/include/Function/Framework/Component/PointLightComponent.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"

namespace Engine
{
	PointLightComponent::PointLightComponent()
		:LightComponent(), mRange(1.0f)
	{ }

	PointLightComponent::~PointLightComponent()
	{ }

	void PointLightComponent::Tick(float deltaTime)
	{
		auto transform_component = mParentObject->GetComponent<TransformComponent>();
		RenderResource* resource = RenderSystem::GetInstance()->GetRenderResource();

		ASSERT(resource != nullptr && transform_component != nullptr);

		PointLight info;
		info.Color = mColor;
		info.Intensity = mIntensity;
		info.Position = transform_component->GetGlobalPosition();
		info.Range = mRange;

		if (mShadow)
		{
			resource->UploadPointLightAndShadow(info);
		}
		else
		{
			resource->UploadPointLight(info);
		}
	}

	void PointLightComponent::Serialize(SerializerDataFrame& stream) const
	{
		LightComponent::Serialize(stream);

		stream << mRange;
	}

	bool PointLightComponent::Deserialize(SerializerDataFrame& stream)
	{
		LightComponent::Deserialize(stream);

		stream >> mRange;

		return true;
	}

	void PointLightComponent::CloneData(GameObject* node)
	{
		LightComponent::CloneData(node);
	}
}

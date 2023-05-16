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

	void PointLightComponent::Awake()
	{
	}

	void PointLightComponent::Tick(float deltaTime)
	{
		if (!mIsEnable)
		{
			return;
		}

		auto transform_component = mParentObject->GetComponent<TransformComponent>();
		RenderResource* resource = RenderSystem::GetInstance()->GetRenderResource();

		ASSERT(resource != nullptr && transform_component != nullptr);

		PointLightActorParameter parameter;
		parameter.Color = mColor;
		parameter.Intensity = mIntensity;
		parameter.ShowShadow = mShadow;
		parameter.Position = transform_component->GetGlobalPosition();
		parameter.Range = mRange;

		resource->UploadPointLight(parameter);
	}

	void PointLightComponent::OnDestroy()
	{
	}

	void PointLightComponent::OnEnable()
	{
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

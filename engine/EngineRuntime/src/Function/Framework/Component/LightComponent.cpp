#include "EngineRuntime/include/Function/Framework/Component/LightComponent.h"
#include "EngineRuntime/include/Core/Math/Color.h"

namespace Engine
{
	IMPLEMENT_RTTI(LightComponent, Component);

	LightComponent::LightComponent()
		:mColor(Colors::AntiqueWhite), mIntensity(1.0f)
	{ }

	LightComponent::~LightComponent()
	{ }

	void LightComponent::Serialize(SerializerDataFrame& stream) const
	{
		stream << mColor;

		stream << mIntensity;
	}

	bool LightComponent::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mColor;

		stream >> mIntensity;

		return true;
	}

	void LightComponent::CloneData(GameObject* node)
	{
		LightComponent* bluePrint = DynamicCast<LightComponent>(node);

		mColor = bluePrint->mColor;

		mIntensity = bluePrint->mIntensity;
	}

}

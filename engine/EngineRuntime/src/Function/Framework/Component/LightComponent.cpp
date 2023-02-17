#include "EngineRuntime/include/Function/Framework/Component/LightComponent.h"
#include "EngineRuntime/include/Core/Math/Color.h"

namespace Engine
{
	IMPLEMENT_RTTI(LightComponent, Component);

	LightComponent::LightComponent()
		:mColor(Colors::AntiqueWhite), mIntensity(1.0f), mShadow(false)
	{ }

	LightComponent::~LightComponent()
	{ }

	void LightComponent::Serialize(SerializerDataFrame& stream) const
	{
		stream << mColor;

		stream << mIntensity;

		stream << mShadow;
	}

	bool LightComponent::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mColor;

		stream >> mIntensity;

		stream >> mShadow;

		return true;
	}

	void LightComponent::CloneData(GameObject* node)
	{
		LightComponent* bluePrint = DynamicCast<LightComponent>(node);

		mColor = bluePrint->mColor;

		mIntensity = bluePrint->mIntensity;

		mShadow = bluePrint->mShadow;
	}

}

#pragma once
#include "EngineRuntime/include/Function/Framework/Component/Component.h"
#include "EngineRuntime/include/Core/Math/Vector4.h"

namespace Engine
{
	class LightComponent : public Component
	{
		DECLARE_RTTI;
	public:
		LightComponent();

		virtual ~LightComponent() override;

	public:
		virtual void Tick(float deltaTime) override = 0;

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		virtual void CloneData(GameObject* node) override;

	public:
		Vector4 mColor;

		float mIntensity;
	};
}

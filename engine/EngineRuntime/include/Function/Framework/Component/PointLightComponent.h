#pragma once
#include "EngineRuntime/include/Function/Framework/Component/LightComponent.h"

namespace Engine
{
	class PointLightComponent : public LightComponent
	{
		DECLARE_RTTI;
	public:
		PointLightComponent();

		virtual ~PointLightComponent() override;

	public:
		virtual void Tick(float deltaTime) override;

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		virtual void CloneData(GameObject* node) override;

	public:
		float mRange;
	};
}

#pragma once
#include "EngineRuntime/include/Function/Framework/Component/LightComponent.h"

namespace Engine
{
	class DirectionalLightComponent : public LightComponent
	{
		DECLARE_RTTI;
	public:
		DirectionalLightComponent();

		virtual ~DirectionalLightComponent() override;

	public:
		virtual void Tick(float deltaTime) override;


	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		virtual void CloneData(GameObject* node) override;
	};
}

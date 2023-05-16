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
		virtual void Awake() override;

		virtual void Tick(float deltaTime) override;

		virtual void OnDestroy() override;

		virtual void OnEnable() override;

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		virtual void CloneData(GameObject* node) override;

	public:
		float mRange;
	};
}

#pragma once
#include "EngineRuntime/include/Function/Framework/Component/Component.h"

namespace Engine
{
	class ScriptComponent : public Component
	{
		DECLARE_RTTI;
	public:
		ScriptComponent();

		virtual ~ScriptComponent() override;

	public:
		virtual void Awake() override;

		virtual void Tick(float deltaTime) override;

		virtual void OnDestroy() override;

		virtual void OnEnable() override;

	public:
		void ReloadScript();

		void SetScriptGuid(const GUID& guid);

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		virtual void CloneData(GameObject* node) override;
	};
}

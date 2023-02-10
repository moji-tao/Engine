#pragma once
#include "EngineRuntime/include/Function/Framework/Object/GameObject.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Engine
{
	class Actor;
	class TransformComponent;

	class ComponentClassRegister
	{
	public:
		ComponentClassRegister(const std::string& className);
	};

#define REGISTER_COMPONENT(class_name) REGISTER_CLASS(class_name); Engine::ComponentClassRegister REGISTERComponent##class_name##ComponentClassRegister(#class_name);


	class Component : public GameObject
	{
		DECLARE_RTTI;
	public:
		Component();

		virtual ~Component() override;

	public:
		void SetParent(Actor* parentObject);

		virtual void Tick(float deltaTime) = 0;

		virtual void Serialize(SerializerDataFrame& stream) const override = 0;

		virtual bool Deserialize(SerializerDataFrame& stream) override = 0;

		virtual void CloneData(GameObject* node) override = 0;

	protected:
		Actor* mParentObject;

		bool mIsEnable = true;

	protected:
		unsigned mOrder;

		friend class Actor;

	public:
		static std::vector<std::string> ALLInstanceComponentName;
	};
}

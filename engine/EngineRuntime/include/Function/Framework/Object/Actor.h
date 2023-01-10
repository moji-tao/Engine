#pragma once
#include <string>
#include <vector>
#include <list>
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"

namespace Engine
{
	class Level;

	class Actor : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		Actor(Actor* parent = nullptr);

		virtual ~Actor();

	public:
		virtual void Tick(float deltaTime);

		std::string GetActorName();

		void SetActorName(const std::string& name);

		TransformComponent* GetTransform();

		void SetParent(Actor* newParent);

		Actor* GetParent();

		bool IsRootNode();

		uint64_t GetChildrenCount();

		Actor* GetChildren(uint64_t index);

		std::list<Actor*>& GetChildrens();

		template<typename T>
		T* AddComponent();

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		void AddChildrenForTools(Actor* actor);

	protected:
		std::string mObjectName;

		Actor* mParent = nullptr;

		std::list<Actor*> mChildrens;

		std::vector<Component*> mComponents;

		friend Component;
	};

	template<typename T>
	inline T* Actor::AddComponent()
	{
		T* component = new T();
		mComponents.push_back(component);
		return component;
	}

}

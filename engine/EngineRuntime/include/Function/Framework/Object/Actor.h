#pragma once
#include <string>
#include <vector>
#include <list>
#include "EngineRuntime/include/Function/Framework/Object/GameObject.h"
#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"

namespace Engine
{
	class Level;

	class Actor : public GameObject
	{
		DECLARE_RTTI;
	public:
		Actor(Actor* parent = nullptr);

		virtual ~Actor() override;

	public:
		virtual void Tick(float deltaTime);

		std::string GetActorName();

		void SetActorName(const std::string& name);

		void SetParent(Actor* newParent);

		Actor* GetParent();

		bool IsRootNode();

		uint64_t GetChildrenCount();

		Actor* GetChildren(uint64_t index);

		std::list<Actor*>& GetChildrens();

		void SetEnable(bool enable);

		bool GetEnable();

		std::vector<Component*>& GetAllComponent();

		Component* AddComponent(const std::string& componentName);

		template<typename T>
			requires std::is_class<Component>::value
		T* AddComponent();

		Component* GetComponent(const std::string& componentName);

		template<typename T>
			requires std::is_class<Component>::value
		T* GetComponent();

	public:
		static Actor* Instantiate(Actor* prefab);

		static void Destory(Actor* actor);

	private:
		static Actor* BuildActor(Actor* node);

	protected:
		virtual void CloneData(GameObject* node) override;

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		void AddChildrenForTools(Actor* actor);

	protected:
		bool mIsEnable = true;

		std::string mObjectName;

		Actor* mParent = nullptr;

		std::list<Actor*> mChildrens;

		//std::vector<Component*> mComponents;

		std::unordered_map<std::string, Component*> mComponentsMap;

		std::vector<Component*> mComponents;

		friend Component;
	};

	class PrefabMeta : public MetaFrame
	{
	public:
		PrefabMeta() = default;
		virtual ~PrefabMeta() override = default;

	public:
		void Load(const std::filesystem::path& metaPath);

		virtual void Save(const std::filesystem::path& metaPath) override;
	};

	template<typename T>
		requires std::is_class<Component>::value
	inline T* Actor::AddComponent()
	{
		const std::string& componentName = T::mType.mName;

		auto it = mComponentsMap.find(componentName);

		if (it != mComponentsMap.end())
		{
			LOG_ERROR("{0} 组件已经添加到 {1} 游戏对象上了", componentName.c_str(), mObjectName.c_str());
			
			return DynamicCast<T>(it->second);
		}

		T* component = new T();

		mComponentsMap.emplace(componentName, component);
		mComponents.push_back(component);

		component->SetParent(this);

		std::sort(mComponents.begin(), mComponents.end(), [](Component* left, Component* right)
			{
				return left->mOrder < right->mOrder;
			});

		return component;
	}

	template<typename T>
		requires std::is_class<Component>::value
	inline T* Actor::GetComponent()
	{
		const std::string& componentName = T::mType.mName;

		Component* component = GetComponent(componentName);
		if (component == nullptr)
		{
			return nullptr;
		}
		
		return DynamicCast<T>(component);
	}

}

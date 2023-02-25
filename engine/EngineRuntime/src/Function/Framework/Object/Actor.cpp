#include <fstream>
#include <yaml-cpp/yaml.h>
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Function/Framework/World/WorldManager.h"
#include "EngineRuntime/include/Function/Framework/Level/Level.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Engine
{
	IMPLEMENT_RTTI(Actor, GameObject);
	REGISTER_CLASS(Actor);

	Actor::Actor(Actor* parent)
		:mParent(parent)
	{ }

	Actor::~Actor()
	{
		for (Actor* actor : mChildrens)
		{
			delete actor;
		}
		for (auto it: mComponents)
		{
			if (it != nullptr)
			{
				delete it;
			}
		}
	}

	void Actor::Tick(float deltaTime)
	{
		if (!mIsEnable)
		{
			return;
		}

		for (auto child : mChildrens)
		{
			child->Tick(deltaTime);
		}

		for (auto it : mComponents)
		{
			ASSERT(it != nullptr);
			it->Tick(deltaTime);
		}
	}

	std::string Actor::GetActorName()
	{
		return mObjectName;
	}

	void Actor::SetActorName(const std::string& name)
	{
		mObjectName = name;
	}

	void Actor::SetParent(Actor* newParent)
	{
		if (mParent == newParent)
		{
			return;
		}

		if (mParent == nullptr)
		{
			// 该对象是根节点
			Level* currentLevel = WorldManager::GetInstance()->GetCurrentActiveLevel();

			auto it = currentLevel->mRootGameObject.begin();
			for (; it != currentLevel->mRootGameObject.end(); ++it)
			{
				if (*it == this)
				{
					break;
				}
			}
			if (it != currentLevel->mRootGameObject.end())
			{
				currentLevel->mRootGameObject.erase(it);
			}

			mParent = newParent;
			newParent->mChildrens.push_back(this);
		}
		else
		{
			// 该对象不是根节点
			auto it = mParent->mChildrens.begin();
			for (; it != mParent->mChildrens.end(); ++it)
			{
				if (*it == this)
				{
					break;
				}
			}
			ASSERT(*it == this);
			mParent->mChildrens.erase(it);

			if (newParent == nullptr)
			{
				Level* currentLevel = WorldManager::GetInstance()->GetCurrentActiveLevel();

				mParent = newParent;
				currentLevel->mRootGameObject.push_back(this);
			}
			else
			{
				mParent = newParent;
				newParent->mChildrens.push_back(this);
			}
		}
	}

	Actor* Actor::GetParent()
	{
		return mParent;
	}

	bool Actor::IsRootNode()
	{
		return mParent == nullptr;
	}

	uint64_t Actor::GetChildrenCount()
	{
		return mChildrens.size();
	}

	Actor* Actor::GetChildren(uint64_t index)
	{
		ASSERT(index < mChildrens.size());
		
		for (auto it = mChildrens.begin(); it != mChildrens.end(); ++it, --index)
		{
			if (index == 0)
			{
				return *it;
			}
		}

		return nullptr;
	}

	std::list<Actor*>& Actor::GetChildrens()
	{
		return mChildrens;
	}

	void Actor::SetEnable(bool enable)
	{
		mIsEnable = enable;
	}

	bool Actor::GetEnable()
	{
		return mIsEnable;
	}

	std::vector<Component*>& Actor::GetAllComponent()
	{
		return mComponents;
	}

	Component* Actor::AddComponent(const std::string& componentName)
	{
		auto it = mComponentsMap.find(componentName);

		if (it != mComponentsMap.end())
		{
			LOG_ERROR("{0} 组件已经添加到 {1} 游戏对象上了", componentName.c_str(), mObjectName.c_str());
			return it->second;
		}

		Class* for_name = Class::ForName(componentName);
		Component* component = (Component*)for_name->SpanClass();
		mComponentsMap.emplace(componentName, component);
		mComponents.push_back(component);

		component->SetParent(this);

		std::sort(mComponents.begin(), mComponents.end(), [](Component* left, Component* right)
			{
				return left->mOrder < right->mOrder;
			});

		return component;
	}

	Component* Actor::GetComponent(const std::string& componentName)
	{
		auto it = mComponentsMap.find(componentName);

		if (it == mComponentsMap.end())
		{
			//LOG_ERROR("{0} 游戏对象没有 {1} 组件", mObjectName.c_str(), componentName.c_str());
			return nullptr;
		}

		return it->second;
	}

	Actor* Actor::Instantiate(Actor* prefab)
	{
		Actor* result = BuildActor(prefab);

		result->CloneData(prefab);

		return result;
	}

	void Actor::Destory(Actor* actor)
	{
		if (actor == nullptr)
		{
			return;
		}

		if (actor->mParent)
		{
			std::erase(actor->mParent->mChildrens, actor);
		}
		else
		{
			WorldManager::GetInstance()->GetCurrentActiveLevel()->RemoveActor(actor);
		}
		delete actor;
	}

	Actor* Actor::BuildActor(Actor* node)
	{
		Actor* cloneActor = (Actor*)Class::ForName(node->GetType().GetName())->SpanClass();

		for (Actor* child : node->mChildrens)
		{
			Actor* cloneChild = BuildActor(child);
			cloneChild->SetParent(cloneActor);
		}

		for (auto it : node->mComponents)
		{
			const auto& className = it->GetType().mName;
			Component* cloneComponent = (Component*)Class::ForName(className)->SpanClass();
			cloneActor->mComponents.push_back(cloneComponent);
			cloneActor->mComponentsMap[className] = cloneComponent;
		}

		return cloneActor;
	}

	void Actor::CloneData(GameObject* node)
	{
		Actor* bluePrint = DynamicCast<Actor>(node);
		mObjectName = bluePrint->mObjectName;

		for (auto itF = bluePrint->mChildrens.begin(), itT = mChildrens.begin(); itT != mChildrens.end(); ++itF, ++itT)
		{
			(*itT)->CloneData(*itF);
		}

		for (auto it : mComponents)
		{
			const auto& className = it->GetType().mName;
			ASSERT(bluePrint->mComponentsMap.count(className) != 0);

			it->CloneData(bluePrint->mComponentsMap[className]);

			it->SetParent(this);
		}
	}

	void Actor::Serialize(SerializerDataFrame& stream) const
	{
		stream << mObjectName;

		stream << (uint32_t)mComponents.size();
		for (auto it : mComponents)
		{
			stream << it->GetType().mName;
			stream << *it;
		}

		stream << (uint32_t)mChildrens.size();
		for (auto it = mChildrens.begin(); it != mChildrens.end(); ++it)
		{
			Actor* child = *it;
			stream << child->GetType().mName;
			stream << *child;
		}
	}

	bool Actor::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mObjectName;

		uint32_t componentSize;
		stream >> componentSize;
		mComponents.reserve(componentSize);
		for (uint32_t i = 0; i < componentSize; ++i)
		{
			std::string componentClassName;
			stream >> componentClassName;
			Class* for_name = Class::ForName(componentClassName);
			Component* component = (Component*)for_name->SpanClass();
			mComponents.push_back(component);
			mComponentsMap[componentClassName] = component;
			component->SetParent(this);
			stream >> *component;
		}

		uint32_t childrenSize;
		stream >> childrenSize;
		for (uint32_t i = 0; i < childrenSize; ++i)
		{
			std::string childrenClassName;
			stream >> childrenClassName;
			Class* for_name = Class::ForName(childrenClassName);
			Actor* actor = (Actor*)for_name->SpanClass();
			mChildrens.push_back(actor);
			actor->mParent = this;
			stream >> *actor;
		}

		return true;
	}

	void Actor::AddChildrenForTools(Actor* actor)
	{
		mChildrens.push_back(actor);

		actor->mParent = this;
	}

	void PrefabMeta::Load(const std::filesystem::path& metaPath)
	{
		YAML::Node node = YAML::LoadFile(ProjectFileSystem::GetInstance()->GetActualPath(metaPath).generic_string());

		ASSERT(node["guid"].IsDefined());

		if (mGuid != nullptr)
		{
			delete mGuid;
		}

		mGuid = new GUID(node["guid"].as<std::string>());
	}

	void PrefabMeta::Save(const std::filesystem::path& metaPath)
	{
		ASSERT(mGuid != nullptr);

		YAML::Node node;
		node["guid"] = mGuid->Data();

		std::ofstream fout(ProjectFileSystem::GetInstance()->GetActualPath(metaPath));
		fout << node;
	}
}

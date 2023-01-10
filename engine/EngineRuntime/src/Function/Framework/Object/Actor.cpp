#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Function/Framework/World/WorldManager.h"
#include "EngineRuntime/include/Function/Framework/Level/Level.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Engine
{
	IMPLEMENT_RTTI(Actor, Object);
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
		for (Component* componet : mComponents)
		{
			delete componet;
		}
	}

	void Actor::Tick(float deltaTime)
	{
		for (int i = 0; i < mComponents.size(); ++i)
		{
			mComponents[i]->Tick(deltaTime);
		}

		for (auto child : mChildrens)
		{
			child->Tick(deltaTime);
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

	TransformComponent* Actor::GetTransform()
	{
		TransformComponent* transform = nullptr;

		ASSERT(mComponents.size() > 0 && mComponents.front()->GetType().mName == "TransformComponent");

		transform = dynamic_cast<TransformComponent*>(mComponents.front());

		ASSERT(transform != nullptr);

		return transform;
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

	void Actor::Serialize(SerializerDataFrame& stream) const
	{
		stream << mObjectName;

		stream << (uint32_t)mComponents.size();
		for (uint32_t i = 0; i < mComponents.size(); ++i)
		{
			stream << mComponents[i]->GetType().mName;
			stream << *mComponents[i];
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
			stream >> *component;
			//component->Deserialize(stream);
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
			//actor->Deserialize(stream);
			stream >> *actor;
		}

		return true;
	}

	void Actor::AddChildrenForTools(Actor* actor)
	{
		mChildrens.push_back(actor);

		actor->mParent = this;
	}

}

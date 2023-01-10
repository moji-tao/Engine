#include "EngineRuntime/include/Function/Framework/Level/Level.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Engine
{
	IMPLEMENT_RTTI(Level, Object);
	REGISTER_CLASS(Level);

	Level::Level()
	{ }
	Level::~Level()
	{ }

	bool Level::Initialize()
	{
		return false;
	}

	void Level::Tick(float deltaTime)
	{
		for (Actor* actor : mRootGameObject)
		{
			actor->Tick(deltaTime);
		}
	}

	void Level::Finalize()
	{
		for (Actor* actor : mRootGameObject)
		{
			delete actor;
		}
	}

	bool Level::Load(const std::string& levelResUrl)
	{
		return false;
	}
	void Level::Unload()
	{
	}
	bool Level::Save()
	{
		return false;
	}

	std::string Level::GetSceneName()
	{
		return mSceneName;
	}

	void Level::AddActor(Actor* actor)
	{
		mRootGameObject.push_back(actor);
		actor->SetParent(nullptr);
	}

	std::list<Actor*>& Level::GetSceneActors()
	{
		return mRootGameObject;
	}

	void Level::Serialize(SerializerDataFrame& stream) const
	{
		stream << mSceneName;

		stream << (uint32_t)mRootGameObject.size();
		for (auto actorPtr : mRootGameObject)
		{
			stream << actorPtr->GetType().mName;

			//actorPtr->Serialize(stream);

			stream << *actorPtr;
		}
	}

	bool Level::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mSceneName;
		uint32_t size;
		stream >> size;

		for (unsigned i = 0; i < size; ++i)
		{
			std::string actorClassType;
			stream >> actorClassType;
			Class* for_name = Class::ForName(actorClassType);
			Actor* actor = (Actor*)for_name->SpanClass();

			//actor->Deserialize(stream);

			stream >> *actor;

			mRootGameObject.push_back(actor);
		}
		return true;
	}
}

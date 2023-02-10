#include <fstream>
#include <yaml-cpp/yaml.h>
#include "EngineRuntime/include/Function/Framework/Level/Level.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

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
		auto pathses = AssetManager::GetInstance()->GetAssetPathFormAssetGuid(mGuid);
		LOG_INFO("保存关卡 guid:{0} 路径:{1}", mGuid.Data(), pathses.generic_string().c_str());

		SerializerDataFrame frame;

		frame << *this;

		frame.Save(pathses);

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

	void Level::RemoveActor(Actor* actor)
	{
		std::erase(mRootGameObject, actor);
	}

	Actor* Level::CreateEmptyActor(const std::string& actorName)
	{
		Actor* actor = new Actor();
		actor->SetActorName(actorName);

		mRootGameObject.emplace_back(actor);

		actor->AddComponent<TransformComponent>();

		return actor;
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

	void SceneMeta::Load(const std::filesystem::path& metaPath)
	{
		YAML::Node node = YAML::LoadFile(ProjectFileSystem::GetInstance()->GetActualPath(metaPath).generic_string());

		ASSERT(node["guid"].IsDefined());

		if (mGuid != nullptr)
		{
			delete mGuid;
		}

		mGuid = new GUID(node["guid"].as<std::string>());
	}

	void SceneMeta::Save(const std::filesystem::path& metaPath)
	{
		ASSERT(mGuid != nullptr);

		YAML::Node node;
		node["guid"] = mGuid->Data();

		std::ofstream fout(ProjectFileSystem::GetInstance()->GetActualPath(metaPath));
		fout << node;
	}
}

#pragma once
#include <memory>
#include <list>
#include <string>
#include "EngineRuntime/include/Function/Framework/World/WorldManager.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Core/Meta/Object.h"

namespace Engine
{
	class Level : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		Level();

		virtual ~Level() override;

	public:
		bool Initialize();

		void Tick(float deltaTime);

		void Finalize();

	public:
		bool Load(const std::string& levelResUrl);

		void Unload();

		bool Save();

		std::string GetSceneName();

		void AddActor(Actor* actor);

		void RemoveActor(Actor* actor);

		Actor* CreateEmptyActor(const std::string& actorName);

		std::list<Actor*>& GetSceneActors();

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

	private:
		std::list<Actor*> mRootGameObject;

		std::string mSceneName;

		friend Actor;

		friend WorldManager;
	};

	class SceneMeta : public MetaFrame
	{
	public:
		SceneMeta() = default;
		virtual ~SceneMeta() override = default;

	public:
		void Load(const std::filesystem::path& metaPath);

		virtual void Save(const std::filesystem::path& metaPath) override;
	};
}

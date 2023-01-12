#include "EngineRuntime/include/Function/Framework/World/WorldManager.h"
#include "EngineRuntime/include/Function/Framework/Level/Level.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"
#include "EngineRuntime/include/Resource/ConfigManager/ConfigManager.h"

namespace Engine
{
	WorldManager::WorldManager()
	{

	}

	WorldManager::~WorldManager()
	{

	}

	bool WorldManager::Initialize()
	{
		std::filesystem::path defaultScenePath = ConfigManager::GetInstance()->GetDefaultScenePath();
		mCurrentActiveLevel = ResourceSerializer::LoadResourceFromFile<Level>(defaultScenePath);
		
		return true;
	}

	void WorldManager::Tick(float deltaTime)
	{
		if(mCurrentActiveLevel != nullptr)
		{
			mCurrentActiveLevel->Tick(deltaTime);
		}
	}

	void WorldManager::Finalize()
	{
		//ConfigManager::GetInstance()->SetDefaultScenePath();
		if (mCurrentActiveLevel != nullptr)
		{
			delete mCurrentActiveLevel;
		}
	}

	void WorldManager::ReloadCurrentLevel()
	{

	}

	void WorldManager::SaveCurrentLevel()
	{

	}

	Level* WorldManager::GetCurrentActiveLevel() const
	{
		ASSERT(mCurrentActiveLevel != nullptr);

		return mCurrentActiveLevel;
	}

	Level* WorldManager::SpanEmptyScene(const std::string& sceneName)
	{
		Level* level = new Level();
		level->mSceneName = sceneName;

		return level;
	}

	bool WorldManager::LoadWorld(const std::string& worldUrl)
	{
		return false;
	}

	bool WorldManager::LoadLevel(const std::string& levelUrl)
	{
		return false;
	}
}

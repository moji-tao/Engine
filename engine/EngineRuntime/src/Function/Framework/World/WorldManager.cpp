#include "EngineRuntime/include/Function/Framework/World/WorldManager.h"
#include "EngineRuntime/include/Function/Framework/Level/Level.h"

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
		return false;
	}

	void WorldManager::Tick(float deltaTime)
	{
		if(mIsWorldLoaded)
		{
			LoadWorld(mCurrentWorldUrl);
		}

		std::shared_ptr<Level> activeLevel = mCurrentActiveLevel.lock();
		if(activeLevel)
		{
			activeLevel->Tick(deltaTime);
		}
	}

	void WorldManager::Finalize()
	{

	}

	void WorldManager::ReloadCurrentLevel()
	{

	}

	void WorldManager::SaveCurrentLevel()
	{

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

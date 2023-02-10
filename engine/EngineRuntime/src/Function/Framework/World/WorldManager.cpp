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
		mCurrentActiveLevel = AssetManager::GetInstance()->LoadResource<Level>(defaultScenePath);
		
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
			AssetManager::GetInstance()->UnLoadResource(mCurrentActiveLevel);
		}
	}

	void WorldManager::ReloadCurrentLevel()
	{

	}

	void WorldManager::SaveCurrentLevel()
	{
		LOG_INFO("正在保存场景...");
		if (mCurrentActiveLevel == nullptr)
		{
			LOG_ERROR("当前没有打开任何场景");
			return;
		}

		mCurrentActiveLevel->Save();
	}

	Level* WorldManager::GetCurrentActiveLevel() const
	{
		ASSERT(mCurrentActiveLevel != nullptr);

		return mCurrentActiveLevel;
	}

	SerializerDataFrame WorldManager::SpanEmptyScene(const std::string& sceneName)
	{
		Level level;
		level.mSceneName = sceneName;

		SerializerDataFrame frame;
		frame << level;

		return frame;
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

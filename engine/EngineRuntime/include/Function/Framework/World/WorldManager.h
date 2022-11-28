#pragma once
#include <memory>
#include <string>
#include <unordered_map>

namespace Engine
{
	class Level;

	class WorldManager
	{
	public:
		WorldManager();

		~WorldManager();

	public:
		bool Initialize();

		void Tick(float deltaTime);

		void Finalize();

	public:
		void ReloadCurrentLevel();

		void SaveCurrentLevel();

	private:
		bool LoadWorld(const std::string& worldUrl);

		bool LoadLevel(const std::string& levelUrl);

	private:
		bool mIsWorldLoaded = false;

		std::string mCurrentWorldUrl;

		std::unordered_map<std::string, std::shared_ptr<Level>> mLoadedLevels;

		std::weak_ptr<Level> mCurrentActiveLevel;
	};
}

#pragma once
#include <string>

namespace Engine
{
	class Level
	{
	public:
		Level();

		~Level();

	public:
		bool Initialize();

		void Tick(float deltaTime);

		void Finalize();

	public:
		bool Load(const std::string& levelResUrl);

		void Unload();

		bool Save();

	private:
	};
}

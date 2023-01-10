#pragma once
#include <list>
#include <string>
#include <memory>
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Editor
{
	class AssetFile
	{
	public:
		bool mIsFolder = false;
		std::string mName;
		AssetFile* mParent = nullptr;
		std::list<std::shared_ptr<AssetFile>> next;
		
		std::shared_ptr<Engine::Blob> GetBinary();
	};
}

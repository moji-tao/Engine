#include "EngineEditor/include/Application/AssetFIle.h"

namespace Editor
{
	std::shared_ptr<Engine::Blob> AssetFile::GetBinary()
	{
		std::filesystem::path result;
		AssetFile* node = this;
		while (node)
		{
			result = node->mName / result;
			node = node->mParent;
		}
		result = result.parent_path();

		return Engine::ProjectFileSystem::GetInstance()->ReadFile(result);
	}
}

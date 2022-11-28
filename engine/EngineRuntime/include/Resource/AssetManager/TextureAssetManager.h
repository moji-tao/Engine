#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <filesystem>
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"

namespace Engine
{
	class TextureAssetManager
	{
	public:
		std::shared_ptr<TextureData> GetHDRTexture(const std::filesystem::path& assetUrl);

		std::shared_ptr<TextureData> GetTexture(const std::filesystem::path& assetUrl, bool isSRGB = false);

	private:
		std::unordered_map<std::string, std::unique_ptr<TextureData>> mTextureSet;
	};
}

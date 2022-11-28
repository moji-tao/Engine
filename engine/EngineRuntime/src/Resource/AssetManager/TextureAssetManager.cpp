#include <stb/stb_image.h>
#include "EngineRuntime/include/Resource/AssetManager/TextureAssetManager.h"

namespace Engine
{
	std::shared_ptr<TextureData> TextureAssetManager::GetHDRTexture(const std::filesystem::path& assetUrl)
	{
		std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

		int iw, ih, n;
		texture->mPixels = stbi_loadf(assetUrl.generic_string().c_str(), &iw, &ih, &n, 0);

		if (texture->mPixels == nullptr)
		{
			return nullptr;
		}
		
		texture->Info.mWidth = iw;
		texture->Info.mHeight = ih;
		switch (n)
		{
		case 2:
			texture->Info.mFormat = PIXEL_FORMAT::PIXEL_FORMAT_R32G32_FLOAT;
			break;
		case 3:
			texture->Info.mFormat = PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32_FLOAT;
			break;
		case 4:
			texture->Info.mFormat = PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32A32_FLOAT;
			break;
		default:
			throw std::runtime_error("unsupported channels number");
			break;
		}
		texture->Info.mRowPitch = iw * n * sizeof(float);
		texture->Info.mSlicePitch = iw * ih * n * sizeof(float);
		texture->Info.mDepth = 1;
		texture->Info.mArrayLayers = 1;
		texture->Info.mMipLevels = 1;
		texture->Info.mType = IMAGE_TYPE::IMAGE_TYPE_2D;

		return texture;
	}

	std::shared_ptr<TextureData> TextureAssetManager::GetTexture(const std::filesystem::path& assetUrl, bool isSRGB)
	{
		std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

		int iw, ih, n;
		texture->mPixels = stbi_load(assetUrl.generic_string().c_str(), &iw, &ih, &n, 4);

		if (!texture->mPixels)
			return nullptr;

		texture->Info.mRowPitch = iw * 4 * sizeof(float);
		texture->Info.mSlicePitch = iw * ih * 4 * sizeof(float);
		texture->Info.mWidth = iw;
		texture->Info.mHeight = ih;
		texture->Info.mFormat = (isSRGB) ? PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_SRGB : PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM;
		texture->Info.mDepth = 1;
		texture->Info.mArrayLayers = 1;
		texture->Info.mMipLevels = 1;
		texture->Info.mType = IMAGE_TYPE::IMAGE_TYPE_2D;

		return texture;
	}
}

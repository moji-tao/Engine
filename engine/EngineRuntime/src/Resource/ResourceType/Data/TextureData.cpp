#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <fstream>
#include <cstdlib>
#include <yaml-cpp/yaml.h>
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"

namespace Engine
{
	IMPLEMENT_RTTI(TextureData, Object);

	TextureData::~TextureData()
	{
		if(mPixels != nullptr)
		{
			free(mPixels);
		}
	}

	bool TextureData::IsValid()
	{
		return mPixels != nullptr;
	}

	void TextureData::Serialize(SerializerDataFrame& stream) const
	{
		stream.Write((const char*)&Info, sizeof(Info));
		stream.Write((const char*)mPixels, Info.mSlicePitch);
	}

	bool TextureData::Deserialize(SerializerDataFrame& stream)
	{
		stream.Read((char*)&Info, sizeof(Info));
		mPixels = malloc(Info.mSlicePitch);
		stream.Read((char*)mPixels, Info.mSlicePitch);

		return true;
	}

	std::shared_ptr<TextureData> LoadTextureForFile(const std::filesystem::path& assetUrl, bool isSRGB)
	{
		std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

		int iw, ih, n;
		void* oldImage = stbi_load(assetUrl.generic_string().c_str(), &iw, &ih, &n, 4);
		if (iw % 256 != 0)
		{
			int new_width = (int)((float)iw / 256 + 0.5) * 256;
			int new_height = (int)(ih * ((float)new_width / iw) + 0.5);
			texture->mPixels = malloc(new_width * 4 * new_height);
			stbir_resize_uint8((const unsigned char*)oldImage, iw, ih, 0, (unsigned char*)texture->mPixels, new_width, new_height, 0, 4);
			stbi_image_free(oldImage);
			iw = new_width;
			ih = new_height;
		}
		else
		{
			texture->mPixels = oldImage;
		}

		if (!texture->mPixels)
			return nullptr;

		texture->Info.mRowPitch = iw * 4 * sizeof(uint8_t);
		texture->Info.mSlicePitch = iw * ih * 4 * sizeof(uint8_t);
		texture->Info.mWidth = iw;
		texture->Info.mHeight = ih;
		texture->Info.mFormat = (isSRGB) ? PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_SRGB : PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM;
		texture->Info.mDepth = 1;
		texture->Info.mArrayLayers = 1;
		texture->Info.mMipLevels = 1;
		texture->Info.mType = IMAGE_TYPE::IMAGE_TYPE_2D;

		return texture;
	}

	std::shared_ptr<TextureData> LoadHDRTextureForFile(const std::filesystem::path& assetUrl)
	{
		std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

		int iw, ih, n;
		void* oldImage = stbi_loadf(assetUrl.generic_string().c_str(), &iw, &ih, &n, 4);
		if (iw % 256 != 0)
		{
			int new_width = (int)((float)iw / 256 + 0.5) * 256;
			int new_height = (int)(ih * ((float)new_width / iw) + 0.5);
			texture->mPixels = malloc(new_width * 4 * new_height * sizeof(float));
			stbir_resize_float((const float*)oldImage, iw, ih, 0, (float*)texture->mPixels, new_width, new_height, 0, 4);
			stbi_image_free(oldImage);
			iw = new_width;
			ih = new_height;
		}
		else
		{
			texture->mPixels = oldImage;
		}

		if (!texture->mPixels)
			return nullptr;
		
		texture->Info.mRowPitch = iw * 4 * sizeof(float);
		texture->Info.mSlicePitch = iw * ih * 4 * sizeof(float);
		texture->Info.mWidth = iw;
		texture->Info.mHeight = ih;
		texture->Info.mFormat = PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32A32_FLOAT;
		texture->Info.mDepth = 1;
		texture->Info.mArrayLayers = 1;
		texture->Info.mMipLevels = 1;
		texture->Info.mType = IMAGE_TYPE::IMAGE_TYPE_2D;

		return texture;
	}

	std::shared_ptr<TextureData> LoadTextureForMemory(const void* data, size_t length, bool isSRGB)
	{
		std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

		int iw, ih, n;
		void* oldImage = stbi_load_from_memory((const stbi_uc*)data, length, &iw, &ih, &n, 4);
		if (iw % 256 != 0)
		{
			int new_width = (int)((float)iw / 256 + 0.5) * 256;
			int new_height = (int)(ih * ((float)new_width / iw) + 0.5);
			texture->mPixels = malloc(new_width * 4 * new_height);
			stbir_resize_uint8((const unsigned char*)oldImage, iw, ih, 0, (unsigned char*)texture->mPixels, new_width, new_height, 0, 4);
			stbi_image_free(oldImage);
			iw = new_width;
			ih = new_height;
		}
		else
		{
			texture->mPixels = oldImage;
		}

		if (!texture->mPixels)
			return nullptr;

		texture->Info.mRowPitch = iw * 4 * sizeof(uint8_t);
		texture->Info.mSlicePitch = iw * ih * 4 * sizeof(uint8_t);
		texture->Info.mWidth = iw;
		texture->Info.mHeight = ih;
		texture->Info.mFormat = (isSRGB) ? PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_SRGB : PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM;
		texture->Info.mDepth = 1;
		texture->Info.mArrayLayers = 1;
		texture->Info.mMipLevels = 1;
		texture->Info.mType = IMAGE_TYPE::IMAGE_TYPE_2D;

		return texture;
	}

	void TextureMeta::Load(const std::filesystem::path& metaPath)
	{
		YAML::Node node = YAML::LoadFile(ProjectFileSystem::GetInstance()->GetActualPath(metaPath).generic_string());

		ASSERT(node["guid"].IsDefined());

		if (mGuid != nullptr)
		{
			delete mGuid;
		}

		mGuid = new GUID(node["guid"].as<std::string>());
	}

	void TextureMeta::Save(const std::filesystem::path& metaPath)
	{
		ASSERT(mGuid != nullptr);

		YAML::Node node;
		node["guid"] = mGuid->Data();

		std::ofstream fout(ProjectFileSystem::GetInstance()->GetActualPath(metaPath));
		fout << node;
	}
}

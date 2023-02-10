#pragma once
#include <cstdint>
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	enum class PIXEL_FORMAT : uint8_t
	{
		PIXEL_FORMAT_UNKNOWN = 0,
		PIXEL_FORMAT_R8G8B8A8_UNORM,
		PIXEL_FORMAT_R8G8B8A8_SRGB,
		PIXEL_FORMAT_R32G32_FLOAT,
		PIXEL_FORMAT_R32G32B32_FLOAT,
		PIXEL_FORMAT_R32G32B32A32_FLOAT
	};

	enum class IMAGE_TYPE : uint8_t
	{
		IMAGE_TYPE_UNKNOWM = 0,
		IMAGE_TYPE_2D,
		IMAGE_TYPE_CUBE,
		IMAGE_TYPE_3D
	};

	struct TextureInfo
	{
		uint32_t mWidth = 0;

		uint32_t mHeight = 0;

		uint32_t mDepth = 0;

		uint32_t mMipLevels = 0;

		uint32_t mArrayLayers = 0;

		uint32_t mRowPitch = 0;

		uint32_t mSlicePitch = 0;

		PIXEL_FORMAT mFormat = PIXEL_FORMAT::PIXEL_FORMAT_UNKNOWN;

		IMAGE_TYPE mType = IMAGE_TYPE::IMAGE_TYPE_UNKNOWM;
	};

	class TextureData : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		TextureInfo Info;

		void* mPixels = nullptr;

		TextureData() = default;

		~TextureData();

		bool IsValid();

		void Serialize(SerializerDataFrame& stream) const override;

		bool Deserialize(SerializerDataFrame& stream) override;
	};

	class TextureMeta : public MetaFrame
	{
	public:
		TextureMeta() = default;
		virtual ~TextureMeta() override = default;

	public:
		void Load(const std::filesystem::path& metaPath);

		virtual void Save(const std::filesystem::path& metaPath) override;
	};


	std::shared_ptr<TextureData> LoadTextureForFile(const std::filesystem::path& assetUrl, bool isSRGB);

	std::shared_ptr<TextureData> LoadHDRTextureForFile(const std::filesystem::path& assetUrl);

	std::shared_ptr<TextureData> LoadTextureForMemory(const void* data, size_t length, bool isSRGB);

}

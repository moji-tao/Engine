#pragma once
#include <cstdint>

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

	class TextureData
	{
	public:
		void* mPixels = nullptr;

		TextureInfo Info;

		TextureData() = default;

		~TextureData();

		bool IsValid();
	};
}

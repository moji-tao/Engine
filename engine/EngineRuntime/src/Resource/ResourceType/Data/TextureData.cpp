#include <cstdlib>
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"

namespace Engine
{
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


}

#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"

namespace Engine
{
	void MaterialData::Serialize(SerializerDataFrame& stream) const
	{
		stream << BaseColorTextureFile;
	}

	bool MaterialData::Deserialize(SerializerDataFrame& stream)
	{
		stream >> BaseColorTextureFile;

		return true;
	}
}


#pragma once
#include "EngineRuntime/include/Core/Random/Random.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	class MaterialData : public ISerializable
	{
	public:
		void Serialize(SerializerDataFrame& stream) const override;

		bool Deserialize(SerializerDataFrame& stream) override;

	public:
		GUID BaseColorTextureFile;
	};
}

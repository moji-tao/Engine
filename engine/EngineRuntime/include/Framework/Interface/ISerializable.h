#pragma once
#include "EngineRuntime/include/Core/Meta/Serializer.h"

namespace Engine
{
	class ISerializable
	{
	public:
		// 将Object序列化到Frame中
		virtual void Serialize(SerializerDataFrame& stream) const = 0;

		// 将Frame中的数据反序列化到Object中
		virtual bool Deserialize(SerializerDataFrame& stream) = 0;
	};
}

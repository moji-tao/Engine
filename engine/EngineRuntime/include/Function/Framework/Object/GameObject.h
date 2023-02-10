#pragma once
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	class GameObject : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		GameObject() = default;

		virtual ~GameObject() = default;

	public:
		virtual void CloneData(GameObject* node) = 0;

	};
}

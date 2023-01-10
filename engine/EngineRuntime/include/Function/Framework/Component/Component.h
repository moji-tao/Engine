#pragma once
#include <memory>
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	class Actor;
	class TransformComponent;

	class Component : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		Component();

		virtual ~Component();

	public:
		void SetParent(Actor* parentObject);

		virtual void Tick(float deltaTime);

	protected:
		Actor* mParentObject;

		bool mIsEnable = true;
	};
}

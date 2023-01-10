#include "EngineRuntime/include/Function/Framework/Component/Component.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"

namespace Engine
{
	IMPLEMENT_RTTI(Component, Object);

	Component::Component()
		:mIsEnable(true)
	{ }

	Component::~Component()
	{ }

	void Component::SetParent(Actor* parentObject)
	{
		mParentObject = parentObject;
	}

	void Component::Tick(float deltaTime)
	{
		
	}
}

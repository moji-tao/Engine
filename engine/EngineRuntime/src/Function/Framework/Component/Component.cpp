#include "EngineRuntime/include/Function/Framework/Component/Component.h"

namespace Engine
{
	Component::Component()
	{

	}

	Component::~Component()
	{

	}

	bool Component::Initialize(std::weak_ptr<GameObject> parentObject)
	{
		mParentObject = parentObject;

		mIsEnable = true;

		return true;
	}

	void Component::Tick(float deltaTime)
	{
		
	}
}

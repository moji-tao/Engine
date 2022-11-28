#include "EngineRuntime/include/Function/Framework/Object/Object.h"
#include "EngineRuntime/include/Function/Framework/Component/Component.h"

namespace Engine
{
	GameObject::GameObject()
	{

	}

	GameObject::~GameObject()
	{

	}

	void GameObject::Tick(float deltaTime)
	{
		for (int i = 0; i < mComponents.size(); ++i)
		{
			mComponents[i]->Tick(deltaTime);
		}

		for (int i = 0; i < mChildren.size(); ++i)
		{
			mChildren[i].Tick(deltaTime);
		}
	}
}

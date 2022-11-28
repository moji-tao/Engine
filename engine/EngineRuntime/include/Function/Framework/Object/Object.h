#pragma once
#include <vector>
#include <memory>
#include "EngineRuntime/include/Function/Framework/Component/Component.h"

namespace Engine
{
	class Object
	{
		
	};

	class GameObject : public Object
	{
	public:
		GameObject();

		virtual ~GameObject();

	public:
		virtual void Tick(float deltaTime);

	protected:
		std::weak_ptr<Object> mParent;

		std::vector<GameObject> mChildren;

		std::vector<std::shared_ptr<Component>> mComponents;
	};
}

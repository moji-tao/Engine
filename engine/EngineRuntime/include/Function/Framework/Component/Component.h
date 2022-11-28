#pragma once
#include <memory>

namespace Engine
{
	class GameObject;

	class Component
	{
	public:
		Component();

		virtual ~Component();

	public:
		bool Initialize(std::weak_ptr<GameObject> parentObject);

		virtual void Tick(float deltaTime);

	protected:
		std::weak_ptr<GameObject> mParentObject;

		bool mIsEnable = true;
	};
}

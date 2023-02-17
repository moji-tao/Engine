#include "EngineRuntime/include/Function/Framework/Component/Component.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/MeshRendererComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/DirectionalLightComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/PointLightComponent.h"

namespace Engine
{
	IMPLEMENT_RTTI(Component, GameObject);

	std::vector<std::string> Component::ALLInstanceComponentName;

	Component::Component()
		:mIsEnable(true)
	{ }

	Component::~Component()
	{ }

	void Component::SetParent(Actor* parentObject)
	{
		mParentObject = parentObject;
	}

	ComponentClassRegister::ComponentClassRegister(const std::string& className)
	{
		Component::ALLInstanceComponentName.emplace_back(className);
	}

	IMPLEMENT_RTTI(TransformComponent, Component);
	REGISTER_COMPONENT(TransformComponent);

	IMPLEMENT_RTTI(MeshRendererComponent, Component);
	REGISTER_COMPONENT(MeshRendererComponent);

	IMPLEMENT_RTTI(DirectionalLightComponent, LightComponent);
	REGISTER_COMPONENT(DirectionalLightComponent);

	IMPLEMENT_RTTI(PointLightComponent, LightComponent);
	REGISTER_COMPONENT(PointLightComponent);
}

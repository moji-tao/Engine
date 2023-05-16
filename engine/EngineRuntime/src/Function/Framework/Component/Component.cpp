#include "EngineRuntime/include/Function/Framework/Component/Component.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/MeshRendererComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/DirectionalLightComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/PointLightComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/SkeletonMeshRendererComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/AnimationComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/AnimatorComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/ScriptComponent.h"

namespace Engine
{
	IMPLEMENT_RTTI(Component, GameObject);

	std::vector<std::string> Component::ALLInstanceComponentName;

	Component::Component()
	{ }

	Component::~Component()
	{ }

	void Component::SetParent(Actor* parentObject)
	{
		mParentObject = parentObject;
	}

	void Component::LuaAttachParent(Actor* parentObject)
	{
		parentObject->LuaAttachParent(this);
	}

	ComponentClassRegister::ComponentClassRegister(const std::string& className)
	{
		Component::ALLInstanceComponentName.emplace_back(className);
	}

	IMPLEMENT_RTTI(TransformComponent, Component);
	REGISTER_COMPONENT(TransformComponent);

	IMPLEMENT_RTTI(MeshRendererComponent, Component);
	REGISTER_COMPONENT(MeshRendererComponent);

	IMPLEMENT_RTTI(SkeletonMeshRendererComponent, Component);
	REGISTER_COMPONENT(SkeletonMeshRendererComponent);

	IMPLEMENT_RTTI(DirectionalLightComponent, LightComponent);
	REGISTER_COMPONENT(DirectionalLightComponent);

	IMPLEMENT_RTTI(PointLightComponent, LightComponent);
	REGISTER_COMPONENT(PointLightComponent);

	IMPLEMENT_RTTI(AnimationComponent, Component);
	REGISTER_COMPONENT(AnimationComponent);

	IMPLEMENT_RTTI(AnimatorComponent, Component);
	REGISTER_COMPONENT(AnimatorComponent);

	IMPLEMENT_RTTI(ScriptComponent, Component);
	REGISTER_COMPONENT(ScriptComponent);
}

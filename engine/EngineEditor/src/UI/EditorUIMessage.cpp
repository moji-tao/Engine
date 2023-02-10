#include "EngineEditor/include/UI/EditorUIMessage.h"

namespace Editor
{
	void EditorUIMessage::SetInspectorShowActor(Engine::Actor* actor)
	{
		mInspectorShowActor = actor;
	}

	Engine::Actor* EditorUIMessage::GetInspectorShowActor()
	{
		return mInspectorShowActor;
	}
}

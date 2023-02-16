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

	void EditorUIMessage::SetProjectSelectFile(const Engine::GUID& guid)
	{
		mProjectSelectFile = guid;
	}

	Engine::GUID EditorUIMessage::GetProjectSelectFile()
	{
		return mProjectSelectFile;
	}
}

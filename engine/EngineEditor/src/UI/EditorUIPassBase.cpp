#include "EngineEditor/include/UI/EditorUIPassBase.h"

namespace Editor
{
	IMPLEMENT_RTTI(EditorUIPassBase, Engine::Object);

	EditorUIPassBase::EditorUIPassBase()
	{ }

	void EditorUIPassBase::Initialize(Engine::ImGuiDevice* device, EngineEditor* editor)
	{
		mDevice = device;
		mEditor = editor;
	}
}


#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIInspectorPass.h"

namespace Editor
{
	EditorUIInspectorPass::EditorUIInspectorPass()
		:EditorUIPassBase()
	{ }

	EditorUIInspectorPass::~EditorUIInspectorPass()
	{

	}

	void EditorUIInspectorPass::Initialize(Engine::ImGuiDevice* device, EngineEditor* editor)
	{
		EditorUIPassBase::Initialize(device, editor);
	}

	void EditorUIInspectorPass::ShowUI()
	{
		if (!mIsOpen)
			return;

		ImGui::Begin("检查器", &mIsOpen, ImGuiWindowFlags_NoCollapse);

		ImGui::End();
	}
}

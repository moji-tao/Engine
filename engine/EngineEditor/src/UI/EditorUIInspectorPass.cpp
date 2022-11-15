#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIInspectorPass.h"

namespace Editor
{
	EditorUIInspectorPass::EditorUIInspectorPass()
	{

	}

	EditorUIInspectorPass::~EditorUIInspectorPass()
	{

	}

	void EditorUIInspectorPass::ShowUI()
	{
		if (!mIsOpen)
			return;

		ImGui::Begin("检查器", &mIsOpen, ImGuiWindowFlags_NoCollapse);

		ImGui::End();
	}
}

#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIHierarchyPass.h"

namespace Editor
{
	EditorUIHierarchyPass::EditorUIHierarchyPass()
	{

	}

	EditorUIHierarchyPass::~EditorUIHierarchyPass()
	{

	}

	void EditorUIHierarchyPass::ShowUI()
	{
		if (!mIsOpen)
			return;

		ImGui::Begin("场景层级", &mIsOpen, ImGuiWindowFlags_NoCollapse);

		ImGui::End();
	}
}

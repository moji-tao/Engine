#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIScenePass.h"

namespace Editor
{
	EditorUIScenePass::EditorUIScenePass()
	{

	}

	EditorUIScenePass::~EditorUIScenePass()
	{

	}

	void EditorUIScenePass::ShowUI()
	{
		if(!mIsOpen)
			return;

		ImGui::Begin("场景", &mIsOpen, ImGuiWindowFlags_NoCollapse);
		
		ImGui::End();
	}
}

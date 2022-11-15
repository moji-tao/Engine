#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIConsolePass.h"

#include <iostream>

namespace Editor
{
	EditorUIConsolePass::EditorUIConsolePass()
	{
		mOldcout = std::cout.rdbuf(fo.rdbuf());
	}

	EditorUIConsolePass::~EditorUIConsolePass()
	{
		
	}

	void EditorUIConsolePass::ShowUI()
	{
		if (!mIsOpen)
			return;

		ImGui::Begin("控制台", &mIsOpen, ImGuiWindowFlags_NoCollapse);
		

		ImGui::End();
	}


}

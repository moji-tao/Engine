#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIConsolePass.h"

#include <iostream>

namespace Editor
{
	EditorUIConsolePass::EditorUIConsolePass()
		:EditorUIPassBase()
	{ }

	EditorUIConsolePass::~EditorUIConsolePass()
	{
		
	}

	void EditorUIConsolePass::Initialize(EditorUIMessage* messageBox, Engine::ImGuiDevice* device, EngineEditor* editor)
	{
		EditorUIPassBase::Initialize(messageBox, device, editor);
		mOldcout = std::cout.rdbuf(fo.rdbuf());
	}

	void EditorUIConsolePass::ShowUI()
	{
		if (!mIsOpen)
			return;

		ImGui::Begin("控制台", &mIsOpen, ImGuiWindowFlags_NoCollapse);
		

		ImGui::End();
	}


}

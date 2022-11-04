#include "EngineEditor/include/UI/EditorUI.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Editor
{
	EditorUI::~EditorUI()
	{ }

	void EditorUI::Initialize(const Engine::WindowUIInitInfo& info)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();


		ImGuiIO& io = ImGui::GetIO();
		//io.

		info.renderSystem->InitializeUIRenderBackend();
	}

	void EditorUI::PreRender()
	{

	}

}
#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIScenePass.h"

#include <imgui/imgui_internal.h>

#include "EngineRuntime/include/Core/Base/macro.h"
#include "Function/Render/RenderSystem.h"

namespace Editor
{
	EditorUIScenePass::EditorUIScenePass()
		:EditorUIPassBase()
	{ }

	EditorUIScenePass::~EditorUIScenePass()
	{

	}

	void EditorUIScenePass::Initialize(Engine::ImGuiDevice* device, EngineEditor* editor)
	{
		EditorUIPassBase::Initialize(device, editor);

		mSceneRender = device->GetSceneTextureInfo();

		mIconFileTexture = mDevice->LoadTexture(mEditor->Config.editorResourceConfig.editorFileIcon);
	}

	void EditorUIScenePass::ShowUI()
	{
		if(!mIsOpen)
			return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("场景", &mIsOpen, ImGuiWindowFlags_NoCollapse);

		ImVec2 render_target_window_size;
		auto menu_bar_rect = ImGui::GetCurrentWindow()->MenuBarRect();
		render_target_window_size.x = ImGui::GetWindowSize().x;
		render_target_window_size.y = (ImGui::GetWindowSize().y + ImGui::GetWindowPos().y) - menu_bar_rect.Max.y;
		if (render_target_window_size.x != currentWindowSize.x || render_target_window_size.y != currentWindowSize.y)
		{
			currentWindowSize = render_target_window_size;
			Engine::RenderSystem::GetInstance()->RenderViewResize((int)currentWindowSize.x, (int)currentWindowSize.y);
		}

		
		
		ImGui::Image(mSceneRender, currentWindowSize);

		

		ImGui::End();
		ImGui::PopStyleVar();
	}
}

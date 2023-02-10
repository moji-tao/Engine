#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "EngineEditor/include/UI/EditorUIScenePass.h"
#include "EngineEditor/include/UI/ImGuiExtensions/ImGuizmo.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"

namespace Editor
{
	EditorUIScenePass::EditorUIScenePass()
		:EditorUIPassBase(), mEditorConstants(Engine::RenderSystem::GetInstance()->GetRenderResource()->GetCameraPass())
	{ }

	EditorUIScenePass::~EditorUIScenePass()
	{

	}

	void EditorUIScenePass::Initialize(EditorUIMessage* messageBox, Engine::ImGuiDevice* device, EngineEditor* editor)
	{
		EditorUIPassBase::Initialize(messageBox, device, editor);

		inputSystem = mEditor->GetInputSystem();

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
		if ((render_target_window_size.x != currentWindowSize.x || render_target_window_size.y != currentWindowSize.y) && render_target_window_size.x >= 0 && render_target_window_size.y >= 0)
		{
			currentWindowSize = render_target_window_size;
			inputSystem->ResizeSceneWindow((int)currentWindowSize.x, (int)currentWindowSize.y);
		}

		ImGui::Image(mSceneRender, currentWindowSize);

		if (ImGui::IsItemHovered())
		{
			inputSystem->mMouseHoveredScene = true;
		}
		else
		{
			inputSystem->mMouseHoveredScene = false;
		}

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, currentWindowSize.x, currentWindowSize.y);

		Engine::Actor* ac = mMessageBox->GetInspectorShowActor();
		if (ac != mSelectActor)
		{
			if (ac != nullptr)
			{
				mActorMat = ac->GetComponent<Engine::TransformComponent>()->GetGlobalMatrix();
			}

			mSelectActor = ac;
		}

		if (mSelectActor != nullptr)
		{
			ImGuizmo::Manipulate(mEditorConstants->View.Ptr(),
				mEditorConstants->Proj.Ptr(),
				ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::WORLD, mActorMat.Ptr());

			if (ImGuizmo::IsUsing())
			{
				ac->GetComponent<Engine::TransformComponent>()->SetGlobalTransform(mActorMat);
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
}

#pragma once
#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIPassBase.h"
#include "EngineRuntime/include/Function/Render/RenderResource.h"

namespace Editor
{
	class EditorUIScenePass : public EditorUIPassBase
	{
	public:
		EditorUIScenePass();

		virtual ~EditorUIScenePass() override;

		virtual void Initialize(EditorUIMessage* messageBox, Engine::ImGuiDevice* device, EngineEditor* editor) override;

	public:
		virtual void ShowUI() override;

	private:
		ImVec2 currentWindowSize;

		ImTextureID mSceneRender;

		Engine::ImGuiTextureInfo mIconFileTexture;

		EditorInputSystem* inputSystem = nullptr;

	private:
		Engine::Matrix4x4 mActorMat;

		const Engine::CameraPassConstants* mEditorConstants = nullptr;

		Engine::Actor* mSelectActor = nullptr;
	};
}

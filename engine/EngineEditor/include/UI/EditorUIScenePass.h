#pragma once
#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIPassBase.h"

namespace Editor
{
	class EditorUIScenePass : public EditorUIPassBase
	{
	public:
		EditorUIScenePass();

		virtual ~EditorUIScenePass() override;

		virtual void Initialize(Engine::ImGuiDevice* device, EngineEditor* editor) override;

	public:
		virtual void ShowUI() override;

	private:
		ImVec2 currentWindowSize;

		ImTextureID mSceneRender;

		Engine::ImGuiTextureInfo mIconFileTexture;
	};
}

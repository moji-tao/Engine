#pragma once
#include <memory>
#include <filesystem>
#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIMessage.h"
#include "EngineRuntime/include/Function/Render/ImGuiDevice.h"
#include "EngineRuntime/include/Function/Window/WindowUI.h"

namespace Editor
{
	struct EditorConfig;
	class EngineEditor;
	class EditorUIPassBase;

#define UIProjectFileDrag "UIProjectFileDrag##Drag"
#define HierarchyNodeDrag "HierarchyNodeDrag##Drag"

	class EditorUI : public Engine::WindowUI
	{
	public:
		EditorUI(EngineEditor* editor);

		virtual ~EditorUI() override;

	public:
		virtual void Initialize(const Engine::WindowUIInitInfo& info) override;

		virtual void PreRender() override;

	private:
		void LoadFonts(const std::filesystem::path& ph);

		void ConfigStyleColor();

	private:
		EditorUIPassBase* mProjectUI;
		EditorUIPassBase* mConsoleUI;
		EditorUIPassBase* mHierarchyUI;
		EditorUIPassBase* mSceneUI;
		EditorUIPassBase* mInspectorUI;
		EditorUIPassBase* mAssetAttributeUI;

	private:
		Engine::ImGuiDevice* mDevice = nullptr;

		ImFont* mFontsSimhei = nullptr;

		EngineEditor* mEditor = nullptr;

		EditorUIMessage* mUIMessage = nullptr;

	//test
	private:

	};
}

#pragma once
#include <memory>
#include <filesystem>
#include <imgui/imgui.h>
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
		std::shared_ptr<EditorUIPassBase> mProjectUI;
		std::shared_ptr<EditorUIPassBase> mConsoleUI;
		std::shared_ptr<EditorUIPassBase> mHierarchyUI;
		std::shared_ptr<EditorUIPassBase> mSceneUI;
		std::shared_ptr<EditorUIPassBase> mInspectorUI;

	private:
		Engine::ImGuiDevice* mDevice = nullptr;

		ImFont* mFontsSimhei = nullptr;

		EngineEditor* mEditor = nullptr;

	//test
	private:

	};
}

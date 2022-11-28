#pragma once
#include <memory>
#include <filesystem>
#include <imgui/imgui.h>
#include "EngineRuntime/include/Function/Window/WindowUI.h"
#include "EngineEditor/include/UI/EditorUIPassBase.h"

namespace Editor
{
	struct EditorConfig;

	class EditorUI : public Engine::WindowUI
	{
	public:
		EditorUI(const EditorConfig* config);

		virtual ~EditorUI() override;

	public:
		virtual void Initialize(const Engine::WindowUIInitInfo& info) override;

		virtual void PreRender() override;

	private:
		void LoadFonts(const std::filesystem::path& ph);

	private:
		std::shared_ptr<EditorUIPassBase> mProjectUI;
		std::shared_ptr<EditorUIPassBase> mConsoleUI;
		std::shared_ptr<EditorUIPassBase> mHierarchyUI;
		std::shared_ptr<EditorUIPassBase> mSceneUI;
		std::shared_ptr<EditorUIPassBase> mInspectorUI;

	private:

		ImFont* mFontsSimhei = nullptr;
	};
}

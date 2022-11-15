#pragma once
#include "EngineRuntime/include/Function/Window/WindowUI.h"
#include "EngineRuntime/include/Function/UI/ImGuiRHI.h"
#include "Function/UI/ImGuiRenderer.h"
#include "EngineEditor/include/UI/EditorUIPassBase.h"

namespace Editor
{
	class EditorUI : public Engine::WindowUI
	{
	public:
		EditorUI();

		virtual ~EditorUI() override;

	public:
		virtual void Initialize(const Engine::WindowUIInitInfo& info) override;

		virtual void PreRender() override;


	private:
		std::shared_ptr<EditorUIPassBase> mProjectUI;
		std::shared_ptr<EditorUIPassBase> mConsoleUI;
		std::shared_ptr<EditorUIPassBase> mHierarchyUI;
		std::shared_ptr<EditorUIPassBase> mSceneUI;
		std::shared_ptr<EditorUIPassBase> mInspectorUI;

	private:
		std::shared_ptr<Engine::ImGuiRenderer> mEditorUIRender;

		ImFont* mFontsSimhei = nullptr;
	};
}

#pragma once
#include "EngineRuntime/include/Function/Window/WindowUI.h"

namespace Editor
{
	class EditorUI : public Engine::WindowUI
	{
	public:
		EditorUI() = default;

		virtual ~EditorUI() override;

	public:
		virtual void Initialize(const Engine::WindowUIInitInfo& info) override;

		virtual void PreRender() override;

	private:

	};
}

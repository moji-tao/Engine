#pragma once
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"

namespace Engine
{
	struct WindowUIInitInfo
	{
		WindowSystem* windowSystem;
		RenderSystem* renderSystem;
	};

	class WindowUI
	{
	public:
		virtual ~WindowUI() = default;

	public:
		virtual void Initialize(const WindowUIInitInfo& info) = 0;

		virtual void PreRender() = 0;
	};
}


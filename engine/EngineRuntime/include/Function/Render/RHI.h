#pragma once
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Function/Render/RenderPipeline.h"
#include "EngineRuntime/include/Function/Window/WindowUI.h"
#include "EngineRuntime/include/Function/Render/ImGuiDevice.h"

namespace Engine
{
	class RHI
	{
	public:
		RHI(WindowSystem* windowSystem) { };

		virtual ~RHI() = default;

	public:
		virtual void InitializeRenderPipeline(std::unique_ptr<RenderPipeline>& renderPipeline) = 0;

		virtual void InitEditorUI(std::unique_ptr<ImGuiDevice>& editorUI, WindowUI* windowUI) = 0;

	public:
		virtual void FlushCommandQueue() = 0;

		virtual void ExecuteCommandLists() = 0;

		virtual void ResetCommandList() = 0;

		virtual void ResetCommandAllocator() = 0;

		virtual void Present() = 0;
		
		virtual void EndFrame() = 0;

		virtual void ResizeViewport(int width, int height) = 0;
	};
}

#pragma once
#include "EngineRuntime/include/Function/Render/ImGuiDevice.h"

namespace Engine
{
	class RHI;

	class RenderPipeline
	{
	public:
		RenderPipeline() = default;

		virtual ~RenderPipeline() = default;

	public:
		void InitEditorUI(ImGuiDevice* imguiDevice);

	public:
		virtual void RenderTargetResize(int width, int height) = 0;

		virtual void ForwardRender() = 0;

		virtual void DeferredRender() = 0;

	protected:
		ImGuiDevice* mImGuiDevice = nullptr;

	private:

	};
}

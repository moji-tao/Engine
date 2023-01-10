#pragma once
#include <memory>
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include "EngineRuntime/include/Function/Window/WindowUI.h"
#include "EngineRuntime/include/Function/Render/RHI.h"
#include "EngineRuntime/include/Function/Render/ImGuiDevice.h"
#include "EngineRuntime/include/Function/Render/RenderPipeline.h"

namespace Engine
{
	class RenderSystem : public IRuntimeModule, public ISingleton<RenderSystem>
	{
	public:
		RenderSystem();

		virtual ~RenderSystem() override;

	public:
		virtual bool Initialize(InitConfig* info) override;

		virtual bool Tick(float deltaTile) override;

		virtual void Finalize() override;

		ImGuiDevice* InitializeUIRenderBackend(WindowUI* windowUI);

		void InitializeUIRenderBackendEnd();

		void FinalizeUIRenderBackend();

		void RenderViewResize(int width, int height);

	private:
		std::unique_ptr<RHI> mRHI = nullptr;

		std::unique_ptr<RenderPipeline> mRenderPipeline = nullptr;

		std::unique_ptr<ImGuiDevice> mImGuiDevice = nullptr;
	};
}

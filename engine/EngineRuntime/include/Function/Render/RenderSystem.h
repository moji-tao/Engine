#pragma once
#include <memory>
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include "EngineRuntime/include/Function/Window/WindowUI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12ImGuiDevice.h"

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

		void InitializeUIRenderBackend(WindowUI* windowUI);

		void FinalizeUIRenderBackend();

	private:
		//std::shared_ptr<DirectXRenderBase> mRenderAPI;
		std::unique_ptr<D3D12RHI> mD3DRHI = nullptr;

		std::unique_ptr<D3D12ImGuiDevice> mD3D12ImGuiDevice = nullptr;
	};
}

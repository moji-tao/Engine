#pragma once
#include <memory>
#include "EngineRuntime/include/Function/Render/RenderCamera.h"
#include "EngineRuntime/include/Function/Render/RenderResourceBase.h"
#include "EngineRuntime/include/Function/Render/DeviceManager.h"
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include "EngineRuntime/include/Function/Window/WindowUI.h"

#include "EngineRuntime/include/Function/Render/VertexBuffer.h"

namespace Engine
{
	class RenderSystem : public IRuntimeModule, public ISingleton<RenderSystem>
	{
	public:
		RenderSystem() = default;

		virtual ~RenderSystem() override;

	public:
		virtual bool Initialize(InitConfig* info) override;

		virtual bool Tick(float deltaTile) override;

		virtual void Finalize() override;

		void InitializeUIRenderBackend(WindowUI* windowUI, IRenderPass* editorPass);

		void ResizeEngineContentViewport(float offsetX, float offsetY, float width, float height);

		DeviceManager* GetRenderDeviceManager();

	private:
		std::shared_ptr<DeviceManager> mDeviceManager;

		std::shared_ptr<VertexBuffer> mDDDD;
	};
}

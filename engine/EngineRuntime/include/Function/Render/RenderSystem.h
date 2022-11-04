#pragma once
#include <memory>
#include "EngineRuntime/include/Function/Render/RenderCamera.h"
#include "EngineRuntime/include/Function/Render/RenderResourceBase.h"
#include "EngineRuntime/include/Function/Render/DeviceManager.h"
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"

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

		void InitializeUIRenderBackend();

		void ResizeEngineContentViewport(float offsetX, float offsetY, float width, float height);

	private:
		std::shared_ptr<DeviceManager> mDeviceManager;

		//std::shared_ptr<RenderCamera> mCamera;

		//std::shared_ptr<RenderPipelineBase> mRenderPipeline;

		//std::shared_ptr<RenderResourceBase> mRenderResource;
	};
}

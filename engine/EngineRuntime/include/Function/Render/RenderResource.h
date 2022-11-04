#pragma once
#include <memory>
#include "EngineRuntime/include/Function/Render/RenderPass.h"
#include "EngineRuntime/include/Function/Render/RenderResourceBase.h"
#include "EngineRuntime/include/Function/Render/RenderCamera.h"

namespace Engine
{
	class RenderResource : public RenderResourceBase
	{
	public:
		RenderResource() = default;

		virtual ~RenderResource() override = default;

	public:
		virtual void UploadGlobalRenderResource(std::shared_ptr<RHI> rhi) override;

		virtual void UploadGameObjectRenderResource(std::shared_ptr<RHI> rhi) override;

		virtual void UploadPerFrameBuffer(std::shared_ptr<RHI> rhi, std::shared_ptr<RenderCamera> camera) override;

	private:
		void UpdateObjectCBs();

		void UpdateMainPassCB(std::shared_ptr<RenderCamera> camera);

	private:
		PassConstants mMainPassCB;
	};
}

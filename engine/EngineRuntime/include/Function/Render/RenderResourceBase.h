#pragma once
#include "EngineRuntime/include/Function/Render/RenderCamera.h"

namespace Engine
{
	class RHI;

	class RenderResourceBase
	{
	public:
		RenderResourceBase() = default;

		virtual ~RenderResourceBase() = default;

	public:
		virtual void UploadGlobalRenderResource(std::shared_ptr<RHI> rhi) = 0;

		virtual void UploadGameObjectRenderResource(std::shared_ptr<RHI> rhi) = 0;

		virtual void UploadPerFrameBuffer(std::shared_ptr<RHI> rhi, std::shared_ptr<RenderCamera> camera) = 0;
	};
}

#pragma once
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Function/Render/RenderPass.h"

namespace Engine
{
	enum class RHIType : uint8_t
	{
		D3D12
	};

	class RHI
	{
	public:
		RHI() = default;

		virtual ~RHI() = default;

	public:
		virtual bool Initialize(InitConfig* info) = 0;

		virtual void Finalize() = 0;

		virtual void ResizeEngineContentViewport(float offsetX, float offsetY, float width, float height) = 0;

	public:
		virtual void CommandListReset() = 0;

		virtual void CommandListClose() = 0;

		virtual void ExecuteCommandLists() = 0;

		virtual void WaitForFences() = 0;

		virtual void ResizeWindow() = 0;

		virtual void SubmitRendering() = 0;

		virtual void ResetCommandAllocator() = 0;

		virtual void UploadVertexData(void* data, unsigned long long dataLength) = 0;

		virtual void UploadIndexData(void* data, unsigned long long dataLength) = 0;

		virtual void UploadMainPassData(const PassConstants& passComstants) = 0;

		virtual float GetAspect() = 0;

		virtual void PrepareContext() = 0;

	public:
		RHIType mRHIType;
	};
}

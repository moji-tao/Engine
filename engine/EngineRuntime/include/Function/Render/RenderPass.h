#pragma once
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Function/Render/DeviceManager.h"
#include "nvrhi/nvrhi.h"

namespace Engine
{
	struct ObjectConstants
	{
		Matrix4x4 World = Matrix4x4::IDENTITY;
	};

	struct PassConstants
	{
		Matrix4x4 View = Matrix4x4::IDENTITY;
		Matrix4x4 InvView = Matrix4x4::IDENTITY;
		Matrix4x4 Proj = Matrix4x4::IDENTITY;
		Matrix4x4 InvProj = Matrix4x4::IDENTITY;
		Matrix4x4 ViewProj = Matrix4x4::IDENTITY;
		Matrix4x4 InvViewProj = Matrix4x4::IDENTITY;
		Vector3 EyePosW = Vector3::ZERO;
		float cbPerObjectPad1 = 0.0f;
		Vector2 RenderTargetSize = Vector2::ZERO;
		Vector2 InvRenderTargetSize = Vector2::ZERO;
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;
	};

	class IRenderPass
	{
	public:
		DeviceManager* m_DeviceManager;

	public:
		explicit IRenderPass(DeviceManager* deviceManager)
			: m_DeviceManager(deviceManager)
		{ }

		virtual ~IRenderPass() = default;

		virtual void Render(nvrhi::IFramebuffer* framebuffer, float deltaTile) { }
		virtual void RenderAfter(nvrhi::IFramebuffer* framebuffer, float deltaTile) { }
		virtual void BackBufferResizing() { }
		virtual void BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount) { }

		DeviceManager* GetDeviceManager() const { return m_DeviceManager; }
		nvrhi::IDevice* GetDevice() const { return m_DeviceManager->GetDevice(); }
		uint64_t GetFrameIndex() const { return m_DeviceManager->GetFrameIndex(); }
	};
}

#pragma once
#include <list>
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include "nvrhi/nvrhi.h"

namespace Engine
{
	struct DeviceCreationParameters
	{
		bool startMaximized = false;
		bool startFullscreen = false;
		bool allowModeSwitch = true;
		int windowPosX = -1;            // -1 means use default placement
		int windowPosY = -1;
		uint32_t backBufferWidth = 1280;
		uint32_t backBufferHeight = 720;
		uint32_t refreshRate = 0;
		uint32_t swapChainBufferCount = 3;
		nvrhi::Format swapChainFormat = nvrhi::Format::SRGBA8_UNORM;
		uint32_t swapChainSampleCount = 1;
		uint32_t swapChainSampleQuality = 0;
		uint32_t maxFramesInFlight = 2;
		bool enableDebugRuntime = false;
		bool enableNvrhiValidationLayer = false;
		bool vsyncEnabled = false;
		bool enableRayTracingExtensions = false; // for vulkan
		bool enableComputeQueue = false;
		bool enableCopyQueue = false;
	};

	class IRenderPass;

	class DeviceManager
	{
	public:
		DeviceManager() = default;

		virtual ~DeviceManager() = default;

	public:
		bool Initialize(const DeviceCreationParameters& params);

		void Finalize();

		void AddRenderPassToFront(IRenderPass* pRenderPass);

		void AddRenderPassToBack(IRenderPass* pRenderPass);

		void RemoveRenderPass(IRenderPass* pRenderPass);

		void Render();

		uint64_t GetFrameIndex() const;

	protected:
		void UpdateWindowSize();

		void BackBufferResizing();

		void BackBufferResized();

		virtual bool CreateDeviceAndSwapChain() = 0;

		virtual void DestroyDeviceAndSwapChain() = 0;

		virtual void ResizeSwapChain() = 0;

		virtual void BeginFrame() = 0;

		virtual void Present() = 0;

	protected:
		bool m_windowVisible = false;

		DeviceCreationParameters m_DeviceParams;

		bool m_RequestedVSync = false;

		std::list<IRenderPass*> mRenderPasses;

		std::vector<nvrhi::FramebufferHandle> mSwapChainFramebuffers;

		uint64_t mFrameIndex = 0;

	public:
		virtual nvrhi::IDevice* GetDevice() const = 0;

		virtual const char* GetRendererString() const = 0;

		virtual nvrhi::GraphicsAPI GetGraphicsAPI() const = 0;

		virtual nvrhi::ITexture* GetCurrentBackBuffer() = 0;

		virtual nvrhi::ITexture* GetBackBuffer(uint32_t index) = 0;

		virtual uint32_t GetCurrentBackBufferIndex() = 0;

		virtual uint32_t GetBackBufferCount() = 0;

		nvrhi::IFramebuffer* GetCurrentFramebuffer();

		nvrhi::IFramebuffer* GetFramebuffer(uint32_t index);
	};

	struct DefaultMessageCallback : public nvrhi::IMessageCallback, public ISingleton<DefaultMessageCallback>
	{
		void message(nvrhi::MessageSeverity severity, const char* messageText) override;
	};
}

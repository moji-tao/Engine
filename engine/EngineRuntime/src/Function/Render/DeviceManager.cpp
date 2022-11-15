#include "EngineRuntime/include/Function/Render/DeviceManager.h"
#include "EngineRuntime/include/Function/Render/RHI.h"
#include "EngineRuntime/include/Core/Base/macro.h"

namespace Engine
{
	bool DeviceManager::Initialize(const DeviceCreationParameters& params)
	{
		m_DeviceParams = params;

		m_RequestedVSync = params.vsyncEnabled;

		if (!CreateDeviceAndSwapChain())
		{
			LOG_ERROR("图形API初始化失败");
			return false;
		}

		m_DeviceParams.backBufferWidth = 0;
		m_DeviceParams.backBufferHeight = 0;

		UpdateWindowSize();

		return true;
	}

	void DeviceManager::Finalize()
	{
		mAllRenderAsset.clear();
		mSwapChainFramebuffers.clear();

		DestroyDeviceAndSwapChain();
	}

	void DeviceManager::AddRenderPassToFront(IRenderPass* pRenderPass)
	{
		mRenderPasses.remove(pRenderPass);
		mRenderPasses.push_front(pRenderPass);

		pRenderPass->BackBufferResizing();
		pRenderPass->BackBufferResized(
			m_DeviceParams.backBufferWidth,
			m_DeviceParams.backBufferHeight,
			m_DeviceParams.swapChainSampleCount);
	}

	void DeviceManager::AddRenderPassToBack(IRenderPass* pRenderPass)
	{
		mRenderPasses.remove(pRenderPass);
		mRenderPasses.push_back(pRenderPass);

		pRenderPass->BackBufferResizing();
		pRenderPass->BackBufferResized(
			m_DeviceParams.backBufferWidth,
			m_DeviceParams.backBufferHeight,
			m_DeviceParams.swapChainSampleCount);
	}

	void DeviceManager::RemoveRenderPass(IRenderPass* pRenderPass)
	{
		mRenderPasses.remove(pRenderPass);
	}

	void DeviceManager::Render(float deltaTile)
	{
		UpdateWindowSize();

		BeginFrame();

		nvrhi::IFramebuffer* framebuffer = mSwapChainFramebuffers[GetCurrentBackBufferIndex()];

		for (auto it : mRenderPasses)
		{
			it->Render(framebuffer, deltaTile);
		}

		if (mAllRenderAsset.size() > 0)
		{
			GetDevice()->executeCommandLists(&mAllRenderAsset[0], mAllRenderAsset.size());
		}
		mAllRenderAsset.clear();

		Present();

		GetDevice()->runGarbageCollection();
		
		++mFrameIndex;
	}

	uint64_t DeviceManager::GetFrameIndex() const
	{
		return mFrameIndex;
	}

	void DeviceManager::UpdateWindowSize()
	{
		int width;
		int height;
		width = WindowSystem::GetInstance()->GetWindowWidth();
		height = WindowSystem::GetInstance()->GetWindowHeight();

		if (width == 0 || height == 0)
		{
			// window is minimized
			m_windowVisible = false;
			return;
		}

		m_windowVisible = true;

		if (int(m_DeviceParams.backBufferWidth) != width ||
			int(m_DeviceParams.backBufferHeight) != height ||
			(m_DeviceParams.vsyncEnabled != m_RequestedVSync && GetGraphicsAPI() == nvrhi::GraphicsAPI::VULKAN))
		{
			// window is not minimized, and the size has changed

			BackBufferResizing();

			m_DeviceParams.backBufferWidth = width;
			m_DeviceParams.backBufferHeight = height;
			m_DeviceParams.vsyncEnabled = m_RequestedVSync;

			ResizeSwapChain();
			BackBufferResized();
		}
		
		m_DeviceParams.vsyncEnabled = m_RequestedVSync;
	}

	void DeviceManager::BackBufferResizing()
	{
		mSwapChainFramebuffers.clear();

		for (auto it : mRenderPasses)
		{
			it->BackBufferResizing();
		}
	}

	void DeviceManager::BackBufferResized()
	{
		for (auto it : mRenderPasses)
		{
			it->BackBufferResized(m_DeviceParams.backBufferWidth,
				m_DeviceParams.backBufferHeight,
				m_DeviceParams.swapChainSampleCount);
		}

		uint32_t backBufferCount = GetBackBufferCount();
		mSwapChainFramebuffers.resize(backBufferCount);
		for (uint32_t index = 0; index < backBufferCount; index++)
		{
			mSwapChainFramebuffers[index] = GetDevice()->createFramebuffer(
				nvrhi::FramebufferDesc().addColorAttachment(GetBackBuffer(index)));
		}
	}

	nvrhi::IFramebuffer* DeviceManager::GetCurrentFramebuffer()
	{
		return GetFramebuffer(GetCurrentBackBufferIndex());
	}

	nvrhi::IFramebuffer* DeviceManager::GetFramebuffer(uint32_t index)
	{
		if (index < mSwapChainFramebuffers.size())
			return mSwapChainFramebuffers[index];

		return nullptr;
	}

	void DeviceManager::AddCommandList(nvrhi::CommandListHandle handle)
	{
		mAllRenderAsset.push_back(handle);
	}

	void DefaultMessageCallback::message(nvrhi::MessageSeverity severity, const char* messageText)
	{
		switch (severity)
		{
		case nvrhi::MessageSeverity::Info:
			LOG_INFO("{0}", messageText);
			break;
		case nvrhi::MessageSeverity::Warning:
			LOG_WARN("{0}", messageText);
			break;
		case nvrhi::MessageSeverity::Error:
			LOG_ERROR("{0}", messageText);
			break;
		case nvrhi::MessageSeverity::Fatal:
			LOG_FATAL("{0}", messageText);
			break;
		}
	}
}

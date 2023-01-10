#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"

namespace Engine
{
	RenderSystem::RenderSystem()
	{ }

	RenderSystem::~RenderSystem()
	{ }

	bool RenderSystem::Initialize(InitConfig* info)
	{
		mRHI = std::make_unique<D3D12RHI>(WindowSystem::GetInstance());

		mRHI->ResizeViewport(WindowSystem::GetInstance()->GetWindowWidth(), WindowSystem::GetInstance()->GetWindowHeight());

		WindowSystem::GetInstance()->RegisterWindowResizeCallback([this](int NewWidth, int NewHeight)
			{
				if (NewWidth == 0 && NewHeight == 0)
				{
					return;
				}
				this->mRHI->ResizeViewport(NewWidth, NewHeight);
			});

		mRHI->InitializeRenderPipeline(mRenderPipeline);

		return true;
	}

	bool RenderSystem::Tick(float deltaTile)
	{
		mRenderPipeline->DeferredRender();

		return true;
	}

	void RenderSystem::Finalize()
	{
		mRenderPipeline.reset();

		mImGuiDevice.reset();

		mRHI.reset();
	}

	ImGuiDevice* RenderSystem::InitializeUIRenderBackend(WindowUI* windowUI)
	{
		mRHI->InitEditorUI(mImGuiDevice, windowUI);

		mRenderPipeline->InitEditorUI(mImGuiDevice.get());

		mRHI->ResetCommandAllocator();

		mRHI->ResetCommandList();

		return mImGuiDevice.get();
	}

	void RenderSystem::InitializeUIRenderBackendEnd()
	{
		mRHI->ExecuteCommandLists();

		mRHI->FlushCommandQueue();
	}

	void RenderSystem::FinalizeUIRenderBackend()
	{
		mImGuiDevice.reset();
	}

	void RenderSystem::RenderViewResize(int width, int height)
	{
		ASSERT(mRenderPipeline != nullptr);

		mRenderPipeline->RenderTargetResize(width, height);
	}
}

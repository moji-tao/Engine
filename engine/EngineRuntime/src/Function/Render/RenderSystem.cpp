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
		mEnableTAA = true;

		mRenderCamera = std::make_unique<RenderCamera>();

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

		mRHI->InitializeRenderPipeline(mRenderPipeline, mRenderResource);

		return true;
	}

	bool RenderSystem::Tick(float deltaTile, bool isEditorMode)
	{
		if (isEditorMode)
		{
			mRenderResource->PerFrameBuffer(mEditorRenderCamera, deltaTile);
		}
		else
		{
			mRenderResource->PerFrameBuffer(mRenderCamera.get(), deltaTile);
		}

		//mRenderPipeline->UploadResource();

		mRenderPipeline->Render();

		mRenderResource->EndFrameBuffer();

		++mFrameCount;

		return true;
	}

	void RenderSystem::Finalize()
	{
		mRenderResource.release();

		mRenderPipeline.release();

		mImGuiDevice.release();

		mRHI.release();
	}

	ImGuiDevice* RenderSystem::InitializeUIRenderBackend(WindowUI* windowUI)
	{
		mRHI->InitEditorUI(mImGuiDevice, windowUI);

		mRenderPipeline->InitEditorUI(mImGuiDevice.get());

		mRHI->ResetCommandAllocator();

		mRHI->ResetCommandList();

		return mImGuiDevice.get();
	}

	RenderResource* RenderSystem::GetRenderResource()
	{
		return mRenderResource.get();
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

		mRenderView.x = width;
		mRenderView.y = height;

		mRenderCamera->SetRenderSize(width, height);

		mRenderPipeline->OnResize(width, height);
	}

	void RenderSystem::SetEditorRenderCamera(const RenderCamera& camera)
	{
		mEditorRenderCamera = &camera;
	}

	Vector2 RenderSystem::GetRenderViewSize()
	{
		return mRenderView;
	}

	RenderCamera* RenderSystem::GetRenderCamera()
	{
		return mRenderCamera.get();
	}
}

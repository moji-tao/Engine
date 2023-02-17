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
		if (false)
		{
			if (HMODULE mod = LoadLibraryA("renderdoc.dll"))
			{
				pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
				int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
				assert(ret == 1);
				LOG_INFO("注入RenderDoc成功");
			}
			else
			{
				LOG_ERROR("注入RenderDoc失败 {0}", GetLastError());
				rdoc_api = nullptr;
			}
		}

		if (rdoc_api != nullptr)
		{
			std::string generic_string = EngineFileSystem::GetInstance()->GetActualPath("RenderDoc/").generic_string();

			rdoc_api->SetCaptureFilePathTemplate(generic_string.c_str());
			LOG_INFO("设置截帧保存模板 {0}", generic_string.c_str());
		}

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

		mDrawCallK = mDrawCall;
		mDrawCall = 0;

		return true;
	}

	void RenderSystem::Finalize()
	{
		if (rdoc_api != nullptr)
		{
			rdoc_api->UnloadCrashHandler();
			rdoc_api = nullptr;
		}

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

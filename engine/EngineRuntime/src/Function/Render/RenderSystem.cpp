#include "EngineRuntime/include/Function/Render/RenderSystem.h"

#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12DeviceManager.h"
#include "EngineRuntime/include/Function/UI/ImGuiRenderer.h"

namespace Engine
{
	RenderSystem::~RenderSystem()
	{ }

	bool RenderSystem::Initialize(InitConfig* info)
	{
		/*
		mRHI = std::make_shared<DirectXRender>();
		mRHI->Initialize(info);

		mRenderResource = std::make_shared<RenderResource>();
		mRenderResource->UploadGlobalRenderResource(mRHI);

		mCamera = std::make_shared<RenderCamera>();
		Vector3 mEyePos;
		mEyePos[0] = 15.0f * sinf(0.2f * XM_PI) * cosf(1.5f * XM_PI);
		mEyePos[2] = 15.0f * sinf(0.2f * XM_PI) * sinf(1.5f * XM_PI);
		mEyePos[1] = 15.0f * cosf(0.2f * XM_PI);

		mCamera->LookAt(mEyePos, Vector3::ZERO, Vector3::UNIT_Y);
		mCamera->SetAspect(mRHI->GetAspect());

		mRenderPipeline = std::make_shared<DeviceManager>();
		mRenderPipeline->Initialize();
		*/
		DeviceCreationParameters param;
		param.backBufferWidth = info->WindowWidth;
		param.backBufferHeight = info->WindowHeight;
		param.swapChainBufferCount = 3;
		param.swapChainSampleCount = 1;
		param.startFullscreen = false;
		param.vsyncEnabled = true;
		param.enableComputeQueue = true;
		param.enableCopyQueue = true;

#ifdef _DEBUG
		//param.enableNvrhiValidationLayer = true;
		param.enableDebugRuntime = true;
#endif
		mDeviceManager = std::make_shared<D3D12DeviceManager>();
		if(!mDeviceManager->Initialize(param))
		{
			return false;
		}

		//mBasicTriangle = std::make_shared<BasicTriangle>(mDeviceManager.get());
		//mDeviceManager->AddRenderPassToFront(mBasicTriangle.get());
		mDDDD = std::make_shared<VertexBuffer>(mDeviceManager.get());
		mDeviceManager->AddRenderPassToFront(mDDDD.get());

		return true;
	}

	bool RenderSystem::Tick(float deltaTile)
	{
		mDeviceManager->Render(deltaTile);

		return true;
	}

	void RenderSystem::Finalize()
	{
		mDDDD = nullptr;
		mDeviceManager->Finalize();
		mDeviceManager = nullptr;
	}

	void RenderSystem::InitializeUIRenderBackend(WindowUI* windowUI, IRenderPass* editorPass)
	{
		//mEditorUI = std::make_shared<ImGuiRenderer>(mDeviceManager.get(), windowUI);
		mDeviceManager->AddRenderPassToBack(editorPass);
	}

	void RenderSystem::ResizeEngineContentViewport(float offsetX, float offsetY, float width, float height)
	{
		/*
		mRHI->ResizeEngineContentViewport(offsetX, offsetY, width, height);
		*/
	}

	DeviceManager* RenderSystem::GetRenderDeviceManager()
	{
		return mDeviceManager.get();
	}

}

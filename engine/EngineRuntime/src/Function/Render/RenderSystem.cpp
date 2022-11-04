#include "EngineRuntime/include/Function/Render/RenderSystem.h"

#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12DeviceManager.h"

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
		mDeviceManager = std::make_shared<D3D12DeviceManager>();
		if(!mDeviceManager->Initialize(param))
		{
			return false;
		}

		return true;
	}

	bool RenderSystem::Tick(float deltaTile)
	{
		/*
		mRHI->PrepareContext();

		// 上传数据 MVP
		mRenderResource->UploadPerFrameBuffer(mRHI, mCamera);


		// 渲染
		mRenderPipeline->ForwardRender(mRHI, mRenderResource);
		*/
		mDeviceManager->Render();

		return true;
	}

	void RenderSystem::Finalize()
	{
		mDeviceManager->Finalize();
	}

	void RenderSystem::InitializeUIRenderBackend()
	{
		/*
		mRenderPipeline->InitializeUIRenderBackend();
		*/
	}

	void RenderSystem::ResizeEngineContentViewport(float offsetX, float offsetY, float width, float height)
	{
		/*
		mRHI->ResizeEngineContentViewport(offsetX, offsetY, width, height);
		*/
	}

}

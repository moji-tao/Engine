#include "EngineRuntime/include/Function/Render/DirectX/D3D12RenderPipeline.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Core/Math/Color.h"

namespace Engine
{
	D3D12RenderPipeline::D3D12RenderPipeline(D3D12RHI* rhi)
		:mRHI(rhi)
	{
		D3D12_VIEWPORT ScreenViewport;
		D3D12_RECT ScissorRect;
		mRHI->GetViewport()->GetD3DViewport(ScreenViewport, ScissorRect);
		
		RenderTargetResize(ScreenViewport.Width, ScreenViewport.Height);
	}

	D3D12RenderPipeline::~D3D12RenderPipeline()
	{ }

	void D3D12RenderPipeline::ForwardRender()
	{

	}

	void D3D12RenderPipeline::DeferredRender()
	{
		mRHI->ResetCommandAllocator();
		mRHI->ResetCommandList();



		if (mImGuiDevice != nullptr)
		{
			mImGuiDevice->DrawUI();
		}
		
		/*
* 场景渲染
*/
		ASSERT(mSceneRenderTarget != nullptr);
		TestImGuiSceneWindow();

		mRHI->ExecuteCommandLists();

		if (mImGuiDevice != nullptr)
		{
			mImGuiDevice->EndDraw();
		}

		mRHI->Present();
		mRHI->FlushCommandQueue();
		mRHI->EndFrame();
	}

	void D3D12RenderPipeline::RenderTargetResize(int width, int height)
	{
		if (mImGuiDevice == nullptr)
			return;

		LOG_INFO("设置渲染目标大小 ({0}, {1})", width, height);

		//mSceneRenderTarget = std::make_unique<RenderTarget2D>(mRHI, false, width, height, DXGI_FORMAT_R8G8B8A8_UINT, Colors::White);
		mSceneRenderTarget = std::make_unique<RenderTarget2D>(mRHI, false, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, Colors::Goldenrod);
		mSceneRenderTarget->GetResource()->D3DResource->SetName(L"测试 渲染到纹理");
		//
		mImGuiDevice->SetSceneUITexture(mSceneRenderTarget->GetResource()->D3DResource.Get());
	}

	void D3D12RenderPipeline::TestImGuiSceneWindow()
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();
		D3D12_VIEWPORT ScreenViewport;
		D3D12_RECT ScissorRect;
		mRHI->GetViewport()->GetD3DViewport(ScreenViewport, ScissorRect);
		commandList->RSSetViewports(1, &ScreenViewport);
		commandList->RSSetScissorRects(1, &ScissorRect);

		mRHI->TransitionResource(mSceneRenderTarget->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandList->ClearRenderTargetView(mSceneRenderTarget->GetRTV()->GetDescriptorHandle(), mSceneRenderTarget->GetClearColorPtr(), 0, nullptr);

		auto descriptorCache = mRHI->GetDevice()->GetCommandContext()->GetDescriptorCache();

		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		cpuHandle = descriptorCache->AppendRtvDescriptor(mSceneRenderTarget->GetRTV()->GetDescriptorHandle());

		commandList->OMSetRenderTargets(1, &cpuHandle, true, &mRHI->GetViewport()->GetDepthStencilView()->GetDescriptorHandle());

		mRHI->TransitionResource(mSceneRenderTarget->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}
}

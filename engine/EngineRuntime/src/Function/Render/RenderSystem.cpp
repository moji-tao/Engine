#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"

namespace Engine
{
	RenderSystem::RenderSystem()
	{
		//mRenderAPI = std::make_shared<DirectXRender>();
	}

	RenderSystem::~RenderSystem()
	{
		//mRenderAPI = nullptr;
	}

	bool RenderSystem::Initialize(InitConfig* info)
	{
		//mRenderAPI->Initialize();
		mD3DRHI = std::make_unique<D3D12RHI>((HWND)WindowSystem::GetInstance()->GetWindowHandle(true), WindowSystem::GetInstance()->GetWindowWidth(), WindowSystem::GetInstance()->GetWindowHeight());

		mD3DRHI->ResizeViewport(WindowSystem::GetInstance()->GetWindowWidth(), WindowSystem::GetInstance()->GetWindowHeight());

		return true;
	}

	bool RenderSystem::Tick(float deltaTile)
	{
		mD3DRHI->ResetCommandAllocator();
		mD3DRHI->ResetCommandList();

		mD3D12ImGuiDevice->DrawUI();

		mD3DRHI->ExecuteCommandLists();
		mD3DRHI->Present();
		mD3DRHI->FlushCommandQueue();
		mD3DRHI->EndFrame();
		//mRenderAPI->Tick(deltaTile);
		return true;
	}

	void RenderSystem::Finalize()
	{
		//mRenderAPI->Finalize();
	}

	void RenderSystem::InitializeUIRenderBackend(WindowUI* windowUI)
	{
		mD3D12ImGuiDevice = std::make_unique<D3D12ImGuiDevice>(windowUI);

		mD3DRHI->InitEditorUI(mD3D12ImGuiDevice.get());
	}

	void RenderSystem::FinalizeUIRenderBackend()
	{
		mD3D12ImGuiDevice.reset();
	}
}

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx12.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12ImGuiDevice.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"

namespace Engine
{
	D3D12ImGuiDevice::D3D12ImGuiDevice(WindowUI* editorUI)
		:mEditorUI(editorUI)
	{ }

	D3D12ImGuiDevice::~D3D12ImGuiDevice()
	{
		Destroy();
	}

	void D3D12ImGuiDevice::DrawUI()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		mEditorUI->PreRender();
		ImGui::Render();

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mViewport->GetCurrentBackBuffer()->D3DResource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		
		mCommandList->OMSetRenderTargets(1, &mViewport->GetCurrentBackBufferView()->GetDescriptorHandle(), true, nullptr);
		mCommandList->SetDescriptorHeaps(1, mSRVHeap.GetAddressOf());

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mViewport->GetCurrentBackBuffer()->D3DResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}

	void D3D12ImGuiDevice::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, D3D12Viewport* viewport)
	{
		mDevice = device;
		mCommandList = commandList;
		mViewport = viewport;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOther((GLFWwindow*)Engine::WindowSystem::GetInstance()->GetWindowHandle(false), true);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NodeMask = 0;
		mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(mSRVHeap.ReleaseAndGetAddressOf()));

		ImGui_ImplDX12_Init(mDevice, mViewport->GetSwapChainBufferCount(), DXGI_FORMAT_R8G8B8A8_UNORM, mSRVHeap.Get(), mSRVHeap->GetCPUDescriptorHandleForHeapStart(), mSRVHeap->GetGPUDescriptorHandleForHeapStart());
	}

	void D3D12ImGuiDevice::Destroy()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}

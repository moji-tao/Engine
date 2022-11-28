#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "EngineRuntime/include/Function/Window/WindowUI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Viewport.h"

namespace Engine
{
	class D3D12ImGuiDevice
	{
	public:
		D3D12ImGuiDevice(WindowUI* editorUI);

		~D3D12ImGuiDevice();

		void DrawUI();

		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, D3D12Viewport* viewport);

	private:
		void Destroy();

	private:
		WindowUI* mEditorUI = nullptr;

		D3D12Viewport* mViewport = nullptr;

		ID3D12Device* mDevice = nullptr;

		ID3D12GraphicsCommandList* mCommandList = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSRVHeap;
	};
}

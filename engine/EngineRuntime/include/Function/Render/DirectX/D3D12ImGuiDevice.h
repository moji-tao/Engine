#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "EngineRuntime/include/Function/Render/ImGuiDevice.h"

#include "EngineRuntime/include/Function/Render/DirectX/D3D12Viewport.h"

namespace Engine
{
	class D3D12ImGuiDevice : public ImGuiDevice
	{
	public:
		D3D12ImGuiDevice(WindowUI* editorUI);

		virtual ~D3D12ImGuiDevice() override;

		virtual void DrawUI() override;

		virtual void EndDraw() override;

		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, D3D12Viewport* viewport);

	public:
		virtual ImGuiTextureInfo LoadTexture(const std::filesystem::path& ph) override;

		virtual void SetSceneUITexture(void* resource) override;

		virtual ImTextureID GetSceneTextureInfo() override;

	private:
		void Destroy();

	private:
		D3D12Viewport* mViewport = nullptr;

		ID3D12Device* mDevice = nullptr;

		ID3D12GraphicsCommandList* mCommandList = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSRVHeap;

		std::vector<std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, Microsoft::WRL::ComPtr<ID3D12Resource>>> mTextures;

		unsigned mTextureHeapMaxCount = 10;

		unsigned mUsingHeapCount = 0;

		unsigned mCBV_SRV_UAVDescriptorSize;
	};


}

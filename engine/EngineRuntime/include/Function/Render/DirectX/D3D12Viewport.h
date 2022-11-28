#pragma once
#include <dxgi1_4.h>
#include <Windows.h>
#include "EngineRuntime/include/Function/Render/DirectX/d3dx12.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Resource.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12View.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Texture.h"

namespace Engine
{
	class D3D12RHI;

	struct D3D12ViewportInfo
	{
		HWND WindowHandle;

		DXGI_FORMAT BackBufferFormat;
		DXGI_FORMAT DepthStencilFormat;

		bool bEnable4xMSAA = false;
		unsigned QualityOf4xMSAA = 0;
	};

	class D3D12Viewport
	{
	public:
		D3D12Viewport(D3D12RHI* InD3DRHI, const D3D12ViewportInfo& Info, int Width, int Height);

		~D3D12Viewport();

	public:
		void OnResize(int NewWidth, int NewHeight);

		void GetD3DViewport(D3D12_VIEWPORT& OutD3DViewPort, D3D12_RECT& OutD3DRect);

		void Present();

		int GetSwapChainBufferCount();

		D3D12Resource* GetCurrentBackBuffer() const;

		D3D12RenderTargetView* GetCurrentBackBufferView() const;

		float* GetCurrentBackBufferClearColor() const;

		D3D12DepthStencilView* GetDepthStencilView() const;

		D3D12ShaderResourceView* GetDepthShaderResourceView() const;

		D3D12ViewportInfo GetViewportInfo() const;

	private:
		void Initialze();

		void CreareSwapChain();

	private:
		D3D12RHI* D3DRHI = nullptr;

		D3D12ViewportInfo ViewportInfo;
		int ViewportWidth = 0;
		int ViewportHeight = 0;

		static const int SwapChainBufferCount = 2;
		Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain = nullptr;
		int CurrBackBuffer = 0;

		D3D12TextureRef RenderTargetTextures[SwapChainBufferCount];

		D3D12TextureRef DepthStencilTexture = nullptr;
	};
}

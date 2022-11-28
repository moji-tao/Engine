#include "EngineRuntime/include/Function/Render/DirectX/D3D12Viewport.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"

namespace Engine
{
	D3D12Viewport::D3D12Viewport(D3D12RHI* InD3DRHI, const D3D12ViewportInfo& Info, int Width, int Height)
		:D3DRHI(InD3DRHI), ViewportInfo(Info), ViewportWidth(Width), ViewportHeight(Height)
	{
		Initialze();
	}

	D3D12Viewport::~D3D12Viewport()
	{ }

	void D3D12Viewport::OnResize(int NewWidth, int NewHeight)
	{
		ViewportWidth = NewWidth;
		ViewportHeight = NewHeight;

		// Flush before changing any resources.
		D3DRHI->GetDevice()->GetCommandContext()->FlushCommandQueue();

		D3DRHI->GetDevice()->GetCommandContext()->ResetCommandAllocator();

		D3DRHI->GetDevice()->GetCommandContext()->ResetCommandList();

		// Release the previous resources
		for (UINT i = 0; i < SwapChainBufferCount; i++)
		{
			RenderTargetTextures[i].reset();
		}
		DepthStencilTexture.reset();

		// Resize the swap chain.
		ThrowIfFailed(SwapChain->ResizeBuffers(SwapChainBufferCount, ViewportWidth, ViewportHeight,
			ViewportInfo.BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH), "");

		CurrBackBuffer = 0;

		// Create RenderTargetTextures
		for (UINT i = 0; i < SwapChainBufferCount; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> SwapChainBuffer = nullptr;
			ThrowIfFailed(SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer)), "");

			D3D12_RESOURCE_DESC BackBufferDesc = SwapChainBuffer->GetDesc();
			
			D3D12TextureInfo TextureInfo;
			TextureInfo.RTVFormat = BackBufferDesc.Format;
			TextureInfo.InitState = D3D12_RESOURCE_STATE_PRESENT;
			RenderTargetTextures[i] = D3DRHI->CreateTexture(SwapChainBuffer, TextureInfo, TexCreate_RTV);
		}

		// Create DepthStencilTexture
		D3D12TextureInfo TextureInfo;
		TextureInfo.Type = IMAGE_TYPE::IMAGE_TYPE_2D;
		TextureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TextureInfo.Width = ViewportWidth;
		TextureInfo.Height = ViewportHeight;
		TextureInfo.Depth = 1;
		TextureInfo.MipCount = 1;
		TextureInfo.ArraySize = 1;
		TextureInfo.InitState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		TextureInfo.Format = DXGI_FORMAT_R24G8_TYPELESS;  // Create with a typeless format, support DSV and SRV(for SSAO)
		TextureInfo.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		TextureInfo.SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

		DepthStencilTexture = D3DRHI->CreateTexture(TextureInfo, TexCreate_DSV | TexCreate_SRV);


		// Execute the resize commands.
		D3DRHI->GetDevice()->GetCommandContext()->ExecuteCommandLists();

		// Wait until resize is complete.
		D3DRHI->GetDevice()->GetCommandContext()->FlushCommandQueue();
	}

	void D3D12Viewport::GetD3DViewport(D3D12_VIEWPORT& OutD3DViewPort, D3D12_RECT& OutD3DRect)
	{
		OutD3DViewPort.TopLeftX = 0;
		OutD3DViewPort.TopLeftY = 0;
		OutD3DViewPort.Width = static_cast<float>(ViewportWidth);
		OutD3DViewPort.Height = static_cast<float>(ViewportHeight);
		OutD3DViewPort.MinDepth = 0.0f;
		OutD3DViewPort.MaxDepth = 1.0f;

		OutD3DRect = { 0, 0, ViewportWidth, ViewportHeight };
	}

	void D3D12Viewport::Present()
	{
		// swap the back and front buffers
		ThrowIfFailed(SwapChain->Present(0, 0), "");
		CurrBackBuffer = (CurrBackBuffer + 1) % SwapChainBufferCount;
	}

	int D3D12Viewport::GetSwapChainBufferCount()
	{
		return SwapChainBufferCount;
	}

	D3D12Resource* D3D12Viewport::GetCurrentBackBuffer() const
	{
		return RenderTargetTextures[CurrBackBuffer]->GetResource();
	}

	D3D12RenderTargetView* D3D12Viewport::GetCurrentBackBufferView() const
	{
		return RenderTargetTextures[CurrBackBuffer]->GetRTV();
	}

	float* D3D12Viewport::GetCurrentBackBufferClearColor() const
	{
		return RenderTargetTextures[CurrBackBuffer]->GetRTVClearValuePtr();
	}

	D3D12DepthStencilView* D3D12Viewport::GetDepthStencilView() const
	{
		return DepthStencilTexture->GetDSV();
	}

	D3D12ShaderResourceView* D3D12Viewport::GetDepthShaderResourceView() const
	{
		return DepthStencilTexture->GetSRV();
	}

	D3D12ViewportInfo D3D12Viewport::GetViewportInfo() const
	{
		return ViewportInfo;
	}

	void D3D12Viewport::Initialze()
	{
		CreareSwapChain();
	}

	void D3D12Viewport::CreareSwapChain()
	{
		SwapChain.Reset();

		DXGI_SWAP_CHAIN_DESC Desc;
		Desc.BufferDesc.Width = ViewportWidth;
		Desc.BufferDesc.Height = ViewportHeight;
		Desc.BufferDesc.RefreshRate.Numerator = 60;
		Desc.BufferDesc.RefreshRate.Denominator = 1;
		Desc.BufferDesc.Format = ViewportInfo.BackBufferFormat;
		Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		Desc.SampleDesc.Count = ViewportInfo.bEnable4xMSAA ? 4 : 1;
		Desc.SampleDesc.Quality = ViewportInfo.bEnable4xMSAA ? (ViewportInfo.QualityOf4xMSAA - 1) : 0;
		Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		Desc.BufferCount = SwapChainBufferCount;
		Desc.OutputWindow = ViewportInfo.WindowHandle;
		Desc.Windowed = true;
		Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue = D3DRHI->GetDevice()->GetCommandQueue();

		ThrowIfFailed(D3DRHI->GetDxgiFactory()->CreateSwapChain(CommandQueue.Get(), &Desc, SwapChain.ReleaseAndGetAddressOf()), "创建交换链失败");
	}
}

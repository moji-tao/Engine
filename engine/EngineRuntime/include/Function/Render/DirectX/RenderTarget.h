#pragma once
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Texture.h"

namespace Engine
{
	class RenderTarget
	{
	public:
		RenderTarget(D3D12RHI* InRHI, bool RenderDepth, unsigned InWidth, unsigned InHeight, DXGI_FORMAT InFormat, Vector4 InClearValue = Vector4::ZERO);

		virtual ~RenderTarget();

	public:
		D3D12TextureRef GetTexture();

		D3D12Resource* GetResource();

		DXGI_FORMAT GetFormat();

		Vector4 GetClearColor();

		float* GetClearColorPtr();

	protected:
		bool bRenderDepth = false;

		D3D12RHI* RHI = nullptr;

		D3D12TextureRef D3DTexture = nullptr;

		unsigned Width;

		unsigned Height;

		DXGI_FORMAT Format;

		Vector4 ClearValue;
	};

	class RenderTarget2D : public RenderTarget
	{
	public:
		RenderTarget2D(D3D12RHI* InD3D12RHI, bool RenderDepth, UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat, Vector4 InClearValue = Vector4::ZERO);

		D3D12RenderTargetView* GetRTV() const;

		D3D12DepthStencilView* GetDSV() const;

		D3D12ShaderResourceView* GetSRV() const;

	private:
		void CreateTexture();
	};

	class RenderTargetCube : public RenderTarget
	{
	public:
		RenderTargetCube(D3D12RHI* InD3D12RHI, bool RenderDepth, UINT Size, DXGI_FORMAT InFormat, Vector4 InClearValue = Vector4::ZERO);

		D3D12RenderTargetView* GetRTV(int Index) const;

		D3D12DepthStencilView* GetDSV(int Index) const;

		D3D12ShaderResourceView* GetSRV() const;

	private:
		void CreateTexture();
	};
}

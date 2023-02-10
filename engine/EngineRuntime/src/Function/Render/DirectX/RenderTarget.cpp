#include "EngineRuntime/include/Function/Render/DirectX/RenderTarget.h"

namespace Engine
{
	RenderTarget::RenderTarget(D3D12RHI* InRHI, bool RenderDepth, unsigned InWidth, unsigned InHeight, DXGI_FORMAT InFormat, Vector4 InClearValue)
		:RHI(InRHI), bRenderDepth(RenderDepth), Width(InWidth), Height(InHeight), Format(InFormat), ClearValue(InClearValue)
	{ }

	RenderTarget::~RenderTarget()
	{ }

	D3D12TextureRef RenderTarget::GetTexture()
	{
		return D3DTexture;
	}

	D3D12Resource* RenderTarget::GetResource()
	{
		return D3DTexture->GetResource();
	}

	DXGI_FORMAT RenderTarget::GetFormat()
	{
		return Format;
	}

	Vector4 RenderTarget::GetClearColor()
	{
		return ClearValue;
	}

	float* RenderTarget::GetClearColorPtr()
	{
		return ClearValue.ptr();
	}

	RenderTarget2D::RenderTarget2D(D3D12RHI* InD3D12RHI, bool RenderDepth, UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat, Vector4 InClearValue)
		:RenderTarget(InD3D12RHI, RenderDepth, InWidth, InHeight, InFormat, InClearValue)
	{
		CreateTexture();
	}

	D3D12RenderTargetView* RenderTarget2D::GetRTV() const
	{
		if (bRenderDepth)
		{
			return nullptr;
		}

		return D3DTexture->GetRTV();
	}

	D3D12DepthStencilView* RenderTarget2D::GetDSV() const
	{
		if (bRenderDepth)
		{
			return D3DTexture->GetDSV();
		}

		return nullptr;
	}

	D3D12ShaderResourceView* RenderTarget2D::GetSRV() const
	{
		return D3DTexture->GetSRV();
	}

	void RenderTarget2D::CreateTexture()
	{
		D3D12TextureInfo textureInfo;
		textureInfo.Type = IMAGE_TYPE::IMAGE_TYPE_2D;
		textureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureInfo.Width = Width;
		textureInfo.Height = Height;
		textureInfo.Depth = 1;
		textureInfo.MipCount = 1;
		textureInfo.ArraySize = 1;
		textureInfo.Format = Format;

		if (bRenderDepth)
		{
			textureInfo.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			textureInfo.SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

			D3DTexture = RHI->CreateTexture(textureInfo, TexCreate_DSV | TexCreate_SRV);
		}
		else
		{
			D3DTexture = RHI->CreateTexture(textureInfo, TexCreate_RTV | TexCreate_SRV, ClearValue);
		}
	}

	RenderTargetCube::RenderTargetCube(D3D12RHI* InD3D12RHI, bool RenderDepth, UINT Size, DXGI_FORMAT InFormat, Vector4 InClearValue)
		:RenderTarget(InD3D12RHI, RenderDepth, Size, Size, InFormat, InClearValue)
	{
		CreateTexture();
	}

	D3D12RenderTargetView* RenderTargetCube::GetRTV(int Index) const
	{
		if (bRenderDepth)
		{
			return nullptr;
		}

		return D3DTexture->GetRTV(Index);
	}

	D3D12DepthStencilView* RenderTargetCube::GetDSV(int Index) const
	{
		if (bRenderDepth)
		{
			return D3DTexture->GetDSV(Index);
		}

		return nullptr;
	}

	D3D12ShaderResourceView* RenderTargetCube::GetSRV() const
	{
		return D3DTexture->GetSRV();
	}

	void RenderTargetCube::CreateTexture()
	{
		D3D12TextureInfo textureInfo;
		textureInfo.Type = IMAGE_TYPE::IMAGE_TYPE_CUBE;
		textureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureInfo.Width = Width;
		textureInfo.Height = Height;
		textureInfo.Depth = 1;
		textureInfo.MipCount = 1;
		textureInfo.ArraySize = 6;
		textureInfo.Format = Format;

		if (bRenderDepth)
		{
			textureInfo.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			textureInfo.SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

			D3DTexture = RHI->CreateTexture(textureInfo, TexCreate_CubeDSV | TexCreate_SRV);
		}
		else
		{
			D3DTexture = RHI->CreateTexture(textureInfo, TexCreate_CubeRTV | TexCreate_SRV, ClearValue);
		}
	}
}

#pragma once
#include <memory>
#include "EngineRuntime/include/Core/Math/Vector4.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12View.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Resource.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"

namespace Engine
{
	class D3D12RHI;

	class D3D12Texture
	{
	public:
		static DXGI_FORMAT TransformationToD3DFormat(PIXEL_FORMAT format);

		D3D12Resource* GetResource();

		ID3D12Resource* GetD3DResource();

		void SetRTVClearValue(const Vector4& ClearValue);

		Vector4 GetRTVClearValue();

		float* GetRTVClearValuePtr();

		D3D12ShaderResourceView* GetSRV(unsigned Index = 0);

		void AddSRV(std::unique_ptr<D3D12ShaderResourceView>& SRV);

		D3D12RenderTargetView* GetRTV(unsigned Index = 0);

		void AddRTV(std::unique_ptr<D3D12RenderTargetView>& RTV);

		D3D12DepthStencilView* GetDSV(unsigned Index = 0);

		void AddDSV(std::unique_ptr<D3D12DepthStencilView>& DSV);

		D3D12UnorderedAccessView* GetUAV(unsigned Index = 0);

		void AddUAV(std::unique_ptr<D3D12UnorderedAccessView>& UAV);

	public:
		D3D12ResourceLocation ResourceLocation;

	private:
		std::vector<std::unique_ptr<D3D12ShaderResourceView>> SRVs;

		std::vector<std::unique_ptr<D3D12RenderTargetView>> RTVs;

		std::vector<std::unique_ptr<D3D12DepthStencilView>> DSVs;

		std::vector<std::unique_ptr<D3D12UnorderedAccessView>> UAVs;

	private:
		Vector4 RTVClearValue;
	};

	typedef std::shared_ptr<D3D12Texture> D3D12TextureRef;

	struct D3D12TextureInfo
	{
		IMAGE_TYPE Type = IMAGE_TYPE::IMAGE_TYPE_UNKNOWM;
		D3D12_RESOURCE_DIMENSION Dimension;
		size_t Width;
		size_t Height;
		size_t Depth;
		size_t ArraySize;
		size_t MipCount;
		DXGI_FORMAT Format;

		D3D12_RESOURCE_STATES InitState = D3D12_RESOURCE_STATE_GENERIC_READ;

		DXGI_FORMAT SRVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT RTVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT DSVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT UAVFormat = DXGI_FORMAT_UNKNOWN;
	};

	enum TextureCreateFlags
	{
		TexCreate_None = 0,
		TexCreate_RTV = 1 << 0,
		TexCreate_CubeRTV = 1 << 1,
		TexCreate_DSV = 1 << 2,
		TexCreate_CubeDSV = 1 << 3,
		TexCreate_SRV = 1 << 4,
		TexCreate_UAV = 1 << 5,
	};
}

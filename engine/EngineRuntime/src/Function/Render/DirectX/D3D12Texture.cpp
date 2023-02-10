#include <unordered_map>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Texture.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Core/Base/macro.h"

namespace Engine
{
    D3D12Resource* D3D12Texture::GetResource()
    {
        return ResourceLocation.UnderlyingResource;
    }

    ID3D12Resource* D3D12Texture::GetD3DResource()
    {
        return ResourceLocation.UnderlyingResource->D3DResource.Get();
    }

    void D3D12Texture::SetRTVClearValue(const Vector4& ClearValue)
    {
        RTVClearValue = ClearValue;
    }

    Vector4 D3D12Texture::GetRTVClearValue()
    {
        return RTVClearValue;
    }

    float* D3D12Texture::GetRTVClearValuePtr()
    {
        return RTVClearValue.ptr();
    }

    D3D12ShaderResourceView* D3D12Texture::GetSRV(unsigned Index)
    {
        ASSERT(SRVs.size() > Index);

        return SRVs[Index].get();
    }

    void D3D12Texture::AddSRV(std::unique_ptr<D3D12ShaderResourceView>& SRV)
    {
        SRVs.emplace_back(std::move(SRV));
    }

    D3D12RenderTargetView* D3D12Texture::GetRTV(unsigned Index)
    {
        ASSERT(RTVs.size() > Index);

        return RTVs[Index].get();
    }

    void D3D12Texture::AddRTV(std::unique_ptr<D3D12RenderTargetView>& RTV)
    {
        RTVs.emplace_back(std::move(RTV));
    }

    D3D12DepthStencilView* D3D12Texture::GetDSV(unsigned Index)
    {
        ASSERT(DSVs.size() > Index);

        return DSVs[Index].get();
    }

    void D3D12Texture::AddDSV(std::unique_ptr<D3D12DepthStencilView>& DSV)
    {
        DSVs.emplace_back(std::move(DSV));
    }

    D3D12UnorderedAccessView* D3D12Texture::GetUAV(unsigned Index)
    {
        ASSERT(UAVs.size() > Index);

        return UAVs[Index].get();
    }

    void D3D12Texture::AddUAV(std::unique_ptr<D3D12UnorderedAccessView>& UAV)
    {
        UAVs.emplace_back(std::move(UAV));
    }

    DXGI_FORMAT D3D12Texture::TransformationToD3DFormat(PIXEL_FORMAT format)
    {
        static std::unordered_map<PIXEL_FORMAT, DXGI_FORMAT> mp =
        {
            {PIXEL_FORMAT::PIXEL_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN},
            {PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM},
            {PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB},
            {PIXEL_FORMAT::PIXEL_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT},
            {PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT},
            {PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT},
        };

        return mp.at(format);
    }
}

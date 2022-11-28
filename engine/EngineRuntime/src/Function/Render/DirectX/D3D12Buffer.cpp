#include "EngineRuntime/include/Function/Render/DirectX/D3D12Buffer.h"

namespace Engine
{
	D3D12Buffer::D3D12Buffer()
	{ }

	D3D12Buffer::~D3D12Buffer()
	{ }

	D3D12Resource* D3D12Buffer::GetResource()
	{
		return ResourceLocation.UnderlyingResource;
	}

	D3D12ShaderResourceView* D3D12StructuredBuffer::GetSRV()
	{
		return SRV.get();
	}

	void D3D12StructuredBuffer::SetSRV(std::unique_ptr<D3D12ShaderResourceView>& InSRV)
	{
		SRV = std::move(InSRV);
	}

	D3D12ShaderResourceView* D3D12RWStructuredBuffer::GetSRV()
	{
		return SRV.get();
	}

	void D3D12RWStructuredBuffer::SetSRV(std::unique_ptr<D3D12ShaderResourceView>& InSRV)
	{
		SRV = std::move(InSRV);
	}

	D3D12UnorderedAccessView* D3D12RWStructuredBuffer::GetUAV()
	{
		return UAV.get();
	}

	void D3D12RWStructuredBuffer::SetUAV(std::unique_ptr<D3D12UnorderedAccessView>& InUAV)
	{
		UAV = std::move(InUAV);
	}
}

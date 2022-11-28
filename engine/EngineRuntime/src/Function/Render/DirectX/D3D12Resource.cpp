#include "EngineRuntime/include/Function/Render/DirectX/D3D12Resource.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12MemoryAllocator.h"

namespace Engine
{
	using namespace Microsoft::WRL;

	D3D12Resource::D3D12Resource(ComPtr<ID3D12Resource> InD3DResource, D3D12_RESOURCE_STATES InitState)
		: D3DResource(InD3DResource), CurrentState(InitState)
	{
		if(D3DResource->GetDesc().Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		{
			GPUVirtualAddress = D3DResource->GetGPUVirtualAddress();
		}
	}

	void D3D12Resource::Map()
	{
		ThrowIfFailed(D3DResource->Map(0, nullptr, &MappedBaseAddress), "");
	}

	D3D12ResourceLocation::D3D12ResourceLocation()
	{ }

	D3D12ResourceLocation::~D3D12ResourceLocation()
	{
		ReleaseResource();
	}

	void D3D12ResourceLocation::ReleaseResource()
	{
		switch (ResourceLocationType)
		{
		case D3D12ResourceLocation::ResourceLocationType::StandAlone:
		{
			delete UnderlyingResource;
			break;
		}
		case D3D12ResourceLocation::ResourceLocationType::SubAllocation:
		{
			if(Allocator)
			{
				Allocator->Deallocate(*this);
			}

			break;
		}
		default:
			break;
		}
	}

}

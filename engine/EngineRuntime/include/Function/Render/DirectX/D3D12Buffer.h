#pragma once
#include <memory>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Resource.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12View.h"

namespace Engine
{
	class D3D12Buffer
	{
	public:
		D3D12Buffer();

		virtual ~D3D12Buffer();

	public:
		D3D12Resource* GetResource();

	public:
		D3D12ResourceLocation ResourceLocation;
	};

	class D3D12ConstantBuffer : public D3D12Buffer
	{
		
	};
	typedef std::shared_ptr<D3D12ConstantBuffer> D3D12ConstantBufferRef;

	class D3D12StructuredBuffer : public D3D12Buffer
	{
	public:
		D3D12ShaderResourceView* GetSRV();

		void SetSRV(std::unique_ptr<D3D12ShaderResourceView>& InSRV);

	private:
		std::unique_ptr<D3D12ShaderResourceView> SRV = nullptr;
	};
	typedef std::shared_ptr<D3D12StructuredBuffer> D3D12StructuredBufferRef;

	class D3D12RWStructuredBuffer : public D3D12Buffer
	{
	public:
		D3D12ShaderResourceView* GetSRV();

		void SetSRV(std::unique_ptr<D3D12ShaderResourceView>& InSRV);

		D3D12UnorderedAccessView* GetUAV();

		void SetUAV(std::unique_ptr<D3D12UnorderedAccessView>& InUAV);

	private:
		std::unique_ptr<D3D12ShaderResourceView> SRV = nullptr;

		std::unique_ptr<D3D12UnorderedAccessView> UAV = nullptr;
	};
	typedef std::shared_ptr<D3D12RWStructuredBuffer> D3D12RWStructuredBufferRef;

	class D3D12VertexBuffer : public D3D12Buffer
	{
		
	};
	typedef std::shared_ptr<D3D12VertexBuffer> D3D12VertexBufferRef;

	class D3D12IndexBuffer : public D3D12Buffer
	{

	};
	typedef std::shared_ptr<D3D12IndexBuffer> D3D12IndexBufferRef;

	class D3D12ReadBackBuffer : public D3D12Buffer
	{

	};
	typedef std::shared_ptr<D3D12ReadBackBuffer> D3D12ReadBackBufferRef;
}

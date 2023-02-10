#pragma once
#ifdef _WIN64
#include <comdef.h>
#include <wrl.h>
#include <d3dcommon.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXCollision.h>
#include "EngineRuntime/include/core/Alloter/MemoryPool.h"
#include "EngineRuntime/include/core/base/macro.h"

namespace Engine
{
	class DxException
	{
	public:
		DxException() = default;
		DxException(HRESULT hr, const std::wstring& functionName, int lineNumber);

		std::wstring ToString()const;

		HRESULT ErrorCode = S_OK;
		std::wstring FunctionName;
		int LineNumber = -1;
	};

#define ThrowIfFailed(hr, ...)													\
	{																			\
		HRESULT hr__ = (hr);													\
		if(FAILED(hr__))														\
		{																		\
			LOG_ERROR(__VA_ARGS__);												\
			throw DxException(hr__, L#hr, __LINE__);							\
		}																		\
	}

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif


	class D3DUtil
	{
	public:
		static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
			const std::wstring& filename,
			const D3D_SHADER_MACRO* defines,
			const std::string& entrypoint,
			const std::string& target);

		static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
			ID3D12Device* device,
			ID3D12GraphicsCommandList* cmdList,
			const void* data,
			UINT64 byteSize,
			Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

		static UINT CalcConstantBufferByteSize(UINT byteSize);
	};

	uint32_t AlignArbitrary(uint32_t Val, uint32_t Alignment);

	template<UINT TNameLength>
	inline void SetDebugName(_In_ ID3D12DeviceChild* resource, _In_z_ const wchar_t(&name)[TNameLength]) noexcept
	{
#if !defined(NO_D3D12_DEBUG_NAME) && (defined(_DEBUG) || defined(PROFILE))
		resource->SetName(name);
#else
		UNREFERENCED_PARAMETER(resource);
		UNREFERENCED_PARAMETER(name);
#endif
	}

	struct SubmeshGeometry
	{
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		INT BaseVertexLocation = 0;

		// Bounding box of the geometry defined by this submesh. 
		// This is used in later chapters of the book.
		DirectX::BoundingBox Bounds;
	};


	struct MeshGeometry
	{
		std::string Name;

		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		UINT IndexBufferByteSize = 0;

		std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

		D3D12_VERTEX_BUFFER_VIEW VectexBufferView() const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv;
			vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
			vbv.StrideInBytes = VertexByteStride;
			vbv.SizeInBytes = VertexBufferByteSize;

			return vbv;
		}

		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
		{
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
			ibv.Format = IndexFormat;
			ibv.SizeInBytes = IndexBufferByteSize;

			return ibv;
		}

		void DisposeUploaders()
		{
			VertexBufferUploader = nullptr;
			IndexBufferUploader = nullptr;
		}

	};
}

#endif
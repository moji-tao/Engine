#ifdef _WIN64
#include <d3dcompiler.h>
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"
#include "EngineRuntime/include/Function/Render/DirectX/d3dx12.h"

namespace Engine
{
	using namespace Microsoft;
	using namespace WRL;

	ComPtr<ID3DBlob> D3DUtil::CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
	{
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ComPtr<ID3DBlob> byteCode = nullptr;
		ComPtr<ID3DBlob> errors;

		HRESULT hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(),
			compileFlags, 0, byteCode.GetAddressOf(), errors.GetAddressOf());

		if (errors != nullptr)
		{
			LOG_ERROR((char*)errors->GetBufferPointer());
		}

		ThrowIfFailed(hr, "着色器编译出错");

		return byteCode;
	}

	ComPtr<ID3D12Resource> D3DUtil::CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* data, UINT64 byteSize,
		ComPtr<ID3D12Resource>& uploadBuffer)
	{
		ComPtr<ID3D12Resource> defaultBuffer;

		CD3DX12_HEAP_PROPERTIES properties1(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC desc1 = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
		ThrowIfFailed(device->CreateCommittedResource(&properties1, D3D12_HEAP_FLAG_NONE, &desc1,
			D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&defaultBuffer)), "创建默认堆失败");

		CD3DX12_HEAP_PROPERTIES properties2(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC desc2 = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
		ThrowIfFailed(device->CreateCommittedResource(&properties2, D3D12_HEAP_FLAG_NONE, &desc2,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer)), "创建上传堆失败");

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = data;
		subResourceData.RowPitch = byteSize;
		subResourceData.SlicePitch = byteSize;

		CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		cmdList->ResourceBarrier(1, &transition);

		UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

		transition = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
		cmdList->ResourceBarrier(1, &transition);

		return defaultBuffer;
	}

	UINT D3DUtil::CalcConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}

	uint32_t AlignArbitrary(uint32_t Val, uint32_t Alignment)
	{
		return ((Val + Alignment - 1) / Alignment) * Alignment;
	}

	DxException::DxException(HRESULT hr, const std::wstring& functionName, int lineNumber)
		:ErrorCode(hr), FunctionName(functionName), LineNumber(lineNumber)
	{ }

	std::wstring DxException::ToString() const
	{
		// Get the string description of the error code.
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();

		return FunctionName + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
	}

}
#endif
#pragma once
#ifdef _WIN64
#include <wrl.h>
#include <d3d12.h>
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"
#include "EngineRuntime/include/Function/Render/DirectX/d3dx12.h"

namespace Engine
{
	using namespace Microsoft;
	using namespace WRL;

	template<typename T>
	class UploadBuffer
	{
	public:
		UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) :
			mIsConstantBuffer(isConstantBuffer)
		{
			mElementByteSize = sizeof(T);

			if (isConstantBuffer)
				mElementByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(T));

			ThrowIfFailed(device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(elementCount * mElementByteSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(&mUploadBuffer)), "创建上传堆失败");

			ThrowIfFailed(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)), "内存映射失败");
		}

		UploadBuffer(const UploadBuffer&) = delete;
		UploadBuffer& operator=(const UploadBuffer&) = delete;

		~UploadBuffer()
		{
			if (mUploadBuffer != nullptr)
			{
				mUploadBuffer->Unmap(0, nullptr);
			}
			mMappedData = nullptr;
		}

		ID3D12Resource* Resource() const
		{
			return mUploadBuffer.Get();
		}

		void CopyData(int elementIndex, const T& data)
		{
			memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
		}

	private:
		ComPtr<ID3D12Resource> mUploadBuffer;
		char* mMappedData = nullptr;

		UINT mElementByteSize = 0;
		bool mIsConstantBuffer = false;
	};
}

#endif
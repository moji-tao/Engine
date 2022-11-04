#pragma once
#ifdef _WIN64
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"
#include "EngineRuntime/include/Function/Render/DirectX/UploadBuffer.h"
#include "EngineRuntime/include/Function/Render/RenderPass.h"

namespace Engine
{
	struct FrameResource
	{
		FrameResource(ID3D12Device* device, UINT objectCount, UINT passCount);

		FrameResource(const FrameResource&) = delete;

		FrameResource& operator=(const FrameResource&) = delete;

		~FrameResource();

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCmdListAlloc;

		std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectUpload = nullptr;

		std::unique_ptr<UploadBuffer<PassConstants>> mPassUpload = nullptr;

		UINT64 mFance = 0;
	};

}

#endif

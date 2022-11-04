#ifdef _WIN64
#include "EngineRuntime/include/Function/Render/DirectX/FrameResource.h"

namespace Engine
{
	FrameResource::FrameResource(ID3D12Device* device, UINT objectCount, UINT passCount)
	{
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCmdListAlloc.GetAddressOf())), "命令分配器创建失败");

		mObjectUpload = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);

		mPassUpload = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	}

	FrameResource::~FrameResource()
	{

	}
}

#endif
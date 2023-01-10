#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <imgui/backends/imgui_impl_dx12.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12ImGuiDevice.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Engine
{
	std::shared_ptr<TextureData> GetTexture(const std::filesystem::path& assetUrl, bool isSRGB);

	D3D12ImGuiDevice::D3D12ImGuiDevice(WindowUI* editorUI)
		:ImGuiDevice(editorUI)
	{ }

	D3D12ImGuiDevice::~D3D12ImGuiDevice()
	{
		Destroy();
	}

	void D3D12ImGuiDevice::DrawUI()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		mEditorUI->PreRender();
		ImGui::Render();

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mViewport->GetCurrentBackBuffer()->D3DResource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		
		mCommandList->OMSetRenderTargets(1, &mViewport->GetCurrentBackBufferView()->GetDescriptorHandle(), true, nullptr);
		mCommandList->SetDescriptorHeaps(1, mSRVHeap.GetAddressOf());

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mViewport->GetCurrentBackBuffer()->D3DResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}

	void D3D12ImGuiDevice::EndDraw()
	{
		if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)mCommandList);
		}
	}

	void D3D12ImGuiDevice::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, D3D12Viewport* viewport)
	{
		mDevice = device;
		mCommandList = commandList;
		mViewport = viewport;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOther((GLFWwindow*)Engine::WindowSystem::GetInstance()->GetWindowHandle(false), true);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;
		srvHeapDesc.NumDescriptors = mTextureHeapMaxCount;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NodeMask = 0;
		mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(mSRVHeap.ReleaseAndGetAddressOf()));

		ImGui_ImplDX12_Init(mDevice, mViewport->GetSwapChainBufferCount(), DXGI_FORMAT_R8G8B8A8_UNORM, mSRVHeap.Get(), mSRVHeap->GetCPUDescriptorHandleForHeapStart(), mSRVHeap->GetGPUDescriptorHandleForHeapStart());

		mUsingHeapCount = 2;

		mCBV_SRV_UAVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	ImGuiTextureInfo D3D12ImGuiDevice::LoadTexture(const std::filesystem::path& ph)
	{
		ASSERT(mUsingHeapCount < mTextureHeapMaxCount);

		auto textureHeapCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mSRVHeap->GetCPUDescriptorHandleForHeapStart(), mUsingHeapCount, mCBV_SRV_UAVDescriptorSize);

		std::shared_ptr<TextureData> texture = GetTexture(EngineFileSystem::GetInstance()->GetActualPath(ph), false);
		if (texture == nullptr)
		{
			return ImGuiTextureInfo();
		}

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

		{
			D3D12_RESOURCE_DESC texDesc;
			memset(&texDesc, 0x00, sizeof(texDesc));
			texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			texDesc.Alignment = 0;
			texDesc.Width = texture->Info.mWidth;
			texDesc.Height = texture->Info.mHeight;
			texDesc.DepthOrArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12_HEAP_PROPERTIES props;
			memset(&props, 0x00, sizeof(props));
			props.Type = D3D12_HEAP_TYPE_DEFAULT;
			props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

			//这里创建的时候就指认了COPY_DEST状态，所以在最后要用资源屏障把它重新弄成只读
			ThrowIfFailed(mDevice->CreateCommittedResource(
				&props,
				D3D12_HEAP_FLAG_NONE,
				&texDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&defaultBuffer)
			), "");
			
			//为UploadTexture创建资源
			unsigned uploadPitch = texture->Info.mRowPitch;
			//unsigned uploadPitch = texture->Info.mWidth * 4;
			//unsigned uploadSize = uploadPitch * texture->Info.mHeight;
			unsigned uploadSize = texture->Info.mSlicePitch;
			D3D12_RESOURCE_DESC uploadTexDesc;
			memset(&uploadTexDesc, 0x00, sizeof(uploadTexDesc));
			uploadTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			uploadTexDesc.Alignment = 0;
			uploadTexDesc.Width = uploadSize;
			uploadTexDesc.Height = 1;
			uploadTexDesc.DepthOrArraySize = 1;
			uploadTexDesc.MipLevels = 1;
			uploadTexDesc.Format = DXGI_FORMAT_UNKNOWN;
			uploadTexDesc.SampleDesc.Count = 1;
			uploadTexDesc.SampleDesc.Quality = 0;
			uploadTexDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			uploadTexDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			
			props.Type = D3D12_HEAP_TYPE_UPLOAD;
			props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

			ThrowIfFailed(mDevice->CreateCommittedResource(
				&props,
				D3D12_HEAP_FLAG_NONE,
				&uploadTexDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&uploadBuffer)
			), "");
			
			//映射内存地址,并把贴图数据拷贝到textureUploadHeap里
			void* mapped = nullptr;
			D3D12_RANGE range = { 0, uploadSize };
			uploadBuffer->Map(0, &range, &mapped);
			memcpy(mapped, texture->mPixels, uploadSize);
			uploadBuffer->Unmap(0, &range);

			//拷贝，把textureUploadHeap里的数据拷贝到texture里
			D3D12_TEXTURE_COPY_LOCATION dst = {};
			dst.pResource = defaultBuffer.Get();
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.SubresourceIndex = 0;

			D3D12_TEXTURE_COPY_LOCATION src = {};
			src.pResource = uploadBuffer.Get();
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			src.PlacedFootprint.Footprint.Width = texture->Info.mWidth;
			src.PlacedFootprint.Footprint.Height = texture->Info.mHeight;
			src.PlacedFootprint.Footprint.Depth = 1;
			src.PlacedFootprint.Footprint.RowPitch = uploadPitch;

			mCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

			//插入资源屏障
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = defaultBuffer.Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

			mCommandList->ResourceBarrier(1, &barrier);
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0x00, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = texture->Info.mMipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;

		mDevice->CreateShaderResourceView(defaultBuffer.Get(), &srvDesc, textureHeapCPUHandle);

		mTextures.emplace_back(defaultBuffer, uploadBuffer);

		auto textureHeapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mSRVHeap->GetGPUDescriptorHandleForHeapStart(), mUsingHeapCount, mCBV_SRV_UAVDescriptorSize);

		++mUsingHeapCount;

		ImGuiTextureInfo result;
		result.TextureID = (ImTextureID)textureHeapGPUHandle.ptr;
		result.Width = texture->Info.mWidth;
		result.Height = texture->Info.mHeight;

		return result;
	}

	void D3D12ImGuiDevice::SetSceneUITexture(void* resource)
	{
		auto textureHeapCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mSRVHeap->GetCPUDescriptorHandleForHeapStart(), 1, mCBV_SRV_UAVDescriptorSize);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0x00, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		mDevice->CreateShaderResourceView((ID3D12Resource*)resource, &srvDesc, textureHeapCPUHandle);
	}

	ImTextureID D3D12ImGuiDevice::GetSceneTextureInfo()
	{
		auto textureHeapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mSRVHeap->GetGPUDescriptorHandleForHeapStart(), 1, mCBV_SRV_UAVDescriptorSize);

		return (ImTextureID)textureHeapGPUHandle.ptr;
	}

	void D3D12ImGuiDevice::Destroy()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	std::shared_ptr<TextureData> GetTexture(const std::filesystem::path& assetUrl, bool isSRGB)
	{
		std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

		int iw, ih, n;
		void* oldImage = stbi_load(assetUrl.generic_string().c_str(), &iw, &ih, &n, 4);
		if (iw % 256 != 0)
		{
			int new_width = (int)((float)iw / 256 + 0.5) * 256;
			int new_height = (int)(ih * ((float)new_width / iw) + 0.5);
			texture->mPixels = malloc(new_width * 4 * new_height);
			stbir_resize_uint8((const unsigned char*)oldImage, iw, ih, 0, (unsigned char*)texture->mPixels, new_width, new_height, 0, 4);
			stbi_image_free(oldImage);
			iw = new_width;
			ih = new_height;
		}
		else
		{
			texture->mPixels = oldImage;
		}
		
		if (!texture->mPixels)
			return nullptr;

		//texture->Info.mRowPitch = (iw * 4 * sizeof(uint8_t) + 256 - 1u) & ~(256 - 1u);
		texture->Info.mRowPitch = iw * 4 * sizeof(uint8_t);
		texture->Info.mSlicePitch = iw * ih * 4 * sizeof(uint8_t);
		texture->Info.mWidth = iw;
		texture->Info.mHeight = ih;
		texture->Info.mFormat = (isSRGB) ? PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_SRGB : PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM;
		texture->Info.mDepth = 1;
		texture->Info.mArrayLayers = 1;
		texture->Info.mMipLevels = 1;
		texture->Info.mType = IMAGE_TYPE::IMAGE_TYPE_2D;

		return texture;
	}
}

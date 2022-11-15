#pragma once
#include <nvrhi/nvrhi.h>
#include <imgui/imgui.h>

#include <memory>
#include <vector>
#include <unordered_map>
#include <stdint.h>

#include "EngineRuntime/include/Function/Render/DeviceManager.h"

namespace Engine
{
    struct ImGuiRHI
    {
        DeviceManager* mDeviceManager;
        nvrhi::CommandListHandle m_commandList;

        nvrhi::ShaderHandle vertexShader;
        nvrhi::ShaderHandle pixelShader;
        nvrhi::InputLayoutHandle shaderAttribLayout;

        nvrhi::TextureHandle fontTexture;
        nvrhi::SamplerHandle fontSampler;

        nvrhi::BufferHandle vertexBuffer;
        nvrhi::BufferHandle indexBuffer;

        nvrhi::BindingLayoutHandle bindingLayout;
        nvrhi::GraphicsPipelineDesc basePSODesc;

        nvrhi::GraphicsPipelineHandle pso;
        std::unordered_map<nvrhi::ITexture*, nvrhi::BindingSetHandle> bindingsCache;

        std::vector<ImDrawVert> vtxBuffer;
        std::vector<ImDrawIdx> idxBuffer;

        bool init(DeviceManager* deviceManager);
        bool beginFrame(float elapsedTimeSeconds);
        bool render(nvrhi::IFramebuffer* framebuffer);
        void backbufferResizing();

    private:
        bool reallocateBuffer(nvrhi::BufferHandle& buffer, size_t requiredSize, size_t reallocateSize, bool isIndexBuffer);

        bool createFontTexture(nvrhi::ICommandList* commandList);

        nvrhi::IGraphicsPipeline* getPSO(nvrhi::IFramebuffer* fb);
        nvrhi::IBindingSet* getBindingSet(nvrhi::ITexture* texture);
        bool updateGeometry(nvrhi::ICommandList* commandList);
    };

}

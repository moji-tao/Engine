#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Function/UI/ImGuiRHI.h"
#include "EngineRuntime/include/Function/Render/ShaderUtil.h"

namespace Engine
{
    struct VERTEX_CONSTANT_BUFFER
    {
        float mvp[4][4];
    };

    bool ImGuiRHI::createFontTexture(nvrhi::ICommandList* commandList)
    {
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels = nullptr;
        int width, height;

        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        assert(pixels != nullptr);

        {
            nvrhi::TextureDesc desc;
            desc.width = width;
            desc.height = height;
            desc.format = nvrhi::Format::RGBA8_UNORM;
            desc.debugName = "ImGui font texture";
            
            fontTexture = mDeviceManager->GetDevice()->createTexture(desc);

            commandList->beginTrackingTextureState(fontTexture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);

            if (fontTexture == nullptr)
                return false;

            commandList->writeTexture(fontTexture, 0, 0, pixels, width * 4);

            commandList->setPermanentTextureState(fontTexture, nvrhi::ResourceStates::ShaderResource);
            commandList->commitBarriers();

            io.Fonts->TexID = fontTexture;
        }

        {
            const auto desc = nvrhi::SamplerDesc()
                .setAllAddressModes(nvrhi::SamplerAddressMode::Wrap)
                .setAllFilters(true);

            fontSampler = mDeviceManager->GetDevice()->createSampler(desc);

            if (fontSampler == nullptr)
                return false;
        }

        return true;
    }

    bool ImGuiRHI::init(DeviceManager* deviceManager)
    {
        mDeviceManager = deviceManager;
        nvrhi::IDevice* renderer = deviceManager->GetDevice();

        m_commandList = renderer->createCommandList();
        m_commandList->open();

        vertexShader = ShaderUtil::CreateShaderFormEngineFolder(renderer, "UI/Shaders/imgui_vertex.cso", "main", nullptr, nvrhi::ShaderType::Vertex);
        // vertexShader = ShaderUtil::CompileShader(renderer, EngineFileSystem::GetInstance()->GetActualPath("UI/Shaders/imgui_vertex.hlsl"), "main", nvrhi::ShaderType::Vertex);
        if (vertexShader == nullptr)
        {
            printf("error creating NVRHI vertex shader object\n");
            assert(0);
            return false;
        }

        pixelShader = ShaderUtil::CreateShaderFormEngineFolder(renderer, "UI/Shaders/imgui_pixel.cso", "main", nullptr, nvrhi::ShaderType::Pixel);
        // pixelShader = ShaderUtil::CompileShader(renderer, EngineFileSystem::GetInstance()->GetActualPath("UI/Shaders/imgui_pixel.hlsl"), "main", nvrhi::ShaderType::Pixel);
        if (pixelShader == nullptr)
        {
            printf("error creating NVRHI pixel shader object\n");
            assert(0);
            return false;
        }

        nvrhi::VertexAttributeDesc vertexAttribLayout[] = {
            nvrhi::VertexAttributeDesc().setName("POSITION").setFormat(nvrhi::Format::RG32_FLOAT).setOffset(offsetof(ImDrawVert,pos)).setElementStride(sizeof(ImDrawVert)),
            nvrhi::VertexAttributeDesc().setName("TEXCOORD").setFormat(nvrhi::Format::RG32_FLOAT).setOffset(offsetof(ImDrawVert,uv)).setElementStride(sizeof(ImDrawVert)),
            nvrhi::VertexAttributeDesc().setName("COLOR").setFormat(nvrhi::Format::RGBA8_UNORM).setOffset(offsetof(ImDrawVert,col)).setElementStride(sizeof(ImDrawVert)),
        };
        
        shaderAttribLayout = renderer->createInputLayout(vertexAttribLayout, uint32_t(std::size(vertexAttribLayout)), vertexShader);

        // create font texture
        if (!createFontTexture(m_commandList))
        {
            return false;
        }

        // create PSO
        {
            nvrhi::BlendState blendState;
            blendState.targets[0].setBlendEnable(true)
                .setSrcBlend(nvrhi::BlendFactor::SrcAlpha)
                .setDestBlend(nvrhi::BlendFactor::InvSrcAlpha)
                .setSrcBlendAlpha(nvrhi::BlendFactor::InvSrcAlpha)
                .setDestBlendAlpha(nvrhi::BlendFactor::Zero);

            auto rasterState = nvrhi::RasterState()
                .setFillSolid()
                .setCullNone()
                .setScissorEnable(true)
                .setDepthClipEnable(true);

            auto depthStencilState = nvrhi::DepthStencilState()
                .disableDepthTest()
                .enableDepthWrite()
                .disableStencil()
                .setDepthFunc(nvrhi::ComparisonFunc::Always);

            nvrhi::RenderState renderState;
            renderState.setBlendState(blendState)
                .setDepthStencilState(depthStencilState)
                .setRasterState(rasterState);

            nvrhi::BindingLayoutDesc layoutDesc;
            layoutDesc.setVisibility(nvrhi::ShaderType::All)
                .addItem(nvrhi::BindingLayoutItem::PushConstants(0, sizeof(float) * 2))
                .addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))
                .addItem(nvrhi::BindingLayoutItem::Sampler(0));
            bindingLayout = renderer->createBindingLayout(layoutDesc);

            basePSODesc.setInputLayout(shaderAttribLayout)
                .setVertexShader(vertexShader)
                .setPixelShader(pixelShader)
                .addBindingLayout(bindingLayout)
                .setRenderState(renderState);
        }

        auto& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();

        m_commandList->close();
        renderer->executeCommandList(m_commandList);

        return true;
    }

    bool ImGuiRHI::reallocateBuffer(nvrhi::BufferHandle& buffer, size_t requiredSize, size_t reallocateSize, const bool indexBuffer)
    {
        if (buffer == nullptr || size_t(buffer->getDesc().byteSize) < requiredSize)
        {
            nvrhi::BufferDesc desc;
            desc.byteSize = uint32_t(reallocateSize);
            desc.structStride = 0;
            desc.debugName = indexBuffer ? "ImGui index buffer" : "ImGui vertex buffer";
            desc.canHaveUAVs = false;
            desc.isVertexBuffer = !indexBuffer;
            desc.isIndexBuffer = indexBuffer;
            desc.isDrawIndirectArgs = false;
            desc.isVolatile = false;
            desc.initialState = indexBuffer ? nvrhi::ResourceStates::IndexBuffer : nvrhi::ResourceStates::VertexBuffer;
            desc.keepInitialState = true;

            buffer = mDeviceManager->GetDevice()->createBuffer(desc);

            if (!buffer)
            {
                return false;
            }
        }

        return true;
    }

    bool ImGuiRHI::beginFrame(float elapsedTimeSeconds)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = elapsedTimeSeconds;
        io.MouseDrawCursor = false;
        ImGui::NewFrame();
        
        return true;
    }

    nvrhi::IGraphicsPipeline* ImGuiRHI::getPSO(nvrhi::IFramebuffer* fb)
    {
        if (pso)
            return pso;
        
        pso = mDeviceManager->GetDevice()->createGraphicsPipeline(basePSODesc, fb);
        assert(pso);

        return pso;
    }

    nvrhi::IBindingSet* ImGuiRHI::getBindingSet(nvrhi::ITexture* texture)
    {
        auto iter = bindingsCache.find(texture);
        if (iter != bindingsCache.end())
        {
            return iter->second;
        }

        nvrhi::BindingSetDesc desc;

        desc.bindings = {
            nvrhi::BindingSetItem::PushConstants(0, sizeof(float) * 2),
            nvrhi::BindingSetItem::Texture_SRV(0, texture),
            nvrhi::BindingSetItem::Sampler(0, fontSampler)
        };
        
        nvrhi::BindingSetHandle binding;
        binding = mDeviceManager->GetDevice()->createBindingSet(desc, bindingLayout);
        assert(binding);

        bindingsCache[texture] = binding;
        return binding;
    }

    bool ImGuiRHI::updateGeometry(nvrhi::ICommandList* commandList)
    {
        ImDrawData* drawData = ImGui::GetDrawData();

        // create/resize vertex and index buffers if needed
        if (!reallocateBuffer(vertexBuffer,
            drawData->TotalVtxCount * sizeof(ImDrawVert),
            (drawData->TotalVtxCount + 5000) * sizeof(ImDrawVert),
            false))
        {
            return false;
        }

        if (!reallocateBuffer(indexBuffer,
            drawData->TotalIdxCount * sizeof(ImDrawIdx),
            (drawData->TotalIdxCount + 5000) * sizeof(ImDrawIdx),
            true))
        {
            return false;
        }

        vtxBuffer.resize(vertexBuffer->getDesc().byteSize / sizeof(ImDrawVert));
        idxBuffer.resize(indexBuffer->getDesc().byteSize / sizeof(ImDrawIdx));

        // copy and convert all vertices into a single contiguous buffer
        ImDrawVert* vtxDst = &vtxBuffer[0];
        ImDrawIdx* idxDst = &idxBuffer[0];

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* cmdList = drawData->CmdLists[n];

            memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

            vtxDst += cmdList->VtxBuffer.Size;
            idxDst += cmdList->IdxBuffer.Size;
        }

        commandList->writeBuffer(vertexBuffer, &vtxBuffer[0], vertexBuffer->getDesc().byteSize);
        commandList->writeBuffer(indexBuffer, &idxBuffer[0], indexBuffer->getDesc().byteSize);

        return true;
    }

    bool ImGuiRHI::render(nvrhi::IFramebuffer* framebuffer)
    {
        ImDrawData* drawData = ImGui::GetDrawData();
        const auto& io = ImGui::GetIO();

        m_commandList->open();
        
        m_commandList->beginMarker("ImGUI");

        if (!updateGeometry(m_commandList))
        {
            return false;
        }

        // handle DPI scaling
        drawData->ScaleClipRects(io.DisplayFramebufferScale);

        float invDisplaySize[2] = { 1.f / io.DisplaySize.x, 1.f / io.DisplaySize.y };

        // set up graphics state
        nvrhi::GraphicsState drawState;

        drawState.framebuffer = framebuffer;
        assert(drawState.framebuffer);

        drawState.pipeline = getPSO(drawState.framebuffer);

        drawState.viewport.viewports.push_back(nvrhi::Viewport(io.DisplaySize.x * io.DisplayFramebufferScale.x,
            io.DisplaySize.y * io.DisplayFramebufferScale.y));
        drawState.viewport.scissorRects.resize(1);  // updated below

        nvrhi::VertexBufferBinding vbufBinding;
        vbufBinding.buffer = vertexBuffer;
        vbufBinding.slot = 0;
        vbufBinding.offset = 0;
        drawState.vertexBuffers.push_back(vbufBinding);

        drawState.indexBuffer.buffer = indexBuffer;
        drawState.indexBuffer.format = (sizeof(ImDrawIdx) == 2 ? nvrhi::Format::R16_UINT : nvrhi::Format::R32_UINT);
        drawState.indexBuffer.offset = 0;

        // render command lists
        int vtxOffset = 0;
        int idxOffset = 0;
        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* cmdList = drawData->CmdLists[n];
            for (int i = 0; i < cmdList->CmdBuffer.Size; i++)
            {
                const ImDrawCmd* pCmd = &cmdList->CmdBuffer[i];

                if (pCmd->UserCallback)
                {
                    pCmd->UserCallback(cmdList, pCmd);
                }
                else
                {
                    drawState.bindings = { getBindingSet((nvrhi::ITexture*)pCmd->TextureId) };
                    assert(drawState.bindings[0]);

                    drawState.viewport.scissorRects[0] = nvrhi::Rect(int(pCmd->ClipRect.x),
                        int(pCmd->ClipRect.z),
                        int(pCmd->ClipRect.y),
                        int(pCmd->ClipRect.w));

                    nvrhi::DrawArguments drawArguments;
                    drawArguments.vertexCount = pCmd->ElemCount;
                    drawArguments.startIndexLocation = idxOffset;
                    drawArguments.startVertexLocation = vtxOffset;

                    m_commandList->setGraphicsState(drawState);
                    m_commandList->setPushConstants(invDisplaySize, sizeof(invDisplaySize));
                    m_commandList->drawIndexed(drawArguments);
                }

                idxOffset += pCmd->ElemCount;
            }

            vtxOffset += cmdList->VtxBuffer.Size;
        }

        m_commandList->endMarker();
        m_commandList->close();
        mDeviceManager->AddCommandList(m_commandList);

    	return true;
    }

    void ImGuiRHI::backbufferResizing()
    {
        pso = nullptr;
    }

}

#pragma once
#include "RenderPass.h"

namespace Engine
{
	class VertexBuffer : public IRenderPass
	{
    private:
        nvrhi::ShaderHandle m_VertexShader;
        nvrhi::ShaderHandle m_PixelShader;
        nvrhi::BufferHandle m_ConstantBuffer;
        nvrhi::BufferHandle m_VertexBuffer;
        nvrhi::BufferHandle m_IndexBuffer;
        nvrhi::TextureHandle m_Texture;
        nvrhi::InputLayoutHandle m_InputLayout;
        nvrhi::BindingLayoutHandle m_BindingLayout;
        nvrhi::BindingSetHandle m_BindingSet;
        nvrhi::GraphicsPipelineHandle m_Pipeline;
        nvrhi::CommandListHandle m_CommandList;
        float m_Rotation = 0.f;

    public:
        VertexBuffer(DeviceManager* manager);
        
        bool Init();

        void BackBufferResizing() override;

        void Render(nvrhi::IFramebuffer* framebuffer, float deltaTile) override;

	};
}

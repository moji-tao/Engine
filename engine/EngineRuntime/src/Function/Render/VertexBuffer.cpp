#include "EngineRuntime/include/Function/Render/VertexBuffer.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Core/Math/Color.h"
#include "EngineRuntime/include/Core/Math/Matrix3x3.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Vector4.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Function/Render/ShaderUtil.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "nvrhi/utils.h"

namespace Engine
{
    struct Vertex1
    {
        Vector3 position;
        Vector4 color;
    };

    const Vertex1 g_Vertices[] = {
    { {-0.5f,  0.5f, -0.5f}, Colors::AliceBlue }, // front face
    { { 0.5f, -0.5f, -0.5f}, Colors::AntiqueWhite },
    { {-0.5f, -0.5f, -0.5f}, Colors::Aqua },
    { { 0.5f,  0.5f, -0.5f}, Colors::Aquamarine },

    { { 0.5f, -0.5f, -0.5f}, Colors::Azure }, // right side face
    { { 0.5f,  0.5f,  0.5f}, Colors::Beige },
    { { 0.5f, -0.5f,  0.5f}, Colors::Bisque },
    { { 0.5f,  0.5f, -0.5f}, Colors::Black },

    { {-0.5f,  0.5f,  0.5f}, Colors::BlanchedAlmond }, // left side face
    { {-0.5f, -0.5f, -0.5f}, Colors::Blue },
    { {-0.5f, -0.5f,  0.5f}, Colors::BlueViolet },
    { {-0.5f,  0.5f, -0.5f}, Colors::Brown },

    { { 0.5f,  0.5f,  0.5f}, Colors::BurlyWood }, // back face
    { {-0.5f, -0.5f,  0.5f}, Colors::CadetBlue },
    { { 0.5f, -0.5f,  0.5f}, Colors::Chartreuse },
    { {-0.5f,  0.5f,  0.5f}, Colors::Chocolate },

    { {-0.5f,  0.5f, -0.5f}, Colors::Coral }, // top face
    { { 0.5f,  0.5f,  0.5f}, Colors::CornflowerBlue },
    { { 0.5f,  0.5f, -0.5f}, Colors::Cornsilk },
    { {-0.5f,  0.5f,  0.5f}, Colors::Crimson },

    { { 0.5f, -0.5f,  0.5f}, Colors::Cyan }, // bottom face
    { {-0.5f, -0.5f, -0.5f}, Colors::DarkBlue },
    { { 0.5f, -0.5f, -0.5f}, Colors::DarkCyan },
    { {-0.5f, -0.5f,  0.5f}, Colors::DarkOrange },
    };

    const uint32_t g_Indices[] = {
         0,  1,  2,   0,  3,  1, // front face
         4,  5,  6,   4,  7,  5, // left face
         8,  9, 10,   8, 11,  9, // right face
        12, 13, 14,  12, 15, 13, // back face
        16, 17, 18,  16, 19, 17, // top face
        20, 21, 22,  20, 23, 21, // bottom face
    };

    VertexBuffer::VertexBuffer(DeviceManager* manager) : IRenderPass(manager) { Init(); }

    bool VertexBuffer::Init()
    {
        m_VertexShader = ShaderUtil::CreateShaderFormProjectFolder(GetDevice(), ProjectFileSystem::GetInstance()->GetActualPath("Assets/shadersV.cso"), "main", nullptr, nvrhi::ShaderType::Vertex);
        m_PixelShader = ShaderUtil::CreateShaderFormProjectFolder(GetDevice(), ProjectFileSystem::GetInstance()->GetActualPath("Assets/shadersP.cso"), "main", nullptr, nvrhi::ShaderType::Pixel);

        if (!m_VertexShader || !m_PixelShader)
        {
            return false;
        }

        m_ConstantBuffer = GetDevice()->createBuffer(nvrhi::utils::CreateVolatileConstantBufferDesc(sizeof(Matrix4x4), "ConstantBuffer", 16));

        nvrhi::VertexAttributeDesc attributes[] = {
            nvrhi::VertexAttributeDesc()
                .setName("POSITION")
                .setFormat(nvrhi::Format::RGB32_FLOAT)
                .setOffset(offsetof(Vertex1, position))
                .setElementStride(sizeof(Vertex1)),
            nvrhi::VertexAttributeDesc()
                .setName("COLOR")
                .setFormat(nvrhi::Format::RGBA32_FLOAT)
                .setOffset(offsetof(Vertex1, color))
                .setElementStride(sizeof(Vertex1)),
        };
        m_InputLayout = GetDevice()->createInputLayout(attributes, uint32_t(std::size(attributes)), m_VertexShader);

        m_CommandList = GetDevice()->createCommandList();
        m_CommandList->open();

        nvrhi::BufferDesc vertexBufferDesc;
        vertexBufferDesc.byteSize = sizeof(g_Vertices);
        vertexBufferDesc.isVertexBuffer = true;
        vertexBufferDesc.debugName = "VertexBuffer";
        vertexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
        m_VertexBuffer = GetDevice()->createBuffer(vertexBufferDesc);

        m_CommandList->beginTrackingBufferState(m_VertexBuffer, nvrhi::ResourceStates::CopyDest);
        m_CommandList->writeBuffer(m_VertexBuffer, g_Vertices, sizeof(g_Vertices));
        m_CommandList->setPermanentBufferState(m_VertexBuffer, nvrhi::ResourceStates::VertexBuffer);

        nvrhi::BufferDesc indexBufferDesc;
        indexBufferDesc.byteSize = sizeof(g_Indices);
        indexBufferDesc.isIndexBuffer = true;
        indexBufferDesc.debugName = "IndexBuffer";
        indexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
        m_IndexBuffer = GetDevice()->createBuffer(indexBufferDesc);

        m_CommandList->beginTrackingBufferState(m_IndexBuffer, nvrhi::ResourceStates::CopyDest);
        m_CommandList->writeBuffer(m_IndexBuffer, g_Indices, sizeof(g_Indices));
        m_CommandList->setPermanentBufferState(m_IndexBuffer, nvrhi::ResourceStates::IndexBuffer);

        m_CommandList->close();
        GetDevice()->executeCommandList(m_CommandList);


        nvrhi::BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            nvrhi::BindingSetItem::ConstantBuffer(0, m_ConstantBuffer),
        };

        if (!nvrhi::utils::CreateBindingSetAndLayout(GetDevice(), nvrhi::ShaderType::All, 0, bindingSetDesc, m_BindingLayout, m_BindingSet))
        {
            LOG_ERROR("Couldn't create the binding set or layout");
            return false;
        }

        return true;
    }

    void VertexBuffer::BackBufferResizing()
    {
        m_Pipeline = nullptr;
    }

    void VertexBuffer::Render(nvrhi::IFramebuffer* framebuffer, float deltaTile)
    {
        m_Rotation += deltaTile * 1.1f;

        const nvrhi::FramebufferInfo& fbinfo = framebuffer->getFramebufferInfo();

        if (!m_Pipeline)
        {
            nvrhi::GraphicsPipelineDesc psoDesc;
            psoDesc.VS = m_VertexShader;
            psoDesc.PS = m_PixelShader;
            psoDesc.inputLayout = m_InputLayout;
            psoDesc.bindingLayouts = { m_BindingLayout };
            psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
            psoDesc.renderState.depthStencilState.depthTestEnable = false;

            m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
        }

        m_CommandList->open();

        nvrhi::utils::ClearColorAttachment(m_CommandList, framebuffer, 0, nvrhi::Color(0.f));

        Matrix4x4 proj = Math::MakePerspectiveMatrix(Radian(Degree(60.0f)), fbinfo.width / fbinfo.height, 0.1f, 10.0f);
        Vector3 eyepos;
        eyepos[0] = m_Rotation * sinf(0) * cosf(0);
        eyepos[2] = m_Rotation * sinf(0) * sinf(0);
        eyepos[1] = m_Rotation * cosf(0);

        Matrix4x4 view = Math::MakeLookAtMatrix(eyepos, Vector3::ZERO, Vector3::UNIT_Y);
        Matrix4x4 vp = proj * view;
        m_CommandList->writeBuffer(m_ConstantBuffer, &vp, sizeof(vp));

        nvrhi::GraphicsState state;
        state.bindings = { m_BindingSet };
        state.indexBuffer = { m_IndexBuffer, nvrhi::Format::R32_UINT, 0 };
        state.vertexBuffers = { { m_VertexBuffer, 0, 0 } };
        state.pipeline = m_Pipeline;
        state.framebuffer = framebuffer;
        state.viewport.addViewportAndScissorRect(fbinfo.getViewport());

        m_CommandList->setGraphicsState(state);

        nvrhi::DrawArguments args;
        args.vertexCount = 12;
        m_CommandList->drawIndexed(args);

        m_CommandList->close();
        //GetDevice()->executeCommandList(m_CommandList);
        m_DeviceManager->AddCommandList(m_CommandList);
    }


}

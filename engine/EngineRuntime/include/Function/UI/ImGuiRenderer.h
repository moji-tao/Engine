#pragma once
#include <filesystem>

#include "EngineRuntime/include/Function/Render/RenderPass.h"
#include "EngineRuntime/include/Function/UI/ImGuiRHI.h"
#include "EngineRuntime/include/Function/Window/WindowUI.h"
#include "GLFW/glfw3.h"

namespace Engine
{
    class ImGuiRenderer : public IRenderPass
    {
    public:
        ImGuiRenderer(DeviceManager* devManager, WindowUI* editorUI);

        virtual ~ImGuiRenderer() override;

        bool Init();

        ImFont* LoadFont(const std::filesystem::path& fontFile, float fontSize);

        virtual void Render(nvrhi::IFramebuffer* framebuffer, float deltaTile) override;
        virtual void BackBufferResizing() override;

    protected:
        void BeginFullScreenWindow();
        void DrawScreenCenteredText(const char* text);
        void EndFullScreenWindow();

    protected:
        std::unique_ptr<ImGuiRHI> mImGuiRHI;

        WindowUI* mEditorUI;

        std::array<bool, 3> mouseDown = { false };
        std::array<bool, GLFW_KEY_LAST + 1> keyDown = { false };

    protected:
        void KeyboardUpdate(int key, int scancode, int action, int mods);

        void KeyboardCharInput(unsigned int unicode, int mods);

        void MousePosUpdate(double xpos, double ypos);

        void MouseScrollUpdate(double xoffset, double yoffset);

        void MouseButtonUpdate(int button, int action, int mods);
    };

    
}

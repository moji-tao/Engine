#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "EngineRuntime/include/Function/UI/ImGuiRenderer.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Engine
{
    ImGuiRenderer::ImGuiRenderer(DeviceManager* devManager, WindowUI* editorUI)
        : IRenderPass(devManager), mEditorUI(editorUI)
    {
        ImGui::CreateContext();
    }

    ImGuiRenderer::~ImGuiRenderer()
    {
        ImGui::DestroyContext();
    }

    bool ImGuiRenderer::Init()
    {
        // Set up keyboard mapping.
        // ImGui will use those indices to peek into the io.KeyDown[] array
        // that we will update during the application lifetime.
        ImGuiIO& io = ImGui::GetIO();
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A] = 'A';
        io.KeyMap[ImGuiKey_C] = 'C';
        io.KeyMap[ImGuiKey_V] = 'V';
        io.KeyMap[ImGuiKey_X] = 'X';
        io.KeyMap[ImGuiKey_Y] = 'Y';
        io.KeyMap[ImGuiKey_Z] = 'Z';

        WindowSystem::GetInstance()->RegisterDownOrReleaseKeyCallback(std::bind(&ImGuiRenderer::KeyboardUpdate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        WindowSystem::GetInstance()->RegisterCharModsInputCallback(std::bind(&ImGuiRenderer::KeyboardCharInput, this, std::placeholders::_1, std::placeholders::_2));
        WindowSystem::GetInstance()->RegisterMouseMoveCallback(std::bind(&ImGuiRenderer::MousePosUpdate, this, std::placeholders::_1, std::placeholders::_2));
        WindowSystem::GetInstance()->RegisterScrollCallback(std::bind(&ImGuiRenderer::MouseScrollUpdate, this, std::placeholders::_1, std::placeholders::_2));
        WindowSystem::GetInstance()->RegisterMouseKeyInputCallback(std::bind(&ImGuiRenderer::MouseButtonUpdate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        
        mImGuiRHI = std::make_unique<ImGuiRHI>();
        return mImGuiRHI->init(m_DeviceManager);
        
    }

    ImFont* ImGuiRenderer::LoadFont(const std::filesystem::path& fontFile, float fontSize)
    {
        std::shared_ptr<Blob> fontData = EngineFileSystem::GetInstance()->ReadFile(fontFile);

        assert(fontData != nullptr);

        ImGuiIO& io = ImGui::GetIO();
        
        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        ImFont* imFont = io.Fonts->AddFontFromMemoryTTF(
            (void*)fontData->GetData(), (int)fontData->GetSize(), fontSize, &fontConfig, io.Fonts->GetGlyphRangesChineseFull());

        return imFont;
    }

    void ImGuiRenderer::Render(nvrhi::IFramebuffer* framebuffer, float deltaTile)
    {
        if (!mImGuiRHI) 
            return;

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)WindowSystem::GetInstance()->GetWindowWidth(), (float)WindowSystem::GetInstance()->GetWindowHeight());
        io.DisplayFramebufferScale.x = 1;
        io.DisplayFramebufferScale.y = 1;

        mImGuiRHI->beginFrame(deltaTile);

        mEditorUI->PreRender();
        
        ImGui::Render();

        mImGuiRHI->render(framebuffer);

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        for (size_t i = 0; i < mouseDown.size(); i++)
        {
            if (io.MouseDown[i] == true && mouseDown[i] == false)
            {
                io.MouseDown[i] = false;
            }
        }

        // reconcile key states
        for (size_t i = 0; i < keyDown.size(); i++)
        {
            if (io.KeysDown[i] == true && keyDown[i] == false)
            {
                io.KeysDown[i] = false;
            }
        }

    }

    void ImGuiRenderer::BackBufferResizing()
    {
        if (mImGuiRHI) 
            mImGuiRHI->backbufferResizing();
    }

    void ImGuiRenderer::BeginFullScreenWindow()
    {
        int width, height;
        width = WindowSystem::GetInstance()->GetWindowWidth();
        height = WindowSystem::GetInstance()->GetWindowHeight();
        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(float(width), float(height)), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::SetNextWindowBgAlpha(0.f);
        ImGui::Begin(" ", 0, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
    }

    void ImGuiRenderer::DrawScreenCenteredText(const char* text)
    {
        int width, height;
        width = WindowSystem::GetInstance()->GetWindowWidth();
        height = WindowSystem::GetInstance()->GetWindowHeight();
        ImVec2 textSize = ImGui::CalcTextSize(text);
        ImGui::SetCursorPosX((float(width) - textSize.x) * 0.5f);
        ImGui::SetCursorPosY((float(height) - textSize.y) * 0.5f);
        ImGui::TextUnformatted(text);
    }

    void ImGuiRenderer::EndFullScreenWindow()
    {
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void ImGuiRenderer::KeyboardUpdate(int key, int scancode, int action, int mods)
    {
        auto& io = ImGui::GetIO();

        bool keyIsDown;
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            keyIsDown = true;
        }
        else {
            keyIsDown = false;
        }

        // update our internal state tracking for this key button
        keyDown[key] = keyIsDown;

        if (keyIsDown)
        {
            // if the key was pressed, update ImGui immediately
            io.KeysDown[key] = true;
        }
        else {
            // for key up events, ImGui state is only updated after the next frame
            // this ensures that short keypresses are not missed
        }
    }

    void ImGuiRenderer::KeyboardCharInput(unsigned int unicode, int mods)
    {
        auto& io = ImGui::GetIO();

        io.AddInputCharacter(unicode);
    }

    void ImGuiRenderer::MousePosUpdate(double xpos, double ypos)
    {
        auto& io = ImGui::GetIO();
        io.MousePos.x = float(xpos);
        io.MousePos.y = float(ypos);
    }

    void ImGuiRenderer::MouseScrollUpdate(double xoffset, double yoffset)
    {
        auto& io = ImGui::GetIO();
        io.MouseWheel += float(yoffset);
    }

    void ImGuiRenderer::MouseButtonUpdate(int button, int action, int mods)
    {
        auto& io = ImGui::GetIO();

        bool buttonIsDown;
        int buttonIndex;

        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            buttonIsDown = true;
        }
        else {
            buttonIsDown = false;
        }

        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            buttonIndex = 0;
            break;

        case GLFW_MOUSE_BUTTON_RIGHT:
            buttonIndex = 1;
            break;

        case GLFW_MOUSE_BUTTON_MIDDLE:
            buttonIndex = 2;
            break;
        }

        // update our internal state tracking for this mouse button
        mouseDown[buttonIndex] = buttonIsDown;

        if (buttonIsDown)
        {
            // update ImGui state immediately
            io.MouseDown[buttonIndex] = true;
        }
        else {
            // for mouse up events, ImGui state is only updated after the next frame
            // this ensures that short clicks are not missed
        }
    }

}

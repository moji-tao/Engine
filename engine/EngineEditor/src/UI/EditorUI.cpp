#include "EngineEditor/include/UI/EditorUI.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "Function/UI/ImGuiRenderer.h"

#include "Function/Window/WindowSystem.h"

#include "EngineEditor/include/UI/EditorUIProjectPass.h"
#include "EngineEditor/include/UI/EditorUIConsolePass.h"
#include "EngineEditor/include/UI/EditorUIHierarchyPass.h"
#include "EngineEditor/include/UI/EditorUIInspectorPass.h"
#include "EngineEditor/include/UI/EditorUIScenePass.h"
#include <d3d12.h>
namespace Editor
{
	EditorUI::EditorUI()
	{
		mEditorUIRender = std::make_shared<Engine::ImGuiRenderer>(Engine::RenderSystem::GetInstance()->GetRenderDeviceManager(), this);

		Engine::RenderSystem::GetInstance()->InitializeUIRenderBackend(this, mEditorUIRender.get());

		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		mFontsSimhei = mEditorUIRender->LoadFont("Resource/simhei.ttf", 20.0f);

		mProjectUI = std::make_shared<EditorUIProjectPass>();
		mConsoleUI = std::make_shared<EditorUIConsolePass>();
		mHierarchyUI = std::make_shared<EditorUIHierarchyPass>();
		mSceneUI = std::make_shared<EditorUIScenePass>();
		mInspectorUI = std::make_shared<EditorUIInspectorPass>();

	}

	EditorUI::~EditorUI()
	{
		mConsoleUI = nullptr;
		mProjectUI = nullptr;
		mEditorUIRender = nullptr;
	}

	void EditorUI::Initialize(const Engine::WindowUIInitInfo& info)
	{
		mEditorUIRender->Init();
		LOG_INFO("编辑器UI初始化完成");
	}

	void EditorUI::PreRender()
	{
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		else
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
			ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
			ImGui::SameLine(0.0f, 0.0f);
			if (ImGui::SmallButton("click here"))
				io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("文件"))
			{
				ImGui::MenuItem("新建场景");
				ImGui::MenuItem("打开场景");
				ImGui::Separator();
				if(ImGui::MenuItem("退出"))
				{
					Engine::WindowSystem::GetInstance()->CloseWindow();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("编辑"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("窗口"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("帮助"))
			{
				ImGui::MenuItem("关于");
				ImGui::EndMenu();
			}


			ImGui::EndMenuBar();
		}

		ImGui::End();

		mProjectUI->ShowUI();
		mConsoleUI->ShowUI();
		mHierarchyUI->ShowUI();
		mSceneUI->ShowUI();
		mInspectorUI->ShowUI();

		static bool my_tool_active = false;
		static float colors[4] = { 0,0,0,0 };

		ImGui::Begin("测试1", &my_tool_active, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
				if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
				if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// 编辑颜色 (stored as ~4 floats)
		ImGui::ColorEdit4("Color", colors);

		// Plot some values
		const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
		ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

		// 在滚动区域中显示内容
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");

		static int k = 9999;



		ImGui::BeginChild("Scrolling");
		for (int n = 0; n < 50; n++)
			ImGui::Text("%04d: Some text %d", n, k);
		ImGui::EndChild();

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* dddd = ImGui::AcceptDragDropPayload("Drag Index Button");
			if (dddd)
			{
				k = *(int*)dddd->Data;
			}
			ImGui::Text(std::to_string(k).c_str());

			ImGui::EndDragDropTarget();
		}


		ImGui::End();

		ImGui::Begin("测试2");

		for (int i = 0; i < 5; ++i)
		{
			ImGui::Button(std::to_string(i).c_str());
			if (i != 4)
				ImGui::SameLine();
			
			if (ImGui::BeginDragDropSource())
			{
				ImGui::Text((std::string("Drag i: ") + std::to_string(i)).c_str());

				ImGui::SetDragDropPayload("Drag Index Button", &i, sizeof(int));
				ImGui::EndDragDropSource();
			}
		}

		ImGui::End();
	}

}

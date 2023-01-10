#include "EngineEditor/include/UI/EditorUI.h"
#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineEditor/include/UI/EditorUIProjectPass.h"
#include "EngineEditor/include/UI/EditorUIConsolePass.h"
#include "EngineEditor/include/UI/EditorUIHierarchyPass.h"
#include "EngineEditor/include/UI/EditorUIInspectorPass.h"
#include "EngineEditor/include/UI/EditorUIScenePass.h"
#include "EngineEditor/include/EditorTools/ModelLoader.h"
#include "EngineEditor/include/UI/ImGuiExtensions/imgui_notify.h"

namespace Editor
{
	EditorUI::EditorUI(EngineEditor* editor)
		:mEditor(editor)
	{
		mDevice = Engine::RenderSystem::GetInstance()->InitializeUIRenderBackend(this);
		
		LoadFonts(editor->Config.editorResourceConfig.editorFontPath);

		mProjectUI = std::make_shared<EditorUIProjectPass>();
		mProjectUI->Initialize(mDevice, editor);
		mConsoleUI = std::make_shared<EditorUIConsolePass>();
		mConsoleUI->Initialize(mDevice, editor);
		mHierarchyUI = std::make_shared<EditorUIHierarchyPass>();
		mHierarchyUI->Initialize(mDevice, editor);
		mSceneUI = std::make_shared<EditorUIScenePass>();
		mSceneUI->Initialize(mDevice, editor);
		mInspectorUI = std::make_shared<EditorUIInspectorPass>();
		mInspectorUI->Initialize(mDevice, editor);
		
		Engine::RenderSystem::GetInstance()->InitializeUIRenderBackendEnd();

		ConfigStyleColor();
	}

	EditorUI::~EditorUI()
	{
		mInspectorUI = nullptr;
		mSceneUI = nullptr;
		mHierarchyUI = nullptr;
		mConsoleUI = nullptr;
		mProjectUI = nullptr;

		Engine::RenderSystem::GetInstance()->FinalizeUIRenderBackend();
	}

	void EditorUI::Initialize(const Engine::WindowUIInitInfo& info)
	{
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
				if (ImGui::MenuItem("退出"))
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
		
		mConsoleUI->ShowUI();
		mProjectUI->ShowUI();
		mHierarchyUI->ShowUI();
		mSceneUI->ShowUI();
		mInspectorUI->ShowUI();

		// 测试1
		{
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

		}

		// 测试2
		{
			ImGui::Begin("测试2");

			for (int i = 0; i < 5; ++i)
			{
				ImGui::Button(std::to_string(i).c_str());


				if (ImGui::BeginDragDropSource())
				{
					ImGui::Text((std::string("Drag i: ") + std::to_string(i)).c_str());

					ImGui::SetDragDropPayload("Drag Index Button", &i, sizeof(int));
					ImGui::EndDragDropSource();
				}

				if (i != 4)
					ImGui::SameLine();
			}

			ImGui::End();

		}


		// 自定义控件
		{
			ImGui::Begin("自定义控件");

			static const char* item_names[] = { "一", "二", "三", "四", "五" };
			for (int n = 0; n < 5; n++)
			{
				const char* item = item_names[n];
				ImGui::Selectable(item);

				if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
				{
					int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
					if (n_next >= 0 && n_next < 5)
					{
						item_names[n] = item_names[n_next];
						item_names[n_next] = item;
						ImGui::ResetMouseDragDelta();
					}
				}
			}

			float xx = ImGui::GetMouseDragDelta(0).x;
			float yy = ImGui::GetMouseDragDelta(0).y;

			if (ImGui::IsKeyReleased(ImGuiKey_M))
			{
				ImGui::ResetMouseDragDelta();
			}

			ImGui::Text(std::to_string(xx).c_str());
			ImGui::Text(std::to_string(yy).c_str());

			ImGui::End();

		}

		// Demo
		{
			static bool demo = true;
			ImGui::ShowDemoWindow(&demo);
		}

		{
			ImGui::Begin("文件窗体");

			


			ImGui::End();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
		ImGui::RenderNotifications();
		ImGui::PopStyleVar(1); // Don't forget to Pop()
		ImGui::PopStyleColor(1);

	}

	void EditorUI::LoadFonts(const std::filesystem::path& ph)
	{
		std::shared_ptr<Engine::Blob> font = Engine::EngineFileSystem::GetInstance()->ReadFile(ph);
		ImGuiIO& io = ImGui::GetIO();

		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		io.Fonts->AddFontFromMemoryTTF((void*)font->GetData(), (int)font->GetSize(), 20.0f, &fontConfig, io.Fonts->GetGlyphRangesChineseFull());
	}

	void EditorUI::ConfigStyleColor()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		
	}
}

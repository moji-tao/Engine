#include <imgui/imgui_internal.h>
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
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Core/Math/Math.h"

using Engine::Math;

namespace Editor
{
	EditorUI::EditorUI(EngineEditor* editor)
		:mEditor(editor)
	{
		mDevice = Engine::RenderSystem::GetInstance()->InitializeUIRenderBackend(this);
		
		LoadFonts(editor->Config.editorResourceConfig.editorFontPath);

		mUIMessage = new EditorUIMessage;

		mProjectUI = new EditorUIProjectPass();
		mProjectUI->Initialize(mUIMessage, mDevice, editor);
		mConsoleUI = new EditorUIConsolePass();
		mConsoleUI->Initialize(mUIMessage, mDevice, editor);
		mHierarchyUI = new EditorUIHierarchyPass();
		mHierarchyUI->Initialize(mUIMessage, mDevice, editor);
		mSceneUI = new EditorUIScenePass();
		mSceneUI->Initialize(mUIMessage, mDevice, editor);
		mInspectorUI = new EditorUIInspectorPass();
		mInspectorUI->Initialize(mUIMessage, mDevice, editor);
		
		Engine::RenderSystem::GetInstance()->InitializeUIRenderBackendEnd();

		ConfigStyleColor();
	}

	EditorUI::~EditorUI()
	{
		delete mInspectorUI;
		delete mSceneUI;
		delete mHierarchyUI;
		delete mConsoleUI;
		delete mProjectUI;

		delete mUIMessage;

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
				if (ImGui::MenuItem("保存场景"))
				{
					Engine::WorldManager::GetInstance()->SaveCurrentLevel();
				}
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
			ImGui::Begin("场景摄像机");

			Engine::RenderCamera* editorCamera = mEditor->GetEditorCamera();
			Engine::Vector3 position = editorCamera->GetPosition();
			ImGui::Text("渲染摄像机参数");
			ImGui::Text("Position (%f, %f, %f)", position.x, position.y, position.z);
			Engine::Vector3 forward = editorCamera->Forward();
			ImGui::Text("Forward  (%f, %f, %f)", forward.x, forward.y, forward.z);
			Engine::Vector3 right = editorCamera->Right();
			ImGui::Text("Right    (%f, %f, %f)", right.x, right.y, right.z);
			Engine::Vector3 up = editorCamera->Up();
			ImGui::Text("Up       (%f, %f, %f)", up.x, up.y, up.z);

			/*
			Engine::Radian angle;
			Engine::Vector3 axis;
			editorCamera->GetRotation().GetAngleAxis(angle, axis);
			ImGui::Text("Axis-Angle (%f, %f, %f), %f", axis.x, axis.y, axis.z, angle.ValueDegrees());
			*/

			ImGui::End();
		}

		{
			ImGui::Begin("摄像机资源");

			const Engine::CameraPassConstants* camera_pass = Engine::RenderSystem::GetInstance()->GetRenderResource()->GetCameraPass();

			/*
				Matrix4x4 View = Matrix4x4::IDENTITY;
				Matrix4x4 InvView = Matrix4x4::IDENTITY;
				Matrix4x4 Proj = Matrix4x4::IDENTITY;
				Matrix4x4 InvProj = Matrix4x4::IDENTITY;
				Matrix4x4 ViewProj = Matrix4x4::IDENTITY;
				Matrix4x4 InvViewProj = Matrix4x4::IDENTITY;
				Vector3 EyePosW = Vector3::ZERO;
				float cbPerObjectPad1 = 0.0f;
				Vector2 RenderTargetSize = Vector2::ZERO;
				Vector2 InvRenderTargetSize = Vector2::ZERO;
				float NearZ = 0.0f;
				float FarZ = 0.0f;
				float TotalTime = 0.0f;
				float DeltaTime = 0.0f;

				Vector4 FogColor = Vector4::ZERO;
				float FogStart = 0.0f;
				float FogRange = 0.0f;
				Vector2 cbPassPad2 = Vector2::ZERO;
			 */

			ImGui::Text("Matrix4x4 View");
			ImGui::Text("\t[%f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f]\n",
				camera_pass->View[0][0], camera_pass->View[0][1], camera_pass->View[0][2], camera_pass->View[0][3],
				camera_pass->View[1][0], camera_pass->View[1][1], camera_pass->View[1][2], camera_pass->View[1][3],
				camera_pass->View[2][0], camera_pass->View[2][1], camera_pass->View[2][2], camera_pass->View[2][3],
				camera_pass->View[3][0], camera_pass->View[3][1], camera_pass->View[3][2], camera_pass->View[3][3]);

			ImGui::Text("Matrix4x4 InvView");
			ImGui::Text("\t[%f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f]\n",
				camera_pass->InvView[0][0], camera_pass->InvView[0][1], camera_pass->InvView[0][2], camera_pass->InvView[0][3],
				camera_pass->InvView[1][0], camera_pass->InvView[1][1], camera_pass->InvView[1][2], camera_pass->InvView[1][3],
				camera_pass->InvView[2][0], camera_pass->InvView[2][1], camera_pass->InvView[2][2], camera_pass->InvView[2][3],
				camera_pass->InvView[3][0], camera_pass->InvView[3][1], camera_pass->InvView[3][2], camera_pass->InvView[3][3]);

			ImGui::Text("Matrix4x4 Proj");
			ImGui::Text("\t[%f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f]\n",
				camera_pass->Proj[0][0], camera_pass->Proj[0][1], camera_pass->Proj[0][2], camera_pass->Proj[0][3],
				camera_pass->Proj[1][0], camera_pass->Proj[1][1], camera_pass->Proj[1][2], camera_pass->Proj[1][3],
				camera_pass->Proj[2][0], camera_pass->Proj[2][1], camera_pass->Proj[2][2], camera_pass->Proj[2][3],
				camera_pass->Proj[3][0], camera_pass->Proj[3][1], camera_pass->Proj[3][2], camera_pass->Proj[3][3]);

			ImGui::Text("Matrix4x4 InvProj");
			ImGui::Text("\t[%f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f]\n",
				camera_pass->InvProj[0][0], camera_pass->InvProj[0][1], camera_pass->InvProj[0][2], camera_pass->InvProj[0][3],
				camera_pass->InvProj[1][0], camera_pass->InvProj[1][1], camera_pass->InvProj[1][2], camera_pass->InvProj[1][3],
				camera_pass->InvProj[2][0], camera_pass->InvProj[2][1], camera_pass->InvProj[2][2], camera_pass->InvProj[2][3],
				camera_pass->InvProj[3][0], camera_pass->InvProj[3][1], camera_pass->InvProj[3][2], camera_pass->InvProj[3][3]);

			ImGui::Text("Matrix4x4 ViewProj");
			ImGui::Text("\t[%f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f]\n",
				camera_pass->ViewProj[0][0], camera_pass->ViewProj[0][1], camera_pass->ViewProj[0][2], camera_pass->ViewProj[0][3],
				camera_pass->ViewProj[1][0], camera_pass->ViewProj[1][1], camera_pass->ViewProj[1][2], camera_pass->ViewProj[1][3],
				camera_pass->ViewProj[2][0], camera_pass->ViewProj[2][1], camera_pass->ViewProj[2][2], camera_pass->ViewProj[2][3],
				camera_pass->ViewProj[3][0], camera_pass->ViewProj[3][1], camera_pass->ViewProj[3][2], camera_pass->ViewProj[3][3]);

			ImGui::Text("Matrix4x4 InvViewProj");
			ImGui::Text("\t[%f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f\n"
				"\t %f, %f, %f, %f]\n",
				camera_pass->InvViewProj[0][0], camera_pass->InvViewProj[0][1], camera_pass->InvViewProj[0][2], camera_pass->InvViewProj[0][3],
				camera_pass->InvViewProj[1][0], camera_pass->InvViewProj[1][1], camera_pass->InvViewProj[1][2], camera_pass->InvViewProj[1][3],
				camera_pass->InvViewProj[2][0], camera_pass->InvViewProj[2][1], camera_pass->InvViewProj[2][2], camera_pass->InvViewProj[2][3],
				camera_pass->InvViewProj[3][0], camera_pass->InvViewProj[3][1], camera_pass->InvViewProj[3][2], camera_pass->InvViewProj[3][3]);

			ImGui::Text("Vector3 EyePosW");
			ImGui::Text("\t (%f, %f, %f)\n", camera_pass->EyePosW[0], camera_pass->EyePosW[1], camera_pass->EyePosW[2]);
			ImGui::Text("Vector2 RenderTargetSize");
			ImGui::Text("\t (%f, %f)\n", camera_pass->RenderTargetSize[0], camera_pass->RenderTargetSize[1]);
			ImGui::Text("Vector2 InvRenderTargetSize");
			ImGui::Text("\t (%f, %f)\n", camera_pass->InvRenderTargetSize[0], camera_pass->InvRenderTargetSize[1]);
			ImGui::Text("float NearZ");
			ImGui::Text("\t %f\n", camera_pass->NearZ);
			ImGui::Text("float FarZ");
			ImGui::Text("\t %f\n", camera_pass->FarZ);
			ImGui::Text("float TotalTime");
			ImGui::Text("\t %f\n", camera_pass->TotalTime);
			ImGui::Text("float DeltaTime");
			ImGui::Text("\t %f\n", camera_pass->DeltaTime);

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

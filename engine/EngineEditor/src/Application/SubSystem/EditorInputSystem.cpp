#include "EngineEditor/include/Application/SubSystem/EditorInputSystem.h"
#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#define k_complement_control_command 0xFFFFFFFF


namespace Editor
{
	EditorInputSystem::EditorInputSystem(EngineEditor* editor)
		:mEditor(editor)
	{
		RegisterInput();
	}

	EditorInputSystem::~EditorInputSystem()
	{ }

	void EditorInputSystem::ResizeSceneWindow(int width, int height)
	{
		mEngineSceneWindowSize = Engine::Vector2(width, height);
		mEditor->GetEditorCamera()->SetRenderSize(width, height);
		Engine::RenderSystem::GetInstance()->RenderViewResize(width, height);
	}

	void EditorInputSystem::Tick()
	{
		ProcessEditorCommand();
	}

	void EditorInputSystem::RegisterInput()
	{
		Engine::WindowSystem::GetInstance()->RegisterDownOrReleaseKeyCallback(std::bind(&EditorInputSystem::OnKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		Engine::WindowSystem::GetInstance()->RegisterMouseMoveCallback(std::bind(&EditorInputSystem::OnCursorPos, this, std::placeholders::_1, std::placeholders::_2));
		Engine::WindowSystem::GetInstance()->RegisterMouseEnterWindowCallback(std::bind(&EditorInputSystem::OnCursorEnter, this, std::placeholders::_1));
		Engine::WindowSystem::GetInstance()->RegisterScrollCallback(std::bind(&EditorInputSystem::OnScroll, this, std::placeholders::_1, std::placeholders::_2));
		Engine::WindowSystem::GetInstance()->RegisterMouseKeyInputCallback(std::bind(&EditorInputSystem::OnMouseButtonClicked, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	void EditorInputSystem::ProcessEditorCommand()
	{
		Engine::RenderCamera* editorCamera = mEditor->GetEditorCamera();
		//Engine::Quaternion cameraRotate = editorCamera->GetRotation().Inverse();
		Engine::Vector3 cameraRelativePos(0, 0, 0);

		if ((unsigned int)EditorCommand::camera_left & mEditorCommand)
		{
			editorCamera->MoveRight(-mCameraSpeed);
			//cameraRelativePos += cameraRotate * Engine::Vector3{ -mCameraSpeed, 0, 0 };
		}

		if ((unsigned int)EditorCommand::camera_back & mEditorCommand)
		{
			editorCamera->MoveForward(-mCameraSpeed);
			//cameraRelativePos += cameraRotate * Engine::Vector3{ 0, -mCameraSpeed, 0 };
		}

		if ((unsigned int)EditorCommand::camera_forward & mEditorCommand)
		{
			editorCamera->MoveForward(mCameraSpeed);
			//cameraRelativePos += cameraRotate * Engine::Vector3{ 0, mCameraSpeed, 0 };
		}

		if ((unsigned int)EditorCommand::camera_right & mEditorCommand)
		{
			editorCamera->MoveRight(mCameraSpeed);
			//cameraRelativePos += cameraRotate * Engine::Vector3{ mCameraSpeed, 0, 0 };
		}

		if ((unsigned int)EditorCommand::camera_up & mEditorCommand)
		{
			editorCamera->MoveUp(mCameraSpeed);
			//cameraRelativePos += cameraRotate * Engine::Vector3{ 0, 0, mCameraSpeed };
		}

		if ((unsigned int)EditorCommand::camera_down & mEditorCommand)
		{
			editorCamera->MoveUp(-mCameraSpeed);
			//cameraRelativePos += cameraRotate * Engine::Vector3{ 0, 0, -mCameraSpeed };
		}

		/*
		if ((unsigned int)EditorCommand::delete_object & mEditorCommand)
		{
			//cameraRelativePos += cameraRotate * Engine::Vector3{ 0, mCameraSpeed, 0 };
		}
		*/

		//editorCamera->Move(cameraRelativePos);
	}

	void EditorInputSystem::OnKey(int key, int scancode, int action, int mods)
	{
		if (mEditorCameraMoveMode)
		{
			if (action == GLFW_PRESS)
			{
				switch (key)
				{
				case GLFW_KEY_A:
					mEditorCommand |= (unsigned int)EditorCommand::camera_left;
					break;
				case GLFW_KEY_S:
					mEditorCommand |= (unsigned int)EditorCommand::camera_back;
					break;
				case GLFW_KEY_W:
					mEditorCommand |= (unsigned int)EditorCommand::camera_forward;
					break;
				case GLFW_KEY_D:
					mEditorCommand |= (unsigned int)EditorCommand::camera_right;
					break;
				case GLFW_KEY_Q:
					mEditorCommand |= (unsigned int)EditorCommand::camera_down;
					break;
				case GLFW_KEY_E:
					mEditorCommand |= (unsigned int)EditorCommand::camera_up;
					break;
				case GLFW_KEY_T:
					mEditorCommand |= (unsigned int)EditorCommand::translation_mode;
					break;
				case GLFW_KEY_R:
					mEditorCommand |= (unsigned int)EditorCommand::rotation_mode;
					break;
				case GLFW_KEY_C:
					mEditorCommand |= (unsigned int)EditorCommand::scale_mode;
					break;
				case GLFW_KEY_DELETE:
					mEditorCommand |= (unsigned int)EditorCommand::delete_object;
					break;
				default:
					break;

				}
			}
			else if (action == GLFW_RELEASE)
			{
				switch (key)
				{
				case GLFW_KEY_ESCAPE:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::exit);
					break;
				case GLFW_KEY_A:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_left);
					break;
				case GLFW_KEY_S:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_back);
					break;
				case GLFW_KEY_W:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_forward);
					break;
				case GLFW_KEY_D:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_right);
					break;
				case GLFW_KEY_Q:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_down);
					break;
				case GLFW_KEY_E:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_up);
					break;
				case GLFW_KEY_T:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::translation_mode);
					break;
				case GLFW_KEY_R:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::rotation_mode);
					break;
				case GLFW_KEY_C:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::scale_mode);
					break;
				case GLFW_KEY_DELETE:
					mEditorCommand &= (k_complement_control_command ^ (unsigned int)EditorCommand::delete_object);
					break;
				default:
					break;
				}
			}
		}
		else
		{
			if (action == GLFW_RELEASE)
			{
				if (key == GLFW_KEY_SPACE)
				{
					Engine::RenderSystem::GetInstance()->mEnableTAA = !Engine::RenderSystem::GetInstance()->mEnableTAA;
				}
			}
		}
	}

	void EditorInputSystem::OnCursorPos(double xpos, double ypos)
	{
		float angularVelocity = 180.0f / Engine::Math::Max(mEngineSceneWindowSize.x, mEngineSceneWindowSize.y); // 180 degrees while moving full screen

		if (mMouseX >= 0.0f && mMouseY >= 0.0f)
		{
			if (mEditorCameraMoveMode)
			{
				//LOG_INFO("编辑器摄像机 旋转 {0}, {1}", xpos, ypos);

				Engine::RenderCamera* editorCamera = mEditor->GetEditorCamera();
				editorCamera->Pitch(Engine::Degree(0.25f * (ypos - mMouseY)));
				editorCamera->Yaw(Engine::Degree(0.25f * (xpos - mMouseX)));

				//editorCamera->Rotate(Engine::Vector2(ypos - mMouseY, xpos - mMouseX) * angularVelocity);
			}
		}

		mMouseX = xpos;
		mMouseY = ypos;
	}

	void EditorInputSystem::OnCursorEnter(int entered)
	{
		//LOG_INFO("鼠标进入 {0}", entered);
	}

	void EditorInputSystem::OnScroll(double xoffset, double yoffset)
	{
		//LOG_INFO("鼠标滚轮滑动 {0}, {1}", xoffset, yoffset);
	}

	void EditorInputSystem::OnMouseButtonClicked(int key, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_MOUSE_BUTTON_RIGHT)
			{
				//LOG_INFO("右键按下");
				if (mMouseHoveredScene)
				{
					mEditorCameraMoveMode = true;
				}
			}
			else if (key == GLFW_MOUSE_BUTTON_LEFT)
			{
				//LOG_INFO("左键按下");
			}
		}
		else if (action == GLFW_RELEASE)
		{
			if (key == GLFW_MOUSE_BUTTON_RIGHT)
			{
				//LOG_INFO("右键释放");
				mEditorCameraMoveMode = false;
			}
			else if (key == GLFW_MOUSE_BUTTON_LEFT)
			{
				//LOG_INFO("左键释放");
			}
		}
	}

	void EditorInputSystem::OnWindowClosed()
	{
	}
}

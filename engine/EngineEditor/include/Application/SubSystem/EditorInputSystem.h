#pragma once
#include "EngineRuntime/include/Core/Math/Vector2.h"

namespace Editor
{
	class EngineEditor;

	enum class EditorCommand : unsigned int
	{
		camera_left = 1 << 0,		// A
		camera_back = 1 << 1,		// S
		camera_forward = 1 << 2,	// W
		camera_right = 1 << 3,		// D
		camera_up = 1 << 4,			// E
		camera_down = 1 << 5,		// Q
		translation_mode = 1 << 6,	// T
		rotation_mode = 1 << 7,		// R
		scale_mode = 1 << 8,		// C
		exit = 1 << 9,				// Esc
		delete_object = 1 << 10,	// Delete
	};

	class EditorInputSystem
	{
	public:
		EditorInputSystem(EngineEditor* editor);

		~EditorInputSystem();

	public:
		void ResizeSceneWindow(int width, int height);

		void Tick();

	public:
		bool mMouseHoveredScene;

	private:
		void RegisterInput();

		void ProcessEditorCommand();

	private:
		void OnKey(int key, int scancode, int action, int mods);

		void OnCursorPos(double xpos, double ypos);

		void OnCursorEnter(int entered);

		void OnScroll(double xoffset, double yoffset);

		void OnMouseButtonClicked(int key, int action, int mods);

		void OnWindowClosed();

	private:
		EngineEditor* mEditor = nullptr;

		Engine::Vector2 mEngineSceneWindowSize = { 0.0f,0.0f };

		bool mEditorCameraMoveMode = false;
		unsigned int mEditorCommand = 0;

		float mCameraSpeed = 0.05f;

		double mMouseX = 0;
		double mMouseY = 0;
	};
}

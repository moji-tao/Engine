#pragma once
#include "EngineRuntime/include/Function/Render/RenderCamera.h"

namespace Editor
{
	class EngineEditor;

	class EditorSceneSystem
	{
	public:
		EditorSceneSystem(EngineEditor* editor);

		~EditorSceneSystem();

		void Tick();

		Engine::RenderCamera* GetEditorRenderCamera();

	private:
		EngineEditor* mEditor = nullptr;

		Engine::RenderCamera mEditorCamera;
	};
}

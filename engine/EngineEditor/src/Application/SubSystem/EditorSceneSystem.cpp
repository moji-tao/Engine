#include "EngineEditor/include/Application/SubSystem/EditorSceneSystem.h"
#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"

namespace Editor
{
	EditorSceneSystem::EditorSceneSystem(EngineEditor* editor)
	{
		/*
		mEditorCamera.LookAt(Engine::Vector3(-5.0f, 0.0f, 3.0f),
			Engine::Vector3(-4.0f, 0.0f, 3.0f),
			Engine::Vector3(0.0f, 0.0f, 1.0f));
		*/
		mEditorCamera.LookAt(Engine::Vector3(0.0f, 0.0f, 0.0f),
			Engine::Vector3(0.0f, 0.0f, 1.0f),
			Engine::Vector3(0.0f, 1.0f, 0.0f));

		/*
		mEditorCamera.mZFar = 0.003f;
		mEditorCamera.mZNear = 10000.0f;
		*/
		
		mEditorCamera.mZFar = 1000.0f;
		mEditorCamera.mZNear = 0.001f;
		
		Engine::RenderSystem::GetInstance()->SetEditorRenderCamera(mEditorCamera);
	}

	EditorSceneSystem::~EditorSceneSystem()
	{

	}

	void EditorSceneSystem::Tick()
	{

	}

	Engine::RenderCamera* EditorSceneSystem::GetEditorRenderCamera()
	{
		return &mEditorCamera;
	}
}

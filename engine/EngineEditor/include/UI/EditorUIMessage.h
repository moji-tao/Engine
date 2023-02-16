#pragma once
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"

namespace Editor
{
	class EditorUIMessage
	{
	public:
		void SetInspectorShowActor(Engine::Actor* actor);

		Engine::Actor* GetInspectorShowActor();

		void SetProjectSelectFile(const Engine::GUID& guid);

		Engine::GUID GetProjectSelectFile();

	private:
		Engine::Actor* mInspectorShowActor = nullptr;

		Engine::GUID mProjectSelectFile;
	};
}

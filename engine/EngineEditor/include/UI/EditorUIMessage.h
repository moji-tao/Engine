#pragma once
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"

namespace Editor
{
	class EditorUIMessage
	{
	public:
		void SetInspectorShowActor(Engine::Actor* actor);

		Engine::Actor* GetInspectorShowActor();

	private:
		Engine::Actor* mInspectorShowActor = nullptr;
	};
}

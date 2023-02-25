#pragma once
#include "EngineEditor/include/UI/EditorUIPassBase.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"

namespace Editor
{
	class EditorUIInspectorPass : public EditorUIPassBase
	{
	public:
		EditorUIInspectorPass();

		virtual ~EditorUIInspectorPass() override;

		virtual void Initialize(EditorUIMessage* messageBox, Engine::ImGuiDevice* device, EngineEditor* editor) override;

	public:
		virtual void ShowUI() override;

	private:
		Engine::Actor* mActor = nullptr;
		Engine::Actor* mLastActor = nullptr;
		std::string mActorName;
		bool mRest = true;
		Engine::Vector3 eural;

		char mNameBuffer[1024];

		std::unordered_map<std::string, std::function<void(Engine::Component*)>> mComponentUIMap;

	private:
		//MeshRender
		char mRefMeshName[512];

		// 
		char mRefSkeletonName[512];

		// 
		char mRefAnimationName[512];
	};
}

#pragma once
#include "EngineEditor/include/UI/EditorUIPassBase.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/SkeletonData.h"

namespace Editor
{
	class EditorUIAssetAttributePass : public EditorUIPassBase
	{
	public:
		EditorUIAssetAttributePass();

		virtual ~EditorUIAssetAttributePass() override;

		virtual void Initialize(EditorUIMessage* messageBox, Engine::ImGuiDevice* device, EngineEditor* editor) override;

	public:
		virtual void ShowUI() override;

	private:
		void ShowDragGuidAsset(Engine::GUID& guid, const std::string& name);

		void ShowSkeleton(Engine::Joint* joint);

	private:
		Engine::GUID mShowAssetGuid;

	};
}

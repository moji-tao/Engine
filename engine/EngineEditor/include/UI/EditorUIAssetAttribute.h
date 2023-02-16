#pragma once
#include "EngineEditor/include/UI/EditorUIPassBase.h"

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


	private:
		Engine::GUID mShowAssetGuid;

	};
}

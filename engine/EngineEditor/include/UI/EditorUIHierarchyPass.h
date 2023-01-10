#pragma once
#include "EngineEditor/include/UI/EditorUIPassBase.h"
#include "EngineEditor/include/UI/SubFrame/EditorRightMenu.h"
#include "EngineRuntime/include/Function/Framework/Level/Level.h"

namespace Editor
{
	/*
	struct DragNode {
		std::string name;
		DragNode* parent;
		std::vector<DragNode*> children;
	};
	*/

	class EditorUIHierarchyPass : public EditorUIPassBase
	{
		DECLARE_RTTI;
	public:
		EditorUIHierarchyPass();

		virtual ~EditorUIHierarchyPass() override;

		virtual void Initialize(Engine::ImGuiDevice* device, EngineEditor* editor) override;

	public:
		virtual void ShowUI() override;

	private:
		void DropNode(Engine::Actor* targetNode);

		void ShowNode(Engine::Actor* node, std::string ext);

		void ShowSceneHierarchy(Engine::Level* node);

	public:
		// 层级窗口的右键菜单需要反射的方法
		void Ref_ClipAndPaste();
		void Ref_CopyGameObject();
		void Ref_PasteGameObject();
		void Ref_PasteGameObjectForChild();
		void Ref_GameObjectReName();
		void Ref_GameObjectDelete();
		void Ref_CreateEmptyGameObject();
		void Ref_Create3DCube();

		bool mIsHoveredGameObject = false;

		void ShowPopupWindow();

	private:
		Engine::Actor* mCurrentHoveringGameObject = nullptr;
		//DragNode* mCurrentHoveringGameObject = nullptr;
		Engine::Level* mCurrentScene = nullptr;

		bool mIsPopupRightMenu = false;

		std::shared_ptr<RightMenuUI> mRightMenu;

		bool mLaseDrag = false;

	//重命名
	private:
		bool mIsOpenRenameWindow = false;
		Engine::Actor* mSeleteActor = nullptr;
		char mRenameBuffer[256];

	//删除
	private:
		bool mIsOpenDeleteActorWindow = false;
		std::string mDeleteText;
	};
}

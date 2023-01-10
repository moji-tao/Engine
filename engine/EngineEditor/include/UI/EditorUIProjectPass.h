#pragma once
#include <filesystem>
#include <list>
#include <string>
#include <array>
#include "EngineEditor/include/UI/EditorUIPassBase.h"
#include "EngineEditor/include/Application/AssetFIle.h"
#include "EngineEditor/include/UI/SubFrame/EditorRightMenu.h"

namespace Editor
{
	class EditorUIProjectPass : public EditorUIPassBase
	{
		DECLARE_RTTI;
	public:
		EditorUIProjectPass();

		virtual ~EditorUIProjectPass() override;

		virtual void Initialize(Engine::ImGuiDevice* device, EngineEditor* editor) override;

	public:
		virtual void ShowUI() override;

	private:
		void SetColor();

		void dfsFolder(AssetFile* root, const std::filesystem::path& currentPath);

		void ShowAssetsTree(AssetFile* node);

		//void CreateRightMenu();

		void SwitchFolder(AssetFile* newNode);

		std::filesystem::path GetProjectPath(AssetFile* node);

		std::filesystem::path GetCurrentFolder();

		void ShowPopupWindow();

		bool CheckCurrentFolderFileExists(const std::string& fileName);

	public:
		// 文件管理器的右键菜单需要反射的方法
		void Ref_CreateFolder();
		void Ref_CreateLuaScript();
		void Ref_CreateScene();
		void Ref_ShowFolderInExplorer();
		void Ref_Open();
		void Ref_Delete();
		void Ref_Rename();
		void Ref_ImportAsset();

	public:
		// 文件管理器的右键菜单需要反射的字段
		bool mIsSelectedFile = false;

	private:
		std::shared_ptr<AssetFile> mRoot;
		std::array<std::vector<AssetFile*>, 2> mCurrentOpenFolder;
		unsigned short mCurrentIndex = 0;
		unsigned short mNextIndex = 1;
		bool mIsClickFile = false;
		std::list<std::shared_ptr<AssetFile>>::iterator mMouseCurrentHoveredItem;

		std::vector<char> mSelectButton;
		std::list<std::list<std::shared_ptr<AssetFile>>::iterator> mSelectItem;

		std::shared_ptr<RightMenuUI> mRightMenu;

		Engine::ImGuiTextureInfo mIconFileTexture;
		Engine::ImGuiTextureInfo mIconFolderTexture;
		Engine::ImGuiTextureInfo mIconImageTexture;
		Engine::ImGuiTextureInfo mIconIntegralTexture;

	private:
		ImVec4 mTreeNodeBackColor;
		ImVec4 mTreeNodeHoveredColor;
		ImVec4 mTreeNodeActiveColor;
		ImVec4 mButtonColor;
		ImVec4 mButtonHoveredColor;
		ImVec4 mButtonActiveColor;

	//重命名
	private:
		bool mIsOpenRenameWindow = false;
		AssetFile* mRenameFile = nullptr;
		char mRenameBuffer[256];

	//新建
	private:
		bool mIsOpenCreateWindow = false;
		uint32_t mCreateOrder = 0;
		char mCreateBuffer[256];
		std::string mExtensionName;
		std::unordered_map<uint32_t, std::function<void()>> mCreateOrderMap;

		enum CreateItem : uint32_t
		{
			emNone,
			emCreateFolder,
			emCreateLuaScript,
			emCreateScene,
		};

		void CreateFolder();

		void CreateLuaScript();

		void CreateScene();
	};
}

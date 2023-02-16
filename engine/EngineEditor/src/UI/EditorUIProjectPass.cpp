#include <set>
#include <sstream>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "EngineEditor/include/UI/EditorUIProjectPass.h"
#include "EngineEditor/include/EditorTools/AssetTool.h"
#include "EngineEditor/include/Application/SubSystem/AssetsFileSystem.h"
#include "EngineEditor/include/EditorTools/LuaScriptTemplate.h"
#include "EngineEditor/include/UI/ImGuiExtensions/imgui_notify.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Function/Framework/World/WorldManager.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Function/Framework/Level/Level.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/TextData.h"

namespace Editor
{
	IMPLEMENT_RTTI(EditorUIProjectPass, EditorUIPassBase);
	REGISTER_CLASS(EditorUIProjectPass);
	REGISTER_CLASS_METHOD(EditorUIProjectPass, Ref_CreateFolder, void);
	REGISTER_CLASS_METHOD(EditorUIProjectPass, Ref_CreateLuaScript, void);
	REGISTER_CLASS_METHOD(EditorUIProjectPass, Ref_CreateScene, void);
	REGISTER_CLASS_METHOD(EditorUIProjectPass, Ref_CreateMaterial, void);
	REGISTER_CLASS_METHOD(EditorUIProjectPass, Ref_ShowFolderInExplorer, void);
	REGISTER_CLASS_METHOD(EditorUIProjectPass, Ref_Open, void);
	REGISTER_CLASS_METHOD(EditorUIProjectPass, Ref_Delete, void);
	REGISTER_CLASS_METHOD(EditorUIProjectPass, Ref_Rename, void);
	REGISTER_CLASS_METHOD(EditorUIProjectPass, Ref_ImportAsset, void);
	REGISTER_CLASS_FIELD(EditorUIProjectPass, mIsSelectedFile, bool);

	EditorUIProjectPass::EditorUIProjectPass()
		:EditorUIPassBase()
	{
		mCreateOrderMap[CreateItem::emCreateFolder] = [this] {CreateFolder(); };
		mCreateOrderMap[CreateItem::emCreateLuaScript] = [this] {CreateLuaScript(); };
		mCreateOrderMap[CreateItem::emCreateScene] = [this] {CreateScene(); };
		mCreateOrderMap[CreateItem::emCreateMaterial] = [this] {CreateMaterial(); };
	}

	EditorUIProjectPass::~EditorUIProjectPass()
	{ }

	void EditorUIProjectPass::Initialize(EditorUIMessage* messageBox, Engine::ImGuiDevice* device, EngineEditor* editor)
	{
		EditorUIPassBase::Initialize(messageBox, device, editor);

		SetColor();

		mRoot = editor->GetFileSystem()->GetRootFile();

		mCurrentOpenFolder[0].push_back(mRoot);
		mCurrentOpenFolder[1].push_back(mRoot);

		mIconFileTexture = mDevice->LoadTexture(mEditor->Config.editorResourceConfig.editorFileIcon);
		mIconFolderTexture = mDevice->LoadTexture(mEditor->Config.editorResourceConfig.editorFolderIcon);
		mIconImageTexture = mDevice->LoadTexture(mEditor->Config.editorResourceConfig.editorImageIcon);
		mIconIntegralTexture = mDevice->LoadTexture(mEditor->Config.editorResourceConfig.editorIntegralIcon);

		mRightMenu = EditorRightMenu::CreateRightMenu(Engine::EngineFileSystem::GetInstance()->GetActualPath("UI/RightMenuInProject.xml"), "EditorUIProjectPassRightMenu");

		Engine::WindowSystem::GetInstance()->RegisterFilesDropCallback([this](int fileCount, const char** filePath)
			{
				for (int i = 0; i < fileCount; ++i)
				{
					LOG_INFO("有文件拖入 {0}", filePath[i]);
					std::filesystem::path ph(filePath[i]);
					AssetTool::LoadAsset(ph, mCurrentOpenFolder[mCurrentIndex].back());
				}
			});
	}

	void EditorUIProjectPass::ShowUI()
	{
		if (!mIsOpen)
			return;

		mCurrentOpenFolder[mCurrentIndex] = mCurrentOpenFolder[mNextIndex];
		std::swap(mCurrentIndex, mNextIndex);
		mIsClickFile = false;

		AssetFile* currentFolder = mCurrentOpenFolder[mCurrentIndex].back();

		mMouseCurrentHoveredItem = currentFolder->next.end();

		ImGui::Begin("项目", &mIsOpen, ImGuiWindowFlags_NoCollapse);

		ImVec2 windowSize = ImGui::GetWindowSize();
		windowSize.x = std::max(500.0f, windowSize.x);
		windowSize.y = std::max(200.0f, windowSize.y);
		ImGui::SetWindowSize(windowSize);
		windowSize.y -= ImGui::GetCurrentWindow()->TitleBarHeight();
		float size = 100.0f;
		
		// 左侧树状文件夹
		ImGui::BeginChild("AssetsLeft", ImVec2(windowSize.x * 0.2f, windowSize.y), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::PushStyleColor(ImGuiCol_Header, mTreeNodeBackColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, mTreeNodeHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, mTreeNodeActiveColor);
		ShowAssetsTree(mRoot);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::EndChild();

		ImGui::SameLine();

		// 右侧管理器视图
		ImGui::BeginChild("AssetsRight", ImVec2(windowSize.x * 0.8f, windowSize.y), false, ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::PushStyleColor(ImGuiCol_Button, mButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, mButtonHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, mButtonActiveColor);
		if (ImGui::Button((mCurrentOpenFolder[mCurrentIndex].front()->mName + "##Button_0").c_str()))
		{
			SwitchFolder(mCurrentOpenFolder[mCurrentIndex].front());
		}
		for (unsigned i = 1; i < mCurrentOpenFolder[mCurrentIndex].size(); ++i)
		{
			AssetFile* node = mCurrentOpenFolder[mCurrentIndex][i];
			ImGui::SameLine();
			ImGui::Text(">");
			ImGui::SameLine();
			if (ImGui::Button((node->mName + "##Button_" + std::to_string(i)).c_str()))
			{
				SwitchFolder(node);
			}
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		unsigned fileCount = (unsigned)currentFolder->next.size();
		std::list<AssetFile*>& folderChilds = currentFolder->next;

		if (ImGui::BeginTable("AssetsSplit", (int)(windowSize.x * 0.8f / size)))
		{
			mSelectButton.resize(fileCount);

			unsigned c = 0;
			for (auto it = folderChilds.begin(); it != folderChilds.end(); ++it)
			{
				ImGui::TableNextColumn();
				AssetFile* file = *it;

				ImVec4 hoveredColor;
				ImVec4 selectedColor(0.5f, 0.5f, 0.5f, 1.0f);

				if (mSelectButton[c])
				{
					hoveredColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				}
				else
				{
					hoveredColor = ImVec4(0.5f, 0.5f, 0.5f, 0.0f);
				}

				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, hoveredColor);
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, selectedColor);
				ImGui::PushStyleColor(ImGuiCol_Header, selectedColor);

				std::string title = "##title" + file->mName;
				if (ImGui::Selectable(title.c_str(), mSelectButton[c], ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0, size)))
				{
					mIsClickFile = true;
					if (!ImGui::GetIO().KeyCtrl)
					{
						memset(mSelectButton.data(), 0x00, mSelectButton.size());
						mSelectItem.clear();
					}
					mSelectButton[c] ^= 1;
					if (mSelectButton[c])
					{
						mSelectItem.push_back(it);
					}
					else
					{
						for (auto selectItemit = mSelectItem.begin(); selectItemit != mSelectItem.end(); ++selectItemit)
						{
							if (**selectItemit == file)
							{
								mSelectItem.erase(selectItemit);
								break;
							}
						}
					}
					if (ImGui::IsMouseDoubleClicked(0))
					{
						if (file->mIsFolder)
						{
							SwitchFolder(file);
							ImGui::PopStyleColor();
							ImGui::PopStyleColor();
							ImGui::PopStyleColor();
							break;
						}
						LOG_INFO("double click");
					}
					else
					{
						LOG_INFO("click");
						
						if (!file->mIsFolder)
						{
							mMessageBox->SetProjectSelectFile(file->GetAssetGuid());
						}
					}
				}

				if (ImGui::BeginDragDropSource())
				{
					ImGui::Text(file->mName.c_str());
					
					ImGui::SetDragDropPayload(UIProjectFileDrag, &file, sizeof(file));

					ImGui::EndDragDropSource();
				}

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip(file->mName.c_str());
				}
				ImGui::SameLine();
				if(file->mIsFolder)
				{
					ImGui::Image(mIconFolderTexture.TextureID, ImVec2(size, size));
				}
				else
				{
					ImGui::Image(mIconFileTexture.TextureID, ImVec2(size, size));
				}
				
				if (ImGui::IsItemHovered())
				{
					mMouseCurrentHoveredItem = it;
				}
				ImGui::Text(file->mName.c_str());
				
				++c;
			}

			ImGui::EndTable();
		}

		mIsSelectedFile = !mSelectItem.empty();

		ShowPopupWindow();

		EditorRightMenu::DrawRightMenu(mRightMenu.get(), this);

		if (mMouseCurrentHoveredItem == folderChilds.end() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			mSelectButton.clear();
			mSelectItem.clear();
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			if (mMouseCurrentHoveredItem == folderChilds.end())
			{
				mSelectButton.clear();
				mSelectItem.clear();
			}
			else
			{
				auto it = mSelectItem.begin();
				for (; it != mSelectItem.end(); ++it)
				{
					if ((*it) == mMouseCurrentHoveredItem)
					{
						mSelectItem.erase(it);
						it = mSelectItem.end();
						break;
					}
				}
				if (it == mSelectItem.end())
				{
					mSelectButton.clear();
					mSelectItem.clear();
				}
				mSelectItem.push_back(mMouseCurrentHoveredItem);
			}
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			EditorRightMenu::ShowRightMenu(mRightMenu.get());
		}
		
		ImGui::EndChild();

		ImGui::End();
	}

	void EditorUIProjectPass::ShowAssetsTree(AssetFile* node)
	{
		assert(node != nullptr);
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Selected;
		bool isOpen = ImGui::TreeNodeEx(node->mName.c_str(), flags);
		if(ImGui::IsItemClicked())
		{
			SwitchFolder(node);
		}

		if(isOpen)
		{
			for (AssetFile* nextNode : node->next)
			{
				if (nextNode->mIsFolder)
				{
					ShowAssetsTree(nextNode);
				}
			}

			ImGui::TreePop();
		}
	}

	void EditorUIProjectPass::SwitchFolder(AssetFile* newNode)
	{
		mCurrentOpenFolder[mNextIndex].clear();
		while (newNode)
		{
			mCurrentOpenFolder[mNextIndex].push_back(newNode);
			newNode = newNode->mParent;
		}
		mCurrentOpenFolder[mNextIndex];
		std::reverse(mCurrentOpenFolder[mNextIndex].begin(), mCurrentOpenFolder[mNextIndex].end());
		mSelectButton.clear();
		mSelectItem.clear();
	}

	std::filesystem::path EditorUIProjectPass::GetProjectPath(AssetFile* node)
	{
		std::filesystem::path result;
		while (node)
		{
			result = node->mName / result;
			node = node->mParent;
		}
		return result.parent_path();
	}

	std::filesystem::path EditorUIProjectPass::GetCurrentFolder()
	{
		std::filesystem::path result;
		std::vector<AssetFile*>& currentFolder = mCurrentOpenFolder[mCurrentIndex];
		for (unsigned i = 0; i < currentFolder.size(); ++i)
		{
			result /= currentFolder[i]->mName;
		}

		return result;
	}

	void EditorUIProjectPass::ShowPopupWindow()
	{
#pragma region 重命名
		if (mIsOpenRenameWindow)
		{
			ImGui::OpenPopup("文件重命名");
			mIsOpenRenameWindow = false;
			memset(mRenameBuffer, 0x00, 256);

			if (mRenameFile->mIsFolder)
			{
				memcpy(mRenameBuffer, mRenameFile->mName.c_str(), mRenameFile->mName.size());
			}
			else
			{
				size_t dotIndex = mRenameFile->mName.find_last_of('.');
				memcpy(mRenameBuffer, mRenameFile->mName.c_str(), dotIndex);
			}
		}
		if (ImGui::BeginPopupModal("文件重命名"))
		{
			if (mRenameFile->mIsFolder)
			{
				ImGui::InputText("文件夹", mRenameBuffer, 256);
			}
			else
			{
				size_t dotIndex = mRenameFile->mName.find_last_of('.');

				ImGui::InputText(mRenameFile->mName.c_str() + dotIndex, mRenameBuffer, 256);
			}

			if (ImGui::Button("确定"))
			{
				std::filesystem::path currentFolder = GetCurrentFolder();

				std::string extension;
				if (!mRenameFile->mIsFolder)
				{
					size_t dotIndex = mRenameFile->mName.find_last_of('.');
					extension = mRenameFile->mName.substr(dotIndex);
				}
				std::string name = mRenameBuffer;

				mEditor->GetFileSystem()->AssetFileRename(mRenameFile, name + extension);

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("取消"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

#pragma endregion

#pragma region 创建
		if (mIsOpenCreateWindow)
		{
			ImGui::OpenPopup("新建文件");
			mIsOpenCreateWindow = false;
			memset(mCreateBuffer, 0x00, 256);
		}
		
		if (ImGui::BeginPopupModal("新建文件", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImVec2 window_pos = ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f);
			ImGui::SetWindowPos(window_pos);

			ImGui::InputText(mExtensionName.c_str(), mCreateBuffer, 256);

			if (ImGui::Button("确定"))
			{
				if (strcmp(mCreateBuffer, "") == 0)
				{
					LOG_ERROR("请输入文件名");

					ImGuiToast toast(ImGuiToastType_Error);
					toast.set_title("错误");
					toast.set_content("请输入文件名");
					ImGui::InsertNotification(toast);
				}
				else
				{
					mCreateOrderMap[mCreateOrder]();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("取消"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

#pragma endregion
	}

	bool EditorUIProjectPass::CheckCurrentFolderFileExists(const std::string& fileName)
	{
		std::filesystem::path assetsFolder = GetCurrentFolder() / fileName;

		return Engine::ProjectFileSystem::GetInstance()->FileExists(assetsFolder);
	}

	void EditorUIProjectPass::SetColor()
	{
		mTreeNodeBackColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		mTreeNodeHoveredColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		mTreeNodeActiveColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		mButtonColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		mButtonHoveredColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		mButtonActiveColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	void EditorUIProjectPass::Ref_CreateFolder()
	{
		LOG_INFO("创建文件夹");
		mIsOpenCreateWindow = true;
		mCreateOrder = CreateItem::emCreateFolder;
		mExtensionName = "文件夹";
	}

	void EditorUIProjectPass::CreateFolder()
	{
		std::filesystem::path assetsFolder = GetCurrentFolder() / mCreateBuffer;
		AssetsFileSystem::CreateResult result = mEditor->GetFileSystem()->CreateFolder(mCurrentOpenFolder[mCurrentIndex].back(), mCreateBuffer);

		if (result == AssetsFileSystem::CreateResult::emSuccess)
		{
			ImGuiToast toast(ImGuiToastType_Success);
			toast.set_title("成功");
			toast.set_content("新建文件夹成功");
			ImGui::InsertNotification(toast);
		}
		else if (result == AssetsFileSystem::CreateResult::emFileExists)
		{
			LOG_ERROR("文件夹在当前路径已存在 {0}", mCreateBuffer);

			ImGuiToast toast(ImGuiToastType_Error);
			toast.set_title("错误");
			toast.set_content("文件夹在当前路径已存在 %s", (const char*)mCreateBuffer);
			ImGui::InsertNotification(toast);
		}
		else if(result == AssetsFileSystem::CreateResult::emError)
		{
			LOG_ERROR("未知错误 创建文件失败");

			ImGuiToast toast(ImGuiToastType_Error);
			toast.set_title("未知错误");
			toast.set_content("创建文件失败");
			ImGui::InsertNotification(toast);
		}

		return;
	}

	void EditorUIProjectPass::Ref_CreateLuaScript()
	{
		LOG_INFO("创建Lua脚本");
		mIsOpenCreateWindow = true;
		mCreateOrder = CreateItem::emCreateLuaScript;
		mExtensionName = ".lua";
	}

	void EditorUIProjectPass::CreateLuaScript()
	{
		std::string name(mCreateBuffer);
		name += ".lua";

		Engine::TextMeta meta;
		meta.SetGuid(Engine::GUID::Get());

		AssetsFileSystem::CreateResult result = mEditor->GetFileSystem()->CreateTextFile(mCurrentOpenFolder[mCurrentIndex].back(), name, luaScriptTemplate, meta);

		if (result == AssetsFileSystem::CreateResult::emSuccess)
		{
			LOG_INFO("Lua脚本创建成功");

			ImGuiToast toast(ImGuiToastType_Success);
			toast.set_title("成功");
			toast.set_content("Lua脚本创建成功");

			ImGui::InsertNotification(toast);
		}
		else if(result == AssetsFileSystem::CreateResult::emFileExists)
		{
			LOG_ERROR("Lua脚本在当前路径已存在 {0}", mCreateBuffer);

			ImGuiToast toast(ImGuiToastType_Error);
			toast.set_title("错误");
			toast.set_content("Lua脚本在当前路径已存在 %s", (const char*)mCreateBuffer);

			ImGui::InsertNotification(toast);
		}
		else if(result == AssetsFileSystem::CreateResult::emError)
		{
			ImGuiToast toast(ImGuiToastType_Error);
			toast.set_title("错误");
			toast.set_content("未知错误");

			ImGui::InsertNotification(toast);
		}
	}

	void EditorUIProjectPass::Ref_CreateScene()
	{
		LOG_INFO("创建一个空场景");
		mIsOpenCreateWindow = true;
		mCreateOrder = CreateItem::emCreateScene;
		mExtensionName = ".scene";
	}

	void EditorUIProjectPass::CreateScene()
	{
		std::filesystem::path currentFolder = GetCurrentFolder();
		std::string name(mCreateBuffer);
		name += ".scene";

		AssetsFileSystem* fileSystem = mEditor->GetFileSystem();

		Engine::SerializerDataFrame frame = Engine::WorldManager::GetInstance()->SpanEmptyScene(name);
		Engine::SceneMeta meta;
		meta.SetGuid(Engine::GUID::Get());
		AssetsFileSystem::CreateResult result = fileSystem->CreateAssetFile(mCurrentOpenFolder[mCurrentIndex].back(), name, frame, meta);

		if (result == AssetsFileSystem::CreateResult::emSuccess)
		{
			LOG_INFO("空场景创建成功");

			ImGuiToast toast(ImGuiToastType_Success);
			toast.set_title("成功");
			toast.set_content("空场景创建成功");

			ImGui::InsertNotification(toast);
		}
		else if (result == AssetsFileSystem::CreateResult::emFileExists)
		{
			LOG_ERROR("文件在当前路径已存在 {0}", mCreateBuffer);

			ImGuiToast toast(ImGuiToastType_Error);
			toast.set_title("错误");
			toast.set_content("文件在当前路径已存在 %s", (const char*)mCreateBuffer);

			ImGui::InsertNotification(toast);
		}
		else if (result == AssetsFileSystem::CreateResult::emError)
		{
			ImGuiToast toast(ImGuiToastType_Error);
			toast.set_title("错误");
			toast.set_content("未知错误");

			ImGui::InsertNotification(toast);
		}
	}

	void EditorUIProjectPass::Ref_CreateMaterial()
	{
		LOG_INFO("创建一个空材质");
		mIsOpenCreateWindow = true;
		mCreateOrder = CreateItem::emCreateMaterial;
		mExtensionName = ".material";
	}

	void EditorUIProjectPass::CreateMaterial()
	{
		AssetsFileSystem* fileSystem = mEditor->GetFileSystem();

		std::filesystem::path currentFolder = GetCurrentFolder();
		std::string name(mCreateBuffer);
		name += ".material";

		Engine::MaterialData material;

		Engine::SerializerDataFrame frame;
		frame << material;
		Engine::SceneMeta meta;
		meta.SetGuid(Engine::GUID::Get());
		AssetsFileSystem::CreateResult result = fileSystem->CreateAssetFile(mCurrentOpenFolder[mCurrentIndex].back(), name, frame, meta);
	}

	void EditorUIProjectPass::Ref_ShowFolderInExplorer()
	{
		LOG_INFO("在资源管理器中显示");
		std::filesystem::path ph = mCurrentOpenFolder[mCurrentIndex].front()->mName;
		for (unsigned i = 1; i < mCurrentOpenFolder[mCurrentIndex].size(); ++i)
		{
			ph /= mCurrentOpenFolder[mCurrentIndex][i]->mName;
		}
		mEditor->OpenExplorer(ph);
	}

	void EditorUIProjectPass::Ref_Open()
	{
		LOG_INFO("打开");
	}

	void EditorUIProjectPass::Ref_Delete()
	{
		LOG_INFO("删除");
		AssetsFileSystem* fileSystem = mEditor->GetFileSystem();
		for (auto it = mSelectItem.begin(); it != mSelectItem.end(); ++it)
		{
			LOG_INFO("删除了 {0}", (**it)->mName.c_str());
			fileSystem->DeleteAssetFile(**it);
		}
	}

	void EditorUIProjectPass::Ref_Rename()
	{
		LOG_INFO("重命名");

		mIsOpenRenameWindow = true;
		mRenameFile = (*(mSelectItem.back()));
	}

	void EditorUIProjectPass::Ref_ImportAsset()
	{
		LOG_INFO("导入新资源");
		std::filesystem::path filePath;
		if (mEditor->OpenFileSelector(filePath))
		{
			AssetTool::LoadAsset(filePath, mCurrentOpenFolder[mCurrentIndex].back());
		}
	}
}

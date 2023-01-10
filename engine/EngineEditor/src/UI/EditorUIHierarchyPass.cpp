#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIHierarchyPass.h"
#include "EngineEditor/include/Application/AssetFIle.h"
#include "EngineEditor/include/UI/ImGuiExtensions/imgui_notify.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Editor
{
	IMPLEMENT_RTTI(EditorUIHierarchyPass, EditorUIPassBase);
	REGISTER_CLASS(EditorUIHierarchyPass);
	REGISTER_CLASS_METHOD(EditorUIHierarchyPass, Ref_ClipAndPaste, void);
	REGISTER_CLASS_METHOD(EditorUIHierarchyPass, Ref_CopyGameObject, void);
	REGISTER_CLASS_METHOD(EditorUIHierarchyPass, Ref_PasteGameObject, void);
	REGISTER_CLASS_METHOD(EditorUIHierarchyPass, Ref_PasteGameObjectForChild, void);
	REGISTER_CLASS_METHOD(EditorUIHierarchyPass, Ref_GameObjectReName, void);
	REGISTER_CLASS_METHOD(EditorUIHierarchyPass, Ref_GameObjectDelete, void);
	REGISTER_CLASS_METHOD(EditorUIHierarchyPass, Ref_CreateEmptyGameObject, void);
	REGISTER_CLASS_METHOD(EditorUIHierarchyPass, Ref_Create3DCube, void);
	REGISTER_CLASS_FIELD(EditorUIHierarchyPass, mIsHoveredGameObject, bool);


	EditorUIHierarchyPass::EditorUIHierarchyPass()
		:EditorUIPassBase()
	{ }

	EditorUIHierarchyPass::~EditorUIHierarchyPass()
	{

	}

	void EditorUIHierarchyPass::Initialize(Engine::ImGuiDevice* device, EngineEditor* editor)
	{
		EditorUIPassBase::Initialize(device, editor);

		mCurrentScene = Engine::WorldManager::GetInstance()->GetCurrentActiveLevel();

		mRightMenu = EditorRightMenu::CreateRightMenu(Engine::EngineFileSystem::GetInstance()->GetActualPath("UI/RightMenuInHierarchy.xml"), "EditorUIHierarchyPassRightMenu");
	}

	void EditorUIHierarchyPass::ShowUI()
	{
		if (!mIsOpen)
			return;

		ImGui::Begin("场景层级", &mIsOpen, ImGuiWindowFlags_NoCollapse);

		ImGui::BeginChild("Drag", ImGui::GetWindowSize());
		
		if (!mIsPopupRightMenu)
		{
			mCurrentHoveringGameObject = nullptr;
			mIsHoveredGameObject = false;
		}

		ImGui::Text("当前场景 %s", mCurrentScene->GetSceneName().c_str());

		ShowSceneHierarchy(mCurrentScene);

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			EditorRightMenu::ShowRightMenu(mRightMenu.get());
		}

		mIsPopupRightMenu = EditorRightMenu::RightMenuIsPopup(mRightMenu.get());

		EditorRightMenu::DrawRightMenu(mRightMenu.get(), this);

		ImGui::EndChild();
		DropNode(nullptr);

		ShowPopupWindow();

		ImGui::End();
	}

	// 在给定的节点上放置拖拽的节点
	void EditorUIHierarchyPass::DropNode(Engine::Actor* targetNode)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (targetNode == nullptr)
			{
				do
				{
					// 来自节点的拖拽
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(HierarchyNodeDrag))
					{
						if (mLaseDrag)
						{
							mLaseDrag = false;
							break;
						}

						Engine::Actor* moveNode = *(Engine::Actor**)payload->Data;

						LOG_INFO("将 {0} 放置到根节点下", moveNode->GetActorName().c_str());
						if (targetNode == moveNode->GetParent())
						{
							break;
						}

						moveNode->SetParent(targetNode);

						//mLaseDrag = true;
					}

					// 来自文件的拖拽
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UIProjectFileDrag))
					{
						if (mLaseDrag)
						{
							mLaseDrag = false;
							break;
						}

						std::shared_ptr<AssetFile> file = *(std::shared_ptr<AssetFile>*)payload->Data;
						std::filesystem::path fileName(file->mName);

						if (fileName.extension() == ".prefab")
						{
							std::shared_ptr<Engine::Blob> blob = file->GetBinary();
							Engine::Actor* actor = Engine::ResourceSerializer::LoadResourceFromMemory<Engine::Actor>(blob->GetData(), blob->GetSize());
							mCurrentScene->AddActor(actor);
							LOG_INFO("生成节点 {0}, 在根节点下", fileName);
						}
						else
						{
							LOG_ERROR("该文件不可以转换为GameObject {0}", file->mName.c_str());
						}
					}
				} while (false);

			}
			else
			{
				do
				{
					// 来自节点的拖拽
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(HierarchyNodeDrag))
					{
						if (mLaseDrag)
						{
							mLaseDrag = false;
							break;
						}
						mLaseDrag = true;

						Engine::Actor* moveNode = *(Engine::Actor**)payload->Data;

						LOG_INFO("将 {0}, 放置到 {1} 节点下", moveNode->GetActorName().c_str(), targetNode->GetActorName().c_str());
						if (targetNode == moveNode->GetParent())
						{
							break;
						}

						Engine::Actor* t = targetNode;

						while (t->GetParent() != moveNode && t->GetParent() != nullptr)
						{
							t = t->GetParent();
						}

						if (t->GetParent() == moveNode)
						{
							LOG_ERROR("{0} 是 {1} 的父节点", moveNode->GetActorName().c_str(), targetNode->GetActorName().c_str());
							ImGuiToast toast(ImGuiToastType_Error);
							toast.set_title("错误");
							toast.set_content("不能将父节点移动到子节点下");

							ImGui::InsertNotification(toast);
							break;
						}
						moveNode->SetParent(targetNode);
						
					}

					// 来自文件的拖拽
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UIProjectFileDrag))
					{
						if (mLaseDrag)
						{
							mLaseDrag = false;
							break;
						}

						std::shared_ptr<AssetFile> file = *(std::shared_ptr<AssetFile>*)payload->Data;
						std::filesystem::path fileName(file->mName);
						if (fileName.extension() == ".prefab")
						{
							std::shared_ptr<Engine::Blob> blob = file->GetBinary();
							Engine::Actor* actor = Engine::ResourceSerializer::LoadResourceFromMemory<Engine::Actor>(blob->GetData(), blob->GetSize());
							actor->SetParent(targetNode);

							LOG_INFO("生成节点 {0}, 在 {1} 节点下", fileName, targetNode->GetActorName().c_str());
						}
						else
						{
							LOG_ERROR("该文件不可以转换为GameObject {0}", file->mName.c_str());
						}

						mLaseDrag = true;
					}

				} while (false);
			}

			ImGui::EndDragDropTarget();
		}
	}

	void EditorUIHierarchyPass::ShowNode(Engine::Actor* node, std::string ext)
	{
		ext += node->GetActorName();
		bool node_open = ImGui::TreeNodeEx((node->GetActorName() + ext).c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

		DropNode(node);

		if (ImGui::IsItemHovered())
		{
			mCurrentHoveringGameObject = node;
			mIsHoveredGameObject = true;
			//LOG_INFO("正在悬浮 {0}", node->name.c_str());
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			// 在拖拽开始时设置拖拽数据
			ImGui::SetDragDropPayload(HierarchyNodeDrag, &node, sizeof(Engine::Actor*));
			ImGui::Text("Moving %s", (node->GetActorName().c_str()));
			ImGui::EndDragDropSource();
		}

		if (node_open)
		{
			int k = 0;
			for (Engine::Actor* child : node->GetChildrens())
			{
				ShowNode(child, ext + std::to_string(k));
				//ImGui::Separator();
				//DropNode(node);
				++k;
			}
			ImGui::TreePop();
		}

	}

	// 在给定的节点上创建 ImGui TreeNode
	void EditorUIHierarchyPass::ShowSceneHierarchy(Engine::Level* node)
	{
		std::list<Engine::Actor*>& actors = node->GetSceneActors();
		std::string ext = "##";
		int k = 0;
		for (Engine::Actor* actor : actors)
		{
			ShowNode(actor, ext + std::to_string(k));
			//ImGui::Separator();
			//DropNode(actor);
			++k;
		}
	}

	void EditorUIHierarchyPass::Ref_ClipAndPaste()
	{
		LOG_INFO("剪贴");
	}

	void EditorUIHierarchyPass::Ref_CopyGameObject()
	{
		LOG_INFO("复制");
	}

	void EditorUIHierarchyPass::Ref_PasteGameObject()
	{
		LOG_INFO("粘贴");
	}

	void EditorUIHierarchyPass::Ref_PasteGameObjectForChild()
	{
		LOG_INFO("粘贴为子对象");
	}

	void EditorUIHierarchyPass::Ref_GameObjectReName()
	{
		ASSERT(mCurrentHoveringGameObject != nullptr);

		mIsOpenRenameWindow = true;
		mSeleteActor = mCurrentHoveringGameObject;
	}

	void EditorUIHierarchyPass::Ref_GameObjectDelete()
	{
		ASSERT(mCurrentHoveringGameObject != nullptr);

		mIsOpenDeleteActorWindow = true;
		mSeleteActor = mCurrentHoveringGameObject;
	}

	void EditorUIHierarchyPass::Ref_CreateEmptyGameObject()
	{
		LOG_INFO("创建空对象");
	}

	void EditorUIHierarchyPass::Ref_Create3DCube()
	{
		LOG_INFO("立方体");
	}

	void EditorUIHierarchyPass::ShowPopupWindow()
	{
#pragma region 重命名
		if (mIsOpenRenameWindow)
		{
			ImGui::OpenPopup("GamePlay重命名");
			mIsOpenRenameWindow = false;
			memset(mRenameBuffer, 0x00, 256);
			std::string actorName = mSeleteActor->GetActorName();
			memcpy(mRenameBuffer, actorName.c_str(), actorName.size());
		}
		if (ImGui::BeginPopupModal("GamePlay重命名", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImVec2 window_pos = ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f);
			ImGui::SetWindowPos(window_pos);

			ImGui::InputText(".actor", mRenameBuffer, 256);

			if (ImGui::Button("确定"))
			{
				mSeleteActor->SetActorName(mRenameBuffer);
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
#pragma region 删除
		if (mIsOpenDeleteActorWindow)
		{
			ImGui::OpenPopup("删除GamePlay");
			mIsOpenDeleteActorWindow = false;
			mDeleteText = mSeleteActor->GetActorName() + " 请核查";
		}
		if (ImGui::BeginPopupModal("删除GamePlay", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImVec2 window_pos = ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f);
			ImGui::SetWindowPos(window_pos);

			ImGui::Text("即将删除节点 %s", mDeleteText.c_str());

			if (ImGui::Button("确定"))
			{
				if (mSeleteActor->IsRootNode())
				{
					std::list<Engine::Actor*>& actors = mCurrentScene->GetSceneActors();
					auto it = std::find(actors.begin(), actors.end(), mSeleteActor);
					ASSERT(it != actors.end());

					actors.erase(it);
					delete mSeleteActor;
					mSeleteActor = nullptr;
				}
				else
				{
					Engine::Actor* parent = mSeleteActor->GetParent();
					std::list<Engine::Actor*>& actors = parent->GetChildrens();
					auto it = std::find(actors.begin(), actors.end(), mSeleteActor);
					ASSERT(it != actors.end());

					actors.erase(it);
					delete mSeleteActor;
					mSeleteActor = nullptr;
				}
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


	}

}

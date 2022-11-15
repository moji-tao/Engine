#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIProjectPass.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "Core/Base/macro.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Editor
{
	EditorUIProjectPass::EditorUIProjectPass()
	{
		mRoot = std::make_shared<Node>();
		mRoot->mIsFolder = true;
		mRoot->mName = "Assets";
		mCurrentPath = "Assets";
		dfsFolder(mRoot.get(), Engine::ProjectFileSystem::GetInstance()->GetActualPath(mCurrentPath));
		mCurrentFolder = mRoot.get();

	}

	void EditorUIProjectPass::dfsFolder(Node* mRoot, const std::filesystem::path& currentPath)
	{
		for(const auto& file : std::filesystem::directory_iterator(currentPath))
		{
			std::shared_ptr<Node> node = std::make_shared<Node>();
			node->mParent = mRoot;
			node->mName = file.path().filename().generic_string();
			if(file.is_directory())
			{
				node->mIsFolder = true;
				dfsFolder(node.get(), file);
			}
			mRoot->next.push_back(node);
		}
	}

	EditorUIProjectPass::~EditorUIProjectPass()
	{
		mRoot.reset();
	}

	void EditorUIProjectPass::ShowUI()
	{
		if (!mIsOpen)
			return;

		ImGui::Begin("项目", &mIsOpen, ImGuiWindowFlags_NoCollapse);

		ImVec2 windowSize = ImGui::GetWindowSize();
		windowSize.x = std::max(500.0f, windowSize.x);
		windowSize.y = std::max(200.0f, windowSize.y);
		ImGui::SetWindowSize(windowSize);
		float size = 100.0f;

		// 左侧树状文件夹
		ImGui::BeginChild("AssetsLeft", ImVec2(windowSize.x * 0.2f, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		ShowAssetsTree(mRoot.get());
		ImGui::EndChild();

		ImGui::SameLine();

		// 右侧管理器视图
		ImGui::BeginChild("AssetsRight", ImVec2(windowSize.x * 0.8f, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Text("当前路径 %s", mCurrentPath.generic_string().c_str(), windowSize.x * 0.8f);

		if (ImGui::BeginTable("AssetsSplit", (int)(windowSize.x * 0.8f / size), ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
		{
			for (auto file : mCurrentFolder->next)
			{
				ImGui::TableNextColumn();
				ImGui::Button(file->mName.c_str(), ImVec2(size, size));
				//ImGui::ImageButton()
			}

			ImGui::EndTable();
		}

		ImGui::EndChild();

		ImGui::End();
	}

	void EditorUIProjectPass::ShowAssetsTree(Node* node)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Selected;
		bool isOpen = ImGui::TreeNodeEx(node->mName.c_str(), flags);
		if(ImGui::IsItemClicked())
		{
			mCurrentFolder = node;
			mCurrentPath = "";
			std::vector<std::string> vec;
			Node* t = mCurrentFolder;
			while(t)
			{
				vec.push_back(t->mName);
				t = t->mParent;
			}
			for(int i=vec.size()-1;i>=0;--i)
			{
				mCurrentPath /= vec[i];
			}
		}
		
		if(isOpen)
		{
			for (std::shared_ptr<Node> nextNode : node->next)
			{
				if (nextNode->mIsFolder)
				{
					ShowAssetsTree(nextNode.get());
				}
			}

			ImGui::TreePop();
		}
	}


}

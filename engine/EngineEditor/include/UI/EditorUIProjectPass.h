#pragma once
#include <filesystem>
#include <list>
#include <string>
#include "EngineEditor/include/UI/EditorUIPassBase.h"

namespace Editor
{
	struct Node
	{
		bool mIsFolder = false;
		std::string mName;
		Node* mParent = nullptr;
		std::list<std::shared_ptr<Node>> next;
	};

	class EditorUIProjectPass : public EditorUIPassBase
	{
	public:
		EditorUIProjectPass();

		virtual ~EditorUIProjectPass() override;

	public:
		virtual void ShowUI() override;

	private:
		void dfsFolder(Node* mRoot, const std::filesystem::path& currentPath);

		void ShowAssetsTree(Node* node);

	private:
		std::shared_ptr<Node> mRoot;
		Node* mCurrentFolder = nullptr;
		std::filesystem::path mCurrentPath;
	};
}

#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "EngineEditor/include/UI/EditorUIPassBase.h"

namespace Editor
{
	class EditorUIConsolePass : public EditorUIPassBase
	{
	public:
		EditorUIConsolePass();

		virtual ~EditorUIConsolePass() override;

		virtual void Initialize(Engine::ImGuiDevice* device, EngineEditor* editor) override;

	public:
		virtual void ShowUI() override;

	private:
		std::vector<std::string> mLogs;

		std::stringstream fo;
		std::streambuf* mOldcout;
	};
}

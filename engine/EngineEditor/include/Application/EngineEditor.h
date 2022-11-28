#pragma once
#include "EngineEditor/include/Application/IApplication.h"
#include "Framework/Interface/IRuntimeModule.h"
#include "EngineEditor/include/UI/EditorUI.h"

namespace Editor
{
	class EngineEditor : public IApplication
	{
	public:
		EngineEditor() = default;

		virtual ~EngineEditor() override = default;

	public:
		virtual bool Initialize(const EditorConfig* config) override;

		virtual void Finalize() override;

		virtual void Run() override;

	private:

	private:
		static void FilesDrop(int fileCount, const char** filePath);

	protected:
		bool mIsQuit = false;

		std::shared_ptr<EditorUI> mEditorUI;
	};
}

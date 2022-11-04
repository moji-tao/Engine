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
		virtual bool Initialize(const ApplicationDesc* config) override;

		virtual void Finalize() override;

		virtual void Run() override;

	private:
		void AnalysisConfig(const ApplicationDesc* config, Engine::InitConfig& initConfig);

	protected:
		bool mIsQuit = false;

		std::shared_ptr<EditorUI> mEditorUI;
	};
}

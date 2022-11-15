#pragma once
#include "EngineEditor/include/UI/EditorUIPassBase.h"

namespace Editor
{
	class EditorUIInspectorPass : public EditorUIPassBase
	{
	public:
		EditorUIInspectorPass();

		virtual ~EditorUIInspectorPass() override;

	public:
		virtual void ShowUI() override;
	};
}

#pragma once
#include "EngineEditor/include/UI/EditorUIPassBase.h"

namespace Editor
{
	class EditorUIScenePass : public EditorUIPassBase
	{
	public:
		EditorUIScenePass();

		virtual ~EditorUIScenePass() override;

	public:
		virtual void ShowUI() override;
	};
}

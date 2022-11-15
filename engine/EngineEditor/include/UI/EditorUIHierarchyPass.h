#pragma once
#include "EngineEditor/include/UI/EditorUIPassBase.h"

namespace Editor
{
	class EditorUIHierarchyPass : public EditorUIPassBase
	{
	public:
		EditorUIHierarchyPass();

		virtual ~EditorUIHierarchyPass() override;

	public:
		virtual void ShowUI() override;
	};
}

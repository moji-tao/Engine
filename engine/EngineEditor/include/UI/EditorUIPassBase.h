#pragma once
#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime/include/Function/Render/ImGuiDevice.h"
#include "EngineRuntime/include/Core/Meta/Object.h"

namespace Editor
{
	class EditorUIPassBase : public Engine::Object
	{
		DECLARE_RTTI;
	public:
		EditorUIPassBase();

		virtual ~EditorUIPassBase() = default;

		virtual void Initialize(Engine::ImGuiDevice* device, EngineEditor* editor);

	public:
		virtual void ShowUI() = 0;

	public:
		bool mIsOpen = true;

	protected:
		Engine::ImGuiDevice* mDevice = nullptr;

		EngineEditor* mEditor = nullptr;
	};
}

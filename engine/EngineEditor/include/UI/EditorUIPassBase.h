#pragma once

namespace Editor
{
	class EditorUIPassBase
	{
	public:
		EditorUIPassBase() = default;

		virtual ~EditorUIPassBase() = default;

	public:
		virtual void ShowUI() = 0;

	public:
		bool mIsOpen = true;
	};
}

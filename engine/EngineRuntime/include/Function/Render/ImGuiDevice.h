#pragma once
#include <filesystem>
#include <imgui/imgui.h>
#include "EngineRuntime/include/Function/Window/WindowUI.h"

namespace Engine
{
	struct ImGuiTextureInfo
	{
		ImTextureID TextureID = nullptr;
		uint32_t Width = 0;
		uint32_t Height = 0;
	};

	class ImGuiDevice
	{
	public:
		ImGuiDevice(WindowUI* editorUI);

		virtual ~ImGuiDevice() = default;

	public:
		virtual void DrawUI() = 0;

		virtual void EndDraw() = 0;

		virtual ImGuiTextureInfo LoadTexture(const std::filesystem::path& ph) = 0;

		virtual void SetSceneUITexture(void* resource) = 0;

		virtual ImTextureID GetSceneTextureInfo() = 0;

	protected:
		WindowUI* mEditorUI = nullptr;
	};
}

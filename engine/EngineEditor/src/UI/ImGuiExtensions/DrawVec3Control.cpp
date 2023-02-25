#include <imgui/imgui.h>
#include "EngineEditor/include/UI/ImGuiExtensions/DrawVec3Control.h"

namespace ImGui
{
	void DrawVec3Control(const std::string& label, Engine::Vector3& values)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 2));
		ImGui::PushItemWidth(200);
		ImGui::Text("%-10s", label.c_str());
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(20);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.204f, 0.204f, 0.204f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.204f, 0.204f, 0.204f, 1.0f));

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values[0], 0.1f, 0.0f, 0.0f, "X", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SameLine(0);

		ImGui::PushItemWidth(80);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.204f, 0.204f, 0.204f, 1.0f));
		ImGui::InputFloat("##X_Input", &values[0]);
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values[1], 0.1f, 0.0f, 0.0f, "Y", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SameLine();

		ImGui::PushItemWidth(80);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.204f, 0.204f, 0.204f, 1.0f));
		ImGui::InputFloat("##Y_Input", &values[1]);
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values[2], 0.1f, 0.0f, 0.0f, "Z", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SameLine();

		ImGui::PushItemWidth(80);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.204f, 0.204f, 0.204f, 1.0f));
		ImGui::InputFloat("##Z_Input", &values[2]);
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::PopID();
	}
}


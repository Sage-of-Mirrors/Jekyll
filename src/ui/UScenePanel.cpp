#include "ui/UScenePanel.hpp"

#include <imgui.h>


UScenePanel::UScenePanel() {

}

UScenePanel::~UScenePanel() {

}

void UScenePanel::Render() {
    ImGui::Begin("Scenegraph");

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.05f, 0.05f, 0.05f, 1.0f });

	ImGui::BeginChild("##scenegraph", ImVec2(0, 0), true, 0);

	ImGui::EndChild();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

    ImGui::End();
}

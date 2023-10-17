#include "ui/UScenePanel.hpp"
#include "model/MScenegraph.hpp"

#include <imgui.h>


UScenePanel::UScenePanel() {

}

UScenePanel::~UScenePanel() {

}

void UScenePanel::RenderScenegraphTree(MScenegraph* scenegraph) {
	if (scenegraph == nullptr) {
		return;
	}

	scenegraph->GetRoot()->RenderUI_Recursive(nullptr, nullptr, 0);
}

void UScenePanel::RenderScenegraphNode_Recursive(const MScenegraphNode* node) {

}

void UScenePanel::Render(MScenegraph* scenegraph) {
    ImGui::Begin("Scenegraph");

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.05f, 0.05f, 0.05f, 1.0f });

	ImGui::BeginChild("##scenegraph", ImVec2(0, 0), true, 0);

	RenderScenegraphTree(scenegraph);

	ImGui::EndChild();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

    ImGui::End();
}

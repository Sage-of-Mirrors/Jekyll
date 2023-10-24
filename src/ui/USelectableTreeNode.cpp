#include "ui/USelectableTreeNode.hpp"

#include <imgui.h>

bool USelectableTreeNode(const std::string& name, const bool& highlight, bool& selected) {
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (highlight) {
		base_flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool node_open = ImGui::TreeNodeEx(name.c_str(), base_flags);
	if (ImGui::IsItemClicked()) {
		selected = true;
	}

	return node_open;
}

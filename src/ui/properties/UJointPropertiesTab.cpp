#include "ui/properties/UJointPropertiesTab.hpp"

#include "model/MJointData.hpp"

#include <imgui.h>


UJointPropertiesTab::UJointPropertiesTab() : UJointPropertiesTab(nullptr) {

}

UJointPropertiesTab::UJointPropertiesTab(MJointData* jointData) : mJointData(jointData) {
    mIcon = "\xee\xbe\xb1";
    mToolTip = "Joints";
}

UJointPropertiesTab::~UJointPropertiesTab() {

}

void UJointPropertiesTab::RenderJoints_Recursive(MJoint* jnt) {
    if (jnt->mChildren.size() == 0) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6);
        ImGui::Text("%s", jnt->GetName());

        return;
    }

    if (ImGui::TreeNode(jnt->GetName())) {
        for (MJoint* child : jnt->mChildren) {
            RenderJoints_Recursive(child);
        }

        ImGui::TreePop();
    }
}

void UJointPropertiesTab::Render() {
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.25f, 0.25f, 0.25f, 1.0f });

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 10 });

    ImGui::SetCursorPos({ contentMin.x + 35, contentMin.y + 5 });

    ImGui::BeginChild("##skeletonView", {contentMax.x - contentMin.x - 40, 300}, true, ImGuiWindowFlags_AlwaysAutoResize);

    RenderJoints_Recursive(mJointData->GetRootJoint());

    ImGui::EndChild();

    ImGui::PopStyleVar(2);

    ImGui::PopStyleColor();
}

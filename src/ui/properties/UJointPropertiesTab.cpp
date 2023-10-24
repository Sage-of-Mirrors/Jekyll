#include "ui/properties/UJointPropertiesTab.hpp"
#include "ui/USelectableTreeNode.hpp"
#include "ui/UEnumCombo.hpp"

#include "model/MJointData.hpp"

#include <imgui.h>


UJointPropertiesTab::UJointPropertiesTab() : UJointPropertiesTab(nullptr) {

}

UJointPropertiesTab::UJointPropertiesTab(MJointData* jointData) : mJointData(jointData), mSelectedJoint(nullptr) {
    mIcon = "\xee\xbe\xb1";
    mToolTip = "Joints";
}

UJointPropertiesTab::~UJointPropertiesTab() {

}

void UJointPropertiesTab::RenderJoints_Recursive(MJoint* jnt) {
    if (jnt->Children.size() == 0) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6);

        if (ImGui::Selectable(jnt->GetName(), mSelectedJoint == jnt)) {
            mSelectedJoint = jnt;
        }

        return;
    }

    bool bWasSelected = false;
    if (USelectableTreeNode(jnt->Name, mSelectedJoint == jnt, bWasSelected)) {
        for (MJoint* child : jnt->Children) {
            RenderJoints_Recursive(child);
        }

        ImGui::TreePop();
    }

    if (bWasSelected) {
        mSelectedJoint = jnt;
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

    if (mSelectedJoint != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.45f, 0.45f, 0.45f, 1.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 10 });

        ImGui::SetCursorPos({ contentMin.x + 35, ImGui::GetCursorPosY() + 4 });

        ImGui::BeginChild("##jointProps", { contentMax.x - contentMin.x - 40, 138 });

        ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 6, ImGui::GetCursorPosY() + 10 });

        ImGui::BeginDisabled();
        ImGui::InputText("Joint Name", mSelectedJoint->Name.data(), mSelectedJoint->Name.length() + 1);
        ImGui::EndDisabled();

        ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 6, ImGui::GetCursorPosY() + 10 });
        ImGui::Checkbox("Ignore Parent Scale", &mSelectedJoint->bIgnoreParentScale);

        int tmpKind = mSelectedJoint->Kind;

        ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 6, ImGui::GetCursorPosY() + 10 });
        ImGui::InputInt("Kind", &tmpKind);

        if (tmpKind < 0) {
            tmpKind = 0;
        }
        else if (tmpKind > 0x0F) {
            tmpKind = 0x0F;
        }

        if (tmpKind != mSelectedJoint->Kind) {
            mSelectedJoint->Kind = tmpKind;
        }

        ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 6, ImGui::GetCursorPosY() + 10 });
        UEnumCombo("Matrix Type", mSelectedJoint->MatrixType);

        ImGui::EndChild();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

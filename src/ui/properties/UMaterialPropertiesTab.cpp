#include "ui/properties/UMaterialPropertiesTab.hpp"
#include "ui/UEnumCombo.hpp"
#include "ui/UUInt8Input.hpp"

#include <J3D/Material/J3DMaterial.hpp>

#include <imgui.h>

#include "model/MMaterialData.hpp"


UMaterialPropertiesTab::UMaterialPropertiesTab() : UMaterialPropertiesTab(nullptr) {

}

UMaterialPropertiesTab::UMaterialPropertiesTab(MMaterialData* matData) : mMaterialData(matData) {
    mIcon = "M";
    mToolTip = "Materials";
}

UMaterialPropertiesTab::~UMaterialPropertiesTab() {

}

void UMaterialPropertiesTab::Render() {
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.25f, 0.25f, 0.25f, 1.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 10 });

    ImGui::SetCursorPos({ contentMin.x + 35, contentMin.y + 5 });

    ImGui::BeginChild("##matView", { contentMax.x - contentMin.x - 40, 300 }, true, ImGuiWindowFlags_AlwaysAutoResize);

    const auto& materials = mMaterialData->GetMaterials();
    for (uint32_t i = 0; i < materials.size(); i++) {
        std::shared_ptr<J3DMaterial> curMaterial = materials[i];

        if (ImGui::Selectable(curMaterial->Name.data(), mSelectedMaterial.lock() == curMaterial)) {
            mSelectedMaterial = materials[i];
        }
    }

    ImGui::EndChild();

    if (!mSelectedMaterial.expired()) {
        std::shared_ptr<J3DMaterial> matLocked = mSelectedMaterial.lock();
        bool bMaterialChanged = false;

        bMaterialChanged |= RenderBlendModeControls(matLocked);
        bMaterialChanged |= RenderAlphaCompareControls(matLocked);
        bMaterialChanged |= RenderZModeControls(matLocked);

        if (bMaterialChanged) {
            matLocked->GenerateShaders();
        }
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

bool UMaterialPropertiesTab::RenderBlendModeControls(std::shared_ptr<J3DMaterial> material) {
    bool bMaterialChanged = false;

    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.45f, 0.45f, 0.45f, 1.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 10 });

    ImGui::SetCursorPosX(contentMin.x + 35);

    ImGui::BeginChild("##matBlendProps", { contentMax.x - contentMin.x - 40, 96 });
    ImGui::Spacing();

    ImGui::Indent(8.0f);

    bMaterialChanged |= UEnumCombo("Blend Mode Type", material->PEBlock.mBlendMode.Type);
    bMaterialChanged |= UEnumCombo("Blend Mode Source", material->PEBlock.mBlendMode.SourceFactor);
    bMaterialChanged |= UEnumCombo("Blend Mode Operation", material->PEBlock.mBlendMode.Operation);
    bMaterialChanged |= UEnumCombo("Blend Mode Dest", material->PEBlock.mBlendMode.DestinationFactor);

    ImGui::Unindent(8.0f);
    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    return bMaterialChanged;
}

bool UMaterialPropertiesTab::RenderAlphaCompareControls(std::shared_ptr<J3DMaterial> material) {
    bool bMaterialChanged = false;

    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.45f, 0.45f, 0.45f, 1.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 10 });

    ImGui::SetCursorPosX(contentMin.x + 35);

    ImGui::BeginChild("##matAlphaProps", { contentMax.x - contentMin.x - 40, 128 });
    ImGui::Spacing();

    ImGui::Indent(8.0f);

    bMaterialChanged |= UEnumCombo("Compare Func 0", material->PEBlock.mAlphaCompare.CompareFunc0);
    bMaterialChanged |= UUInt8Input("Reference 0", material->PEBlock.mAlphaCompare.Reference0);

    ImGui::Spacing();

    bMaterialChanged |= UEnumCombo("Operation", material->PEBlock.mAlphaCompare.Operation);

    ImGui::Spacing();

    bMaterialChanged |= UEnumCombo("Compare Func 1", material->PEBlock.mAlphaCompare.CompareFunc1);
    bMaterialChanged |= UUInt8Input("Reference 1", material->PEBlock.mAlphaCompare.Reference1);

    ImGui::Unindent(8.0f);
    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    return bMaterialChanged;
}

bool UMaterialPropertiesTab::RenderZModeControls(std::shared_ptr<J3DMaterial> material) {
    bool bMaterialChanged = false;

    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.45f, 0.45f, 0.45f, 1.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 10 });

    ImGui::SetCursorPosX(contentMin.x + 35);

    ImGui::BeginChild("##matZModeProps", { contentMax.x - contentMin.x - 40, 100 });
    ImGui::Spacing();

    ImGui::Indent(8.0f);

    bMaterialChanged |= ImGui::Checkbox("ZMode Enable", &material->PEBlock.mZMode.Enable);
    bMaterialChanged |= UEnumCombo("ZMode Function", material->PEBlock.mZMode.Function);
    bMaterialChanged |= ImGui::Checkbox("ZMode Update Enable", &material->PEBlock.mZMode.UpdateEnable);

    ImGui::Spacing();

    bMaterialChanged |= ImGui::Checkbox("ZCompLoc", &material->PEBlock.mZCompLoc);

    ImGui::Unindent(8.0f);
    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    return bMaterialChanged;
}

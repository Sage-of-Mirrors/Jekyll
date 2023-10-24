#include "ui/properties/UShapePropertiesTab.hpp"
#include "ui/UEnumCombo.hpp"

#include "model/MShapeData.hpp"

#include <imgui.h>


UShapePropertiesTab::UShapePropertiesTab() : UShapePropertiesTab(nullptr) {

}

UShapePropertiesTab::UShapePropertiesTab(MShapeData* shapeData) : mShapeData(shapeData), mCurrentShape(nullptr) {
    mIcon = "\xee\xa0\x8d";
    mToolTip = "Shapes";
}

UShapePropertiesTab::~UShapePropertiesTab() {

}

void UShapePropertiesTab::Render() {
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.25f, 0.25f, 0.25f, 1.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 10 });

    ImGui::SetCursorPos({ contentMin.x + 35, contentMin.y + 5 });

    ImGui::BeginChild("##shapeView", {contentMax.x - contentMin.x - 40, 300}, true, ImGuiWindowFlags_AlwaysAutoResize);

    const std::vector<MShape*> shapes = mShapeData->GetShapes();
    for (uint32_t i = 0; i < shapes.size(); i++) {
        char buf[12];
        snprintf(buf, 12, "Shape %i", i);
        buf[11] = '\0';

        if (ImGui::Selectable(buf, mCurrentShape == shapes[i])) {
            mCurrentShape = shapes[i];
        }
    }

    ImGui::EndChild();

    if (mCurrentShape != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.45f, 0.45f, 0.45f, 1.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 10 });

        ImGui::SetCursorPos({ contentMin.x + 35, ImGui::GetCursorPosY() + 4});

        ImGui::BeginChild("##shapeProps", { contentMax.x - contentMin.x - 40, 40 });

        ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 6, ImGui::GetCursorPosY() + 10});
        UEnumCombo("Matrix Type", mCurrentShape->MatrixType);

        ImGui::EndChild();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

#include "ui/UPropertiesPanel.hpp"
#include "ui/properties/UPropertiesTabBase.hpp"

#include <imgui.h>


UPropertiesPanel::UPropertiesPanel() : mCurrentTab(nullptr) {

}

UPropertiesPanel::~UPropertiesPanel() {

}

void UPropertiesPanel::RenderTabBg() {
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.1f, 0.1f, 0.1f, 1.0f });
    ImGui::SetCursorPos(contentMin);

    ImGui::BeginChild("##tabBg", { 30, contentMax.y - contentMin.y });
    RenderTabs();
    ImGui::EndChild();

    ImGui::PopStyleColor();
}

void UPropertiesPanel::RenderTabs() {
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImGui::SetCursorPos({ contentMin.x + 5, contentMin.y + 10 });

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.2f, 0.2f, 1.0f });

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0.0f, 1.0f });
    
    ImGui::SetWindowFontScale(1.5f);

    for (UPropertiesTabBase* tab : mTabs) {
        bool bSetButtonColor = false;
        if (mCurrentTab == tab) {
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.3f, 0.3f, 0.3f, 1.0f });
            bSetButtonColor = true;
        }

        bool buttonResult = ImGui::Button(tab->GetIcon(), { 30, 30 });
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::SetTooltip("%s", tab->GetTooltip());
        }
        if (buttonResult) {
            mCurrentTab = tab;
        }

        if (bSetButtonColor) {
            ImGui::PopStyleColor();
        }

        ImGui::SetCursorPos({ contentMin.x + 5, ImGui::GetCursorPosY() });
    }

    ImGui::SetWindowFontScale(1.0f);

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void UPropertiesPanel::RenderContent() {
    if (mCurrentTab == nullptr) {
        return;
    }

    mCurrentTab->Render();
}

void UPropertiesPanel::Render() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.3f, 0.3f, 0.3f, 1.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

    ImGui::Begin("Properties");

    RenderTabBg();
    RenderContent();

    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

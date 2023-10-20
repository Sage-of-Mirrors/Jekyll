#include "ui/ULightsPanel.hpp"
#include "application/AJ3DContext.hpp"

#include <imgui.h>

#include <string>


constexpr int LIGHT_CNT = 8;

void ULightsPanel::RenderLightControl(J3DLight& light, int index) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.25f, 0.25f, 0.25f, 1.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

    std::string childName = "##lightCtrl" + std::to_string(index);
    ImGui::BeginChild(childName.c_str(), {250, 250});

    ImGui::Text("Color");
    ImGui::ColorEdit4("##color", &light.Color.r);

    ImGui::Text("Position");
    ImGui::InputFloat4("##position", &light.Position.x);

    ImGui::Text("Direction");
    ImGui::InputFloat4("##direction", &light.Direction.x);

    ImGui::Text("Angle Attenuation");
    ImGui::InputFloat4("##angleAtten", &light.AngleAtten.x);

    ImGui::Text("Distance Attenuation");
    ImGui::InputFloat4("##distAtten", &light.DistAtten.x);

    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void ULightsPanel::Render() {
    J3DLight* lights = mJ3DContext->GetLights();

    ImGui::Begin("Lights", &bIsOpen);

    for (uint32_t i = 0; i < LIGHT_CNT; i++) {
        RenderLightControl(lights[i], i);
    }

    ImGui::End();
}

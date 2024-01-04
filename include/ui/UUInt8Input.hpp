#pragma once

#include <cstdint>
#include <string>

#include <imgui.h>


bool UUInt8Input(std::string name, uint8_t& value) {
    int tempValue = value;
    bool bChanged = false;

    if (ImGui::InputInt(name.data(), &tempValue)) {
        if (tempValue < 0) {
            tempValue = 0;
        }
        else if (tempValue > 255) {
            tempValue = 255;
        }
        else {
            value = tempValue & 0xFF;
            bChanged = true;
        }
    }

    return bChanged;
}

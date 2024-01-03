#include "ui/properties/UTexturePropertiesTab.hpp"
#include "ui/UEnumCombo.hpp"

#include "model/MTextureData.hpp"

#include <J3D/Texture/J3DTexture.hpp>
#include <J3D/Texture/J3DTextureFactory.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>


UTexturePropertiesTab::UTexturePropertiesTab() : UTexturePropertiesTab(nullptr) {

}

UTexturePropertiesTab::UTexturePropertiesTab(MTextureData* texData) : mTextureData(texData) {
    mIcon = "T";
    mToolTip = "Textures";
}

UTexturePropertiesTab::~UTexturePropertiesTab() {

}

void UTexturePropertiesTab::Render() {
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.25f, 0.25f, 0.25f, 1.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 10 });

    ImGui::SetCursorPos({ contentMin.x + 35, contentMin.y + 5 });

    ImGui::BeginChild("##texView", { contentMax.x - contentMin.x - 40, 300 }, true, ImGuiWindowFlags_AlwaysAutoResize);

    const auto& textures = mTextureData->GetTextures();
    for (uint32_t i = 0; i < textures.size(); i++) {
        std::shared_ptr<J3DTexture> texLocked = textures[i];

        char buf[64];
        snprintf(buf, 64, "%s##%i", texLocked->Name.data(), i);

        if (ImGui::Selectable(buf, mSelectedTexture.lock() == texLocked)) {
            mSelectedTexture = textures[i];
            OnSelectedTextureChanged();
        }
    }

    ImGui::EndChild();

    if (!mSelectedTexture.expired()) {
        std::shared_ptr<J3DTexture> texLocked = mSelectedTexture.lock();

        float centerX = (contentMax.x + contentMin.x) / 2.0f + 30.0f;

        for (uint32_t i = 0; i < texLocked->MipmapCount; i++) {
            float mipWidth = texLocked->Width / std::pow(2.0f, i);
            float mipHeight = texLocked->Height / std::pow(2.0f, i);

            ImGui::SetCursorPosX(centerX - (mipWidth / 2.0f));
            ImGui::Image((ImTextureID)mSelectedTextureViews[i], { mipWidth, mipHeight });
        }

        ImGui::Spacing();

        ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 35, ImGui::GetCursorPosY() + 10 });
        if (ImGui::CollapsingHeader("Basic Info")) {
            ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 60, ImGui::GetCursorPosY() + 10 });
            UEnumCombo("Format", texLocked->TextureFormat);

            ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 60, ImGui::GetCursorPosY() + 10 });
            if (UEnumCombo("Wrap S", texLocked->WrapS)) {
                //glTextureParameteri(texLocked->TexHandle, GL_TEXTURE_WRAP_S, J3DTextureFactory::GXWrapToGLWrap(texLocked->WrapS));
            }
            ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 60, ImGui::GetCursorPosY() + 10 });
            if (UEnumCombo("Wrap T", texLocked->WrapT)) {
                //glTextureParameteri(texLocked->TexHandle, GL_TEXTURE_WRAP_T, J3DTextureFactory::GXWrapToGLWrap(texLocked->WrapT));
            }
        }

        ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 35, ImGui::GetCursorPosY() + 10 });
        if (ImGui::CollapsingHeader("Mipmap Info")) {
            ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 60, ImGui::GetCursorPosY() + 10 });
            if (UEnumCombo("Min Filter", texLocked->MinFilter)) {
                //glTextureParameteri(texLocked->TexHandle, GL_TEXTURE_MIN_FILTER, J3DTextureFactory::GXFilterToGLFilter(texLocked->MinFilter));
            }

            ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 60, ImGui::GetCursorPosY() + 10 });
            if (UEnumCombo("Mag Filter", texLocked->MagFilter)) {
                //glTextureParameteri(texLocked->TexHandle, GL_TEXTURE_MAG_FILTER, J3DTextureFactory::GXFilterToGLFilter(texLocked->MagFilter));
            }

            ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 60, ImGui::GetCursorPosY() + 10 });
            if (ImGui::InputInt("Min LOD", &texLocked->MinLOD)) {
                glTextureParameterf(texLocked->TexHandle, GL_TEXTURE_MIN_LOD, static_cast<float>(texLocked->MinLOD) * 0.125f);
            }

            ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 60, ImGui::GetCursorPosY() + 10 });
            if (ImGui::InputInt("Max LOD", &texLocked->MaxLOD)) {
                glTextureParameterf(texLocked->TexHandle, GL_TEXTURE_MAX_LOD, static_cast<float>(texLocked->MaxLOD) * 0.125f);
            }

            ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 60, ImGui::GetCursorPosY() + 10 });
            if (ImGui::InputInt("LOD Bias", &texLocked->LODBias)) {
                glTextureParameterf(texLocked->TexHandle, GL_TEXTURE_LOD_BIAS, static_cast<float>(texLocked->LODBias) * 0.01f);
            }

            ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 60, ImGui::GetCursorPosY() + 10 });
            if (UEnumCombo("Max Anisotropy", texLocked->MaxAnisotropy)) {
                //glTextureParameterf(texLocked->TexHandle, GL_TEXTURE_MAX_ANISOTROPY, J3DTextureFactory::GXAnisoToGLAniso(texLocked->MaxAnisotropy));
            }
        }
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void UTexturePropertiesTab::OnSelectedTextureChanged() {
    if (!mSelectedTextureViews.empty()) {
        glDeleteTextures(mSelectedTextureViews.size(), &mSelectedTextureViews[0]);
    }

    std::shared_ptr<J3DTexture> texLocked = mSelectedTexture.lock();
    for (uint32_t i = 0; i < texLocked->ImageData.size(); i++) {
        uint32_t newView;
        glGenTextures(1, &newView);

        glTextureView(newView, GL_TEXTURE_2D, texLocked->TexHandle, GL_RGBA8, i, 1, 0, 1);

        mSelectedTextureViews.push_back(newView);
    }
}

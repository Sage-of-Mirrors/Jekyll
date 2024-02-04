#include "ui/UViewport.hpp"
#include "application/AJ3DContext.hpp"

#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include <imgui.h>

#include <string>


constexpr int TEX_COLOR = 0;
constexpr int TEX_DEPTH = 1;

constexpr float COLOR_RESET[] = { 0.5f, 0.5f, 0.5f, 1.0f };
constexpr float DEPTH_RESET = 1.0f;


UViewport::UViewport(std::string name) : mViewportName(name) {
    CreateFramebuffer();
}

UViewport::~UViewport() {
    glDeleteFramebuffers(1, &mFBO);
    glDeleteTextures(2, mTexIds);
}

void UViewport::CreateFramebuffer() {
    // Generate framebuffer
    glCreateFramebuffers(1, &mFBO);

    // Generate color texture
    glCreateTextures(GL_TEXTURE_2D, 2, mTexIds);
    glTextureStorage2D(mTexIds[TEX_COLOR], 1, GL_RGB8, mViewportWidth, mViewportHeight);
    glTextureParameteri(mTexIds[TEX_COLOR], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(mTexIds[TEX_COLOR], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Generate depth texture
    glTextureStorage2D(mTexIds[TEX_DEPTH], 1, GL_DEPTH_COMPONENT32F, mViewportWidth, mViewportHeight);

    // Attach textures to framebuffer
    glNamedFramebufferTexture(mFBO, GL_COLOR_ATTACHMENT0, mTexIds[TEX_COLOR], 0);
    glNamedFramebufferTexture(mFBO, GL_DEPTH_ATTACHMENT, mTexIds[TEX_DEPTH], 0);

    // Specify color buffer
    glNamedFramebufferDrawBuffer(mFBO, GL_COLOR_ATTACHMENT0);
}

void UViewport::ResizeViewport() {
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();

    mViewportWidth = contentRegionMax.x - contentRegionMin.x;
    mViewportHeight = contentRegionMax.y - contentRegionMin.y;

    glDeleteFramebuffers(1, &mFBO);
    glDeleteTextures(2, mTexIds);

    CreateFramebuffer();
}

void UViewport::RenderUI(float deltaTime) {
    std::string name = mViewportName;

    if (name.empty()) {
        name = "Viewport##" + std::to_string(mTexIds[TEX_COLOR]);
    }

    ImGui::Begin(name.c_str(), &bIsOpen);

    ImVec2 wPos = ImGui::GetWindowPos();
    mViewportOriginX = wPos.x;
    mViewportOriginY = wPos.y;

    // This would normally be in an Update() function, but
    // the camera needs access to this ImGui window's input data.
    if (ImGui::IsWindowFocused()) {
        mCamera.Update(deltaTime);
    }

    ResizeViewport();
    ImGui::Image((void*)mTexIds[TEX_COLOR], { mViewportWidth, mViewportHeight }, { 0, 1 }, { 1, 0 });

    ImGui::End();
}

void UViewport::RenderScene(AJ3DContext* ctx, float deltaTime) {
    // Bind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glViewport(0, 0, mViewportWidth, mViewportHeight);

    glClearBufferfv(GL_COLOR, 0, COLOR_RESET);
    glClearBufferfv(GL_DEPTH, 0, &DEPTH_RESET);

    ctx->Render(mCamera, deltaTime);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

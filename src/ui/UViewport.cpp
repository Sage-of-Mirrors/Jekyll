#include "ui/UViewport.hpp"
#include "application/AJ3DContext.hpp"

#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include <imgui.h>

#include <string>


constexpr int TEX_COLOR = 0;
constexpr int TEX_DEPTH = 1;

constexpr float COLOR_RESET[] = { 0.5f, 0.5f, 0.5f, 1.0f };
constexpr float DEPTH_RESET = 1.0f;


UViewport::UViewport() {
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
    glTextureStorage2D(mTexIds[TEX_COLOR], 1, GL_RGB8, 640, 480);
    glTextureParameteri(mTexIds[TEX_COLOR], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(mTexIds[TEX_COLOR], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Generate depth texture
    glTextureStorage2D(mTexIds[TEX_DEPTH], 1, GL_DEPTH_COMPONENT32F, 640, 480);

    // Attach textures to framebuffer
    glNamedFramebufferTexture(mFBO, GL_COLOR_ATTACHMENT0, mTexIds[TEX_COLOR], 0);
    glNamedFramebufferTexture(mFBO, GL_DEPTH_ATTACHMENT, mTexIds[TEX_DEPTH], 0);

    // Specify color buffer
    glNamedFramebufferDrawBuffer(mFBO, GL_COLOR_ATTACHMENT0);
}

void UViewport::Render(float deltaTime) {
    std::string name = "Viewport##" + std::to_string(mTexIds[TEX_COLOR]);

    ImGui::Begin(name.c_str(), &bIsOpen);

    mCamera.Update(deltaTime);

    ImGui::Image((void*)mTexIds[TEX_COLOR], { 640, 480 }, { 0, 1 }, { 1, 0 });

    ImGui::End();
}

void UViewport::PostRender(AJ3DContext* ctx, float deltaTime) {
    // Bind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glClearBufferfv(GL_COLOR, 0, COLOR_RESET);
    glClearBufferfv(GL_DEPTH, 0, &DEPTH_RESET);

    ctx->Render(mCamera, deltaTime);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

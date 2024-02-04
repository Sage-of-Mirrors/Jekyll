#pragma once

#include "application/ACamera.hpp"

#include <string>


class AJ3DContext;

class UViewport {
    ASceneCamera mCamera;
    std::string mViewportName;

    uint32_t mFBO;
    uint32_t mTexIds[2];

    float mViewportWidth;
    float mViewportHeight;

    bool bIsOpen;

    void CreateFramebuffer();

    void ResizeViewport();

public:
    uint32_t mViewportOriginX;
    uint32_t mViewportOriginY;

    UViewport() : UViewport("Viewport") { }
    UViewport(std::string name);
    ~UViewport();

    bool IsOpen() const { return bIsOpen; }

    glm::vec2 GetViewportSize() const { return glm::vec2(mViewportWidth, mViewportHeight); }

    void RenderUI(float deltaTime);
    void RenderScene(AJ3DContext* ctx, float deltaTime);
};
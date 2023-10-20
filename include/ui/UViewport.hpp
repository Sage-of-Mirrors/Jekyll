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
    UViewport() : UViewport("Viewport") { }
    UViewport(std::string name);
    ~UViewport();

    bool IsOpen() const { return bIsOpen; }

    void RenderUI(float deltaTime);
    void RenderScene(AJ3DContext* ctx, float deltaTime);
};
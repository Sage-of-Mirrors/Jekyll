#pragma once

#include "application/ACamera.hpp"


class AJ3DContext;

class UViewport {
    ASceneCamera mCamera;

    uint32_t mFBO;
    uint32_t mTexIds[2];
    uint32_t mRBO;

    bool bIsOpen;

    void CreateFramebuffer();

public:
    UViewport();
    ~UViewport();

    void Render(float deltaTime);
    void PostRender(AJ3DContext* ctx, float deltaTime);
};
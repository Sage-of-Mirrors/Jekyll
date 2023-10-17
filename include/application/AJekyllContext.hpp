#pragma once

#include "ui/UScenePanel.hpp"
#include "ui/UPropertiesPanel.hpp"
#include "ui/UViewport.hpp"

#include <cstdint>
#include <string>


class MScenegraph;
class AJ3DContext;

class AJekyllContext {
    /* Panels */

    UScenePanel mScenePanel;
    UPropertiesPanel mPropertiesPanel;
    UViewport mViewport;
    UViewport mViewport2;
    
    /* Docking */
    bool bIsDockingConfigured;

    uint32_t mMainDockSpaceID;
    uint32_t mDockNodeTopID;
    uint32_t mDockNodeRightID;
    uint32_t mDockNodeDownID;
    uint32_t mDockNodeLeftID;

    /* Data */
    MScenegraph* mScenegraph;
    AJ3DContext* mJ3DContext;

    void SetUpDocking();

    void RenderMenuBar();

public:
    AJekyllContext();
    ~AJekyllContext();

    void Update(float deltaTime);
    void Render(float deltaTime);
    void PostRender(float deltaTime);

    void OnFileDropped(std::string fileName);
};

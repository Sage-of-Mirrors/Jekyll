#pragma once

#include "ui/UScenePanel.hpp"
#include "ui/UPropertiesPanel.hpp"

#include <cstdint>
#include <string>


class AJekyllContext {
    /* Panels */

    UScenePanel mScenePanel;
    UPropertiesPanel mPropertiesPanel;
    
    /* Docking */
    bool bIsDockingConfigured;

    uint32_t mMainDockSpaceID;
    uint32_t mDockNodeTopID;
    uint32_t mDockNodeRightID;
    uint32_t mDockNodeDownID;
    uint32_t mDockNodeLeftID;

    void SetUpDocking();

    void RenderMenuBar();

public:
    AJekyllContext();
    ~AJekyllContext();

    void Update(float deltaTime);
    void Render(float deltaTime);

    void OnFileDropped(std::string fileName);
};

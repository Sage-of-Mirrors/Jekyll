#pragma once

#include "ui/UScenePanel.hpp"
#include "ui/UPropertiesPanel.hpp"
#include "ui/UViewport.hpp"
#include "ui/ULightsPanel.hpp"

#include "model/MMiscModelData.hpp"

#include <glm/vec2.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>


class MScenegraph;
class MJointData;
class MShapeData;
class MTextureData;
class MMaterialData;

class AJ3DContext;

namespace bStream {
    class CStream;
}

class AJekyllContext {
    /* Panels */

    UScenePanel mScenePanel;
    UPropertiesPanel mPropertiesPanel;
    
    ULightsPanel* mLightsPanel;

    UViewport* mMainViewport;
    std::vector<UViewport*> mOtherViewports;

    glm::vec2 mAppPosition;
    
    /* Docking */
    bool bIsDockingConfigured;

    uint32_t mMainDockSpaceID;
    uint32_t mDockNodeTopID;
    uint32_t mDockNodeRightID;
    uint32_t mDockNodeDownID;
    uint32_t mDockNodeLeftID;

    /* Data */
    MMiscModelData mMiscModelData;
    MScenegraph* mScenegraph;
    MJointData* mJointData;
    MShapeData* mShapeData;
    MTextureData* mTextureData;
    MMaterialData* mMaterialData;

    AJ3DContext* mJ3DContext;

    void SetUpDocking();

    void RenderMenuBar();

    void OpenModelCB();
    void LoadModel(std::filesystem::path filePath);
    void LoadSections(bStream::CStream& stream);

    void LoadMaterialTable(std::filesystem::path filePath);
    void LoadAnimation(std::filesystem::path filePath);

    void SaveModelCB();
    void SaveModel(std::filesystem::path filePath);

public:
    AJekyllContext();
    ~AJekyllContext();

    void Update(float deltaTime);
    void Render(float deltaTime);
    void PostRender(float deltaTime);

    void OnGLInitialized();
    void OnFileDropped(std::filesystem::path filePath);

    void SetAppPosition(const int xPos, const int yPos);
};

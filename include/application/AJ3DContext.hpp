#pragma once

#include <J3D/Rendering/J3DLight.hpp>
#include <J3D/Util/J3DTransform.hpp>

#include <memory>
#include <vector>
#include <filesystem>


namespace bStream {
    class CStream;
}

struct J3DTexture;

class ASceneCamera;
class J3DModelData;
class J3DModelInstance;
class J3DMaterial;

struct AJ3DModelEntry {
    std::shared_ptr<J3DModelData> data;
    std::shared_ptr<J3DModelInstance> instance;

    J3DTransformInfo transform;
};

class AJ3DContext {
    std::vector<std::shared_ptr<AJ3DModelEntry>> mLoadedModels;

    std::weak_ptr<AJ3DModelEntry> mSelectedModel;

    J3DLight mLights[8];

public:
    AJ3DContext();
    ~AJ3DContext();

    J3DLight* GetLights() { return mLights; }
    std::vector<std::weak_ptr<J3DTexture>> GetTextures();
    std::vector<std::weak_ptr<J3DMaterial>> GetMaterials();

    void LoadModel(bStream::CStream& stream);
    void LoadMaterialTable(bStream::CStream& stream);
    void LoadAnimation(bStream::CStream& stream, const std::string& extension);

    void Render(ASceneCamera& camera, float deltaTime);

    void ToggleBmt();

    void PickQuery(uint32_t x, uint32_t y);
    void HoverQuery(glm::vec2 mousePos);
    void ResizePickingBuffer(glm::vec2 size);

    bool HasModels() const { return mLoadedModels.size() != 0; }
    void Clear() { mLoadedModels.clear(); }

    void UpdateTextureLods(float bias);
};

#pragma once

#include <J3D/Rendering/J3DLight.hpp>

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

class AJ3DContext {
    std::shared_ptr<J3DModelData> mModelData;
    std::shared_ptr<J3DModelInstance> mModelInstance;

    J3DLight mLights[8];

public:
    AJ3DContext();
    ~AJ3DContext();

    J3DLight* GetLights() { return mLights; }
    std::vector<std::shared_ptr<J3DTexture>> GetTextures();
    std::vector<std::shared_ptr<J3DMaterial>> GetMaterials();

    void LoadModel(bStream::CStream& stream);
    void LoadMaterialTable(bStream::CStream& stream);
    void LoadAnimation(bStream::CStream& stream, const std::string& extension);

    void Render(ASceneCamera& camera, float deltaTime);

    void ToggleBmt();

    void PickQuery(uint32_t x, uint32_t y);
    void HoverQuery(glm::vec2 mousePos);
    void ResizePickingBuffer(glm::vec2 size);
};

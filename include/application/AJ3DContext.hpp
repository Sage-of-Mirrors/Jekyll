#pragma once

#include <J3D/J3DLight.hpp>

#include <memory>


namespace bStream {
    class CStream;
}

class ASceneCamera;
class J3DModelData;
class J3DModelInstance;

class AJ3DContext {
    std::shared_ptr<J3DModelData> mModelData;
    std::shared_ptr<J3DModelInstance> mModelInstance;

    J3DLight mLights[8];

public:
    AJ3DContext() { }
    ~AJ3DContext() { }

    void LoadModel(bStream::CStream& stream);

    void Render(ASceneCamera& camera, float deltaTime);
};

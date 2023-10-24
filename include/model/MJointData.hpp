#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cstdint>
#include <vector>
#include <string>


class MScenegraphNode;

namespace bStream {
    class CStream;
}

enum class EJointMatrixType : uint8_t {
    Basic,
    Softimage,
    Maya
};

struct MJoint {
    // Static members

    std::string Name;

    glm::vec3 Position;
    glm::quat Rotation;
    glm::vec3 Scale;

    MJoint* Parent;
    std::vector<MJoint*> Children;


    // Editable members

    uint8_t Kind;
    EJointMatrixType MatrixType;
    bool bIgnoreParentScale;

    const char* GetName() const { return Name.data(); }
};

class MJointData {
    std::vector<MJoint*> mJoints;
    MJoint* mRoot;

    uint32_t mSize;
    uint8_t* mData;

    size_t mJointDataOffset;

    void ReadJoints();

public:
    MJointData();
    ~MJointData();

    const std::vector<MJoint*> GetJoints() const { return mJoints; }
    MJoint* GetRootJoint() const { return mRoot; }

    void LoadJointData(bStream::CStream& stream, const MScenegraphNode* sceneRoot);
    void SaveJointData(bStream::CStream& stream);
};

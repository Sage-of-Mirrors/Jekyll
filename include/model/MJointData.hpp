#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cstdint>
#include <vector>
#include <string>


class MScenegraphNode;

namespace bStream {
    class CStream;
    class CMemoryStream;
}

enum class EJointMatrixType : uint8_t {
    Basic,
    Softimage,
    Maya
};

struct MJoint {
    // Static members

    std::string mName;

    glm::vec3 mPosition;
    glm::quat mRotation;
    glm::vec3 mScale;

    MJoint* mParent;
    std::vector<MJoint*> mChildren;


    // Editable members

    uint8_t mKind;
    EJointMatrixType mMatrixType;
    bool bIgnoreParentScale;
};

class MJointData {
    std::vector<MJoint*> mJoints;
    MJoint* mRoot;

    uint32_t mSize;
    uint8_t* mData;

    void ReadJoints();

public:
    MJointData();
    ~MJointData();

    void LoadJointData(bStream::CStream& stream, const MScenegraphNode* sceneRoot);
    void SaveJointData(bStream::CStream& stream);
};

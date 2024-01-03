#pragma once

#include <J3D/Util/J3DNameTable.hpp>

#include <cstdint>
#include <string>
#include <vector>


struct MJoint;

namespace bStream {
    class CStream;
    class CMemoryStream;
}

using MMaterialShapeIdxPair = std::tuple<uint32_t, uint32_t>;

/* Represents a node read directly from the INF1 scenegraph. */
struct MSerializedNode {
    uint32_t Type;
    uint32_t Data;

    MSerializedNode() : MSerializedNode(UINT32_MAX, UINT32_MAX) { }
    MSerializedNode(uint32_t type, uint32_t data) : Type(type), Data(data) { }

    ~MSerializedNode() {
        for (MSerializedNode* node : Children) {
            delete node;
        }

        Children.clear();
    }

    MSerializedNode* Parent = nullptr;
    std::vector<MSerializedNode*> Children;
};

/* Represents a joint in the scenegraph hierarchy with all its child joints and its attached materials and shapes. */
class MScenegraphNode {
    uint32_t mJointIdx;
    std::vector<MMaterialShapeIdxPair> mMaterialShapeIdxs;

    MScenegraphNode* mParent;
    std::vector<MScenegraphNode*> mChildren;

public:
    MScenegraphNode() : mJointIdx(UINT32_MAX), mParent(nullptr) { }

    ~MScenegraphNode() {
        for (MScenegraphNode* child : mChildren) {
            delete child;
        }

        mChildren.clear();
    }

    uint32_t GetJointIndex() const { return mJointIdx; }

    /* Flattens the given MSerializedNode's children into material/shape pairs and child joint MScenegraphNodes. */
    void ProcessSerializedNodes(MSerializedNode* jntNode);
    void ProcessSerializedNodes_Recursive(MSerializedNode* node);

    /* Renders this node onto the scenegraph window. */
    void RenderUI_Recursive(const J3DNameTable* jntNames, const J3DNameTable* matNames, int level) const;

    void SetupJointHierarchy_Recursive(std::vector<MJoint*>& joints) const;
};

class MScenegraph {
    MScenegraphNode* mRoot;

    uint16_t mFlags;

    uint32_t mSize;
    uint8_t* mData;

    size_t LoadData(bStream::CStream& stream);

    MSerializedNode* LoadSerializedNodes(bStream::CStream& stream);

public:
    MScenegraph();
    ~MScenegraph();

    void LoadScenegraphData(bStream::CStream& stream);
    void SetMatrixType(uint16_t type);

    void SaveScenegraph(bStream::CStream& stream);

    const MScenegraphNode* GetRoot() const { return mRoot; }
};

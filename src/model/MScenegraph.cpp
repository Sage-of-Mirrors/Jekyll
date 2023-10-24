#include "model/MScenegraph.hpp"
#include "model/MJointData.hpp"

#include <bstream.h>
#include <imgui.h>

#include <string>
#include <tuple>


constexpr size_t SIZE_OFFSET = 0x04;
constexpr size_t FLAGS_OFFSET = 0x08;
constexpr size_t HIERARCHY_OFFSET = 0x14;

constexpr uint16_t NODE_ID_NONE = 0x00;
constexpr uint16_t NODE_ID_DOWN = 0x01;
constexpr uint16_t NODE_ID_UP = 0x02;
constexpr uint16_t NODE_ID_JOINT = 0x10;
constexpr uint16_t NODE_ID_MATERIAL = 0x11;
constexpr uint16_t NODE_ID_SHAPE = 0x12;

constexpr uint16_t MATRIX_TYPE_MASK = 0x000F;

constexpr float TREE_INDENT = 0.6f;


/* MScenegraph */

MScenegraph::MScenegraph() : mRoot(nullptr), mFlags(0), mSize(0), mData(nullptr) {

}

MScenegraph::~MScenegraph() {
    delete[] mData;
    delete mRoot;
}

size_t MScenegraph::LoadData(bStream::CStream& stream) {
    size_t curPos = stream.tell();

    mFlags = stream.peekUInt16(curPos + FLAGS_OFFSET);
    mSize = stream.peekUInt32(curPos + SIZE_OFFSET);

    mData = new uint8_t[mSize];
    stream.readBytesTo(mData, mSize);

    return curPos;
}

MSerializedNode* MScenegraph::LoadSerializedNodes(bStream::CStream& stream) {
    uint16_t nodeId = stream.readUInt16();
    uint16_t nodeData = stream.readUInt16();

    MSerializedNode* curParent = nullptr;
    MSerializedNode* lastNode = nullptr;

    // Read the hierarchy data and generate a tree structure
    while (nodeId != NODE_ID_NONE) {
        switch (nodeId) {
            case NODE_ID_DOWN:
            {
                curParent = lastNode;
                break;
            }
            case NODE_ID_UP:
            {
                if (curParent != nullptr && curParent->Parent != nullptr) {
                    curParent = curParent->Parent;
                }

                break;
            }
            case NODE_ID_JOINT:
            case NODE_ID_MATERIAL:
            case NODE_ID_SHAPE:
            {
                MSerializedNode* newNode = new MSerializedNode(nodeId, nodeData);

                if (curParent != nullptr) {
                    curParent->Children.push_back(newNode);
                }

                newNode->Parent = curParent;
                lastNode = newNode;

                break;
            }
            default:
            {
                std::cout << "Unknown INF1 scenegraph type \"" << nodeId << "\"!" << std::endl;
                break;
            }
        }

        nodeId = stream.readUInt16();
        nodeData = stream.readUInt16();
    }

    if (curParent == nullptr) {
        std::cout << "Error while parsing INF1 scenegraph!" << std::endl;
        return nullptr;
    }

    return curParent;
}

void MScenegraph::LoadScenegraphData(bStream::CStream& stream) {
    size_t curPos = LoadData(stream);

    size_t hierarchyOffset = stream.peekUInt32(curPos + HIERARCHY_OFFSET);
    stream.seek(curPos + hierarchyOffset);

    MSerializedNode* serializedRoot = LoadSerializedNodes(stream);

    mRoot = new MScenegraphNode();
    mRoot->ProcessSerializedNodes(serializedRoot);

    delete serializedRoot;

    stream.seek(curPos + mSize);
}

void MScenegraph::SetMatrixType(uint16_t type) {
    mFlags &= ~MATRIX_TYPE_MASK;
    mFlags |= type;
}

void MScenegraph::SaveScenegraph(bStream::CStream& stream) {
    size_t curPos = stream.tell();

    stream.writeBytes(mData, mSize);

    stream.seek(curPos + FLAGS_OFFSET);
    stream.writeUInt16(mFlags);

    stream.seek(curPos + mSize);
}


/* MScenegraphNode */

void MScenegraphNode::ProcessSerializedNodes(MSerializedNode* jntNode) {
    mJointIdx = jntNode->Data;

    ProcessSerializedNodes_Recursive(jntNode);
}

void MScenegraphNode::ProcessSerializedNodes_Recursive(MSerializedNode* node) {
    for (MSerializedNode* child : node->Children) {
        switch (child->Type) {
            case NODE_ID_JOINT:
            {
                MScenegraphNode* childNode = new MScenegraphNode();
                childNode->ProcessSerializedNodes(child);

                childNode->mParent = this;
                mChildren.push_back(childNode);

                break;
            }
            case NODE_ID_MATERIAL:
            {
                mMaterialShapeIdxs.push_back({ child->Data, child->Children[0]->Data });
                ProcessSerializedNodes_Recursive(child);

                break;
            }
            case NODE_ID_SHAPE:
            {
                ProcessSerializedNodes_Recursive(child);
                break;
            }
            default:
            {
                std::cout << "Error processing serialized nodes!" << std::endl;
                break;
            }
        }
    }
}

void MScenegraphNode::RenderUI_Recursive(const J3DNameTable* jntNames, const J3DNameTable* matNames, int level) const {
    std::string nodeId = "##jnt" + std::to_string(mJointIdx);

    if (mChildren.size() == 0) {
        ImGui::Text("Joint %i", mJointIdx);
        return;
    }

    if (ImGui::TreeNode(nodeId.c_str(), "Joint %i", mJointIdx)) {
        ImGui::Indent(TREE_INDENT * level);

        for (const MScenegraphNode* child : mChildren) {
            child->RenderUI_Recursive(jntNames, matNames, level + 1);
        }

        for (MMaterialShapeIdxPair p : mMaterialShapeIdxs) {
            ImGui::Text("Material: %i, Shape: %i", std::get<0>(p), std::get<1>(p));
        }

        ImGui::Unindent(TREE_INDENT * level);
        ImGui::TreePop();
    }
}

void MScenegraphNode::SetupJointHierarchy_Recursive(std::vector<MJoint*>& joints) const {
    if (mParent == nullptr) {
        joints[mJointIdx]->Parent = nullptr;
    }
    else {
        joints[mJointIdx]->Parent = joints[mParent->mJointIdx];
    }

    for (MScenegraphNode* child : mChildren) {
        joints[mJointIdx]->Children.push_back(joints[child->mJointIdx]);

        child->SetupJointHierarchy_Recursive(joints);
    }
}

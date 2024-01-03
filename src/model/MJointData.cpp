#include "model/MJointData.hpp"
#include "model/MScenegraph.hpp"

#include <J3D/Util/J3DNameTable.hpp>
#include <bstream.h>


constexpr uint32_t JNT1_ID = 0x4A4E5431;

constexpr size_t SIZE_OFFSET = 0x04;
constexpr size_t JNT_COUNT_OFFSET = 0x08;
constexpr size_t JNT_DATA_OFFSET = 0x0C;
constexpr size_t NAME_TABLE_OFFSET = 0x14;

MJointData::MJointData() : mRoot(nullptr), mSize(0), mData(nullptr), mJointDataOffset(0) {

}

MJointData::~MJointData() {
    for (MJoint* jnt : mJoints) {
        delete jnt;
    }

    mJoints.clear();
    mRoot = nullptr;

    mSize = 0;

    delete[] mData;
    mData = nullptr;
}

void MJointData::LoadJointData(bStream::CStream& stream, const MScenegraphNode* sceneRoot) {
    size_t curPos = stream.tell();

    // Sanity check, ensure we're reading JNT1.
    if (stream.peekUInt32(curPos) != JNT1_ID) {
        return;
    }

    // Make a copy of the entire JNT1 section.
    mSize = stream.peekUInt32(curPos + SIZE_OFFSET);
    mData = new uint8_t[mSize];

    stream.readBytesTo(mData, mSize);

    ReadJoints();
    sceneRoot->SetupJointHierarchy_Recursive(mJoints);

    mRoot = mJoints[sceneRoot->GetJointIndex()];
}

void MJointData::ReadJoints() {
    bStream::CMemoryStream dataStrm(mData, mSize, bStream::Big, bStream::In);

    // Read name table
    uint32_t nameTableOffset = dataStrm.peekUInt32(NAME_TABLE_OFFSET);
    dataStrm.seek(nameTableOffset);

    J3DNameTable jntNameTable;
    jntNameTable.Deserialize(&dataStrm);

    // Read joints
    uint16_t jntCount = dataStrm.peekUInt16(JNT_COUNT_OFFSET);
    mJointDataOffset = dataStrm.peekUInt32(JNT_DATA_OFFSET);

    dataStrm.seek(mJointDataOffset);

    for (uint32_t i = 0; i < jntCount; i++) {
        MJoint* newJnt = new MJoint();
        newJnt->Name = jntNameTable.GetName(i);

        uint16_t flags = dataStrm.readUInt16();
        newJnt->Kind = flags & 0x0F;
        newJnt->MatrixType = static_cast<EJointMatrixType>((flags & 0xF0) >> 4);

        uint8_t ignoreParentScale = dataStrm.readUInt8();
        newJnt->bIgnoreParentScale = ignoreParentScale == 0xFF ? false : ignoreParentScale;

        dataStrm.skip(1);

        newJnt->Scale.x = dataStrm.readFloat();
        newJnt->Scale.y = dataStrm.readFloat();
        newJnt->Scale.z = dataStrm.readFloat();

        glm::vec3 eulerAngles;
        eulerAngles.x = dataStrm.readUInt16() * (180.0f / 65535.0f);
        eulerAngles.y = dataStrm.readUInt16() * (180.0f / 65535.0f);
        eulerAngles.z = dataStrm.readUInt16() * (180.0f / 65535.0f);
        dataStrm.skip(2);

        newJnt->Rotation = glm::quat(eulerAngles);

        newJnt->Position.x = dataStrm.readFloat();
        newJnt->Position.y = dataStrm.readFloat();
        newJnt->Position.z = dataStrm.readFloat();

        dataStrm.skip(0x1C);

        mJoints.push_back(newJnt);
    }
}

void MJointData::SaveJointData(bStream::CStream& stream) {
    size_t curPos = stream.tell();

    stream.writeBytes(mData, mSize);
    size_t endPos = stream.tell();

    stream.seek(curPos + mJointDataOffset);

    for (MJoint* jnt : mJoints) {
        uint16_t flags = jnt->Kind;
        flags |= (static_cast<uint8_t>(jnt->MatrixType) & 0x0F) << 4;

        stream.writeUInt16(flags);
        stream.writeUInt8(jnt->bIgnoreParentScale);

        stream.skip(0x3D);
    }

    stream.seek(endPos);
}

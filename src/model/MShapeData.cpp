#include "model/MShapeData.hpp"

#include <bstream.h>


constexpr uint32_t SHP1_ID = 0x53485031;

constexpr size_t SIZE_OFFSET = 0x04;
constexpr size_t SHP_COUNT_OFFSET = 0x08;
constexpr size_t SHP_DATA_OFFSET = 0x0C;

MShapeData::MShapeData() : mSize(0), mData(nullptr), mShapeDataOffset(0) {

}

MShapeData::~MShapeData() {
    for (MShape* shape : mShapes) {
        delete shape;
    }

    mShapes.clear();
    mSize = 0;

    delete[] mData;
    mData = nullptr;
}

void MShapeData::LoadShapeData(bStream::CStream& stream) {
    size_t curPos = stream.tell();

    // Sanity check, ensure we're reading SHP1.
    if (stream.peekUInt32(curPos) != SHP1_ID) {
        return;
    }

    // Make a copy of the entire SHP1 section.
    mSize = stream.peekUInt32(curPos + SIZE_OFFSET);
    mData = new uint8_t[mSize];

    stream.readBytesTo(mData, mSize);

    ReadShapes();
}

void MShapeData::ReadShapes() {
    bStream::CMemoryStream dataStrm(mData, mSize, bStream::Big, bStream::In);

    uint16_t shpCount = dataStrm.peekUInt16(SHP_COUNT_OFFSET);
    mShapeDataOffset = dataStrm.peekUInt32(SHP_DATA_OFFSET);

    dataStrm.seek(mShapeDataOffset);

    for (uint32_t i = 0; i < shpCount; i++) {
        MShape* newShape = new MShape();
        newShape->MatrixType = static_cast<EShapeMatrixType>(dataStrm.readUInt8());

        dataStrm.skip(0x0B);

        newShape->BoundingSphereRadius = dataStrm.readFloat();

        newShape->BoundingBoxMin.x = dataStrm.readFloat();
        newShape->BoundingBoxMin.y = dataStrm.readFloat();
        newShape->BoundingBoxMin.z = dataStrm.readFloat();

        newShape->BoundingBoxMax.x = dataStrm.readFloat();
        newShape->BoundingBoxMax.y = dataStrm.readFloat();
        newShape->BoundingBoxMax.z = dataStrm.readFloat();

        mShapes.push_back(newShape);
    }
}

void MShapeData::SaveShapeData(bStream::CStream& stream) {
    size_t curPos = stream.tell();

    stream.writeBytes(mData, mSize);
    size_t endPos = stream.tell();

    stream.seek(curPos + mShapeDataOffset);

    for (MShape* shape : mShapes) {
        stream.writeUInt8(static_cast<uint8_t>(shape->MatrixType));
        stream.skip(0x27);
    }

    stream.seek(endPos);
}

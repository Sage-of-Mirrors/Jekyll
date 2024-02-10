#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <vector>


namespace bStream {
    class CStream;
}

enum class EShapeMatrixType : uint8_t {
    Matrix,
    Billboard,
    Y_Billboard,
    Multi
};

struct MShape {
    // Static members

    uint32_t MaterialIndex;

    float BoundingSphereRadius;
    glm::vec3 BoundingBoxMin;
    glm::vec3 BoundingBoxMax;


    // Editable members

    EShapeMatrixType MatrixType;
};

class MShapeData {
    std::vector<MShape*> mShapes;

    uint32_t mSize;
    uint8_t* mData;

    size_t mShapeDataOffset;

    void ReadShapes();

public:
    MShapeData();
    ~MShapeData();

    const std::vector<MShape*> GetShapes() const { return mShapes; }

    void LoadShapeData(bStream::CStream& stream);
    void SaveShapeData(bStream::CStream& stream);

    void Clear();
};

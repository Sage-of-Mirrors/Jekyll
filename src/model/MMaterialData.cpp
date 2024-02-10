#include "model/MMaterialData.hpp"

#include <J3D/Util/J3DNameTable.hpp>
#include <J3D/Texture/J3DTexture.hpp>

#include <bstream.h>
#include <decode.h>


constexpr uint32_t TEX1_ID = 0x54455831;

constexpr size_t SIZE_OFFSET = 0x04;
constexpr size_t TEX_COUNT_OFFSET = 0x08;
constexpr size_t TEX_DATA_OFFSET = 0x0C;
constexpr size_t NAME_TABLE_OFFSET = 0x10;

MMaterialData::MMaterialData() : mSelectedMaterial() {
}

MMaterialData::~MMaterialData() {
    Clear();
}

void MMaterialData::SaveMaterialData(bStream::CStream& stream) {

}

void MMaterialData::Clear() {
    mMaterials.clear();
}
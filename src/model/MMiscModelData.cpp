#include "model/MMiscModelData.hpp"


MMiscModelData::MMiscModelData() : mHeaderData(nullptr) {

}

MMiscModelData::~MMiscModelData() {
    delete[] mHeaderData;

    for (auto& [id, data] : mSectionData) {
        delete[] data;
    }

    mSectionSizes.clear();
    mSectionData.clear();
}

void MMiscModelData::AddSection(uint32_t id, uint32_t size, uint8_t* data) {
    mSectionSizes[id] = size;
    mSectionData[id] = data;
}

void MMiscModelData::GetSection(uint32_t id, uint32_t& size, uint8_t*& data) {
    if (mSectionSizes.find(id) == mSectionSizes.end()) {
        size = 0;
        data = nullptr;

        return;
    }

    size = mSectionSizes[id];
    data = mSectionData[id];
}

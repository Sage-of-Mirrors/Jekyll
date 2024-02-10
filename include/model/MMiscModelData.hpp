#pragma once

#include <cstdint>
#include <map>


using MSectionIdSizes = std::map<uint32_t, uint32_t>;
using MSectionIdData = std::map<uint32_t, uint8_t*>;

class MMiscModelData {
    MSectionIdSizes mSectionSizes;
    MSectionIdData mSectionData;

    uint8_t* mHeaderData;

public:
    MMiscModelData();
    ~MMiscModelData();

    void AddSection(uint32_t id, uint32_t size, uint8_t* data);
    void GetSection(uint32_t id, uint32_t& size, uint8_t*& data);

    void SetHeader(uint8_t* data) { mHeaderData = data; }
    uint8_t* GetHeader() const { return mHeaderData; }

    void Clear();
};

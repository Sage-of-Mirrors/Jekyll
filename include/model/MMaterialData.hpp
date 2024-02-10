#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>


class J3DMaterial;

namespace bStream {
    class CStream;
}

class MMaterialData {
    std::vector<std::weak_ptr<J3DMaterial>> mMaterials;
    std::weak_ptr<J3DMaterial> mSelectedMaterial;

public:
    MMaterialData();
    ~MMaterialData();

    void SetMaterialData(std::vector<std::weak_ptr<J3DMaterial>>& materials) { mMaterials = materials; }
    const std::vector<std::weak_ptr<J3DMaterial>>& GetMaterials() const { return mMaterials; }

    void SaveMaterialData(bStream::CStream& stream);

    void Clear();
};
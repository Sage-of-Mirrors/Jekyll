#pragma once

#include "ui/properties/UPropertiesTabBase.hpp"

#include <memory>
#include <vector>


struct J3DTexture;
class MTextureData;

class UTexturePropertiesTab : public UPropertiesTabBase {
    MTextureData* mTextureData;
    std::weak_ptr<J3DTexture> mSelectedTexture;

    std::vector<uint32_t> mSelectedTextureViews;

    void OnSelectedTextureChanged();

public:
    UTexturePropertiesTab();
    UTexturePropertiesTab(MTextureData* texData);

    ~UTexturePropertiesTab();

    virtual void Render() override;
};

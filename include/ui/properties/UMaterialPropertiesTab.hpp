#pragma once

#include "ui/properties/UPropertiesTabBase.hpp"

#include <memory>
#include <vector>


class J3DMaterial;
class MMaterialData;

class UMaterialPropertiesTab : public UPropertiesTabBase {
    MMaterialData* mMaterialData;
    std::weak_ptr<J3DMaterial> mSelectedMaterial;

    bool RenderBlendModeControls(std::shared_ptr<J3DMaterial> material);
    bool RenderAlphaCompareControls(std::shared_ptr<J3DMaterial> material);
    bool RenderZModeControls(std::shared_ptr<J3DMaterial> material);

public:
    UMaterialPropertiesTab();
    UMaterialPropertiesTab(MMaterialData* matData);

    ~UMaterialPropertiesTab();

    virtual void Render() override;
};
#pragma once

#include "ui/properties/UPropertiesTabBase.hpp"

#include <string>


class MShape;
class MShapeData;

class UShapePropertiesTab : public UPropertiesTabBase {
    MShapeData* mShapeData;
    MShape* mCurrentShape;

public:
    UShapePropertiesTab();
    UShapePropertiesTab(MShapeData* jointData);

    ~UShapePropertiesTab();

    virtual void Render() override;
};

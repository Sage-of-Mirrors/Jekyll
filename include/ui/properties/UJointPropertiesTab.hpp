#pragma once

#include "ui/properties/UPropertiesTabBase.hpp"

#include <string>


struct MJoint;
class MJointData;

class UJointPropertiesTab : public UPropertiesTabBase {
    MJointData* mJointData;
    MJoint* mSelectedJoint;

    void RenderJoints_Recursive(MJoint* jnt);

public:
    UJointPropertiesTab();
    UJointPropertiesTab(MJointData* jointData);

    ~UJointPropertiesTab();

    virtual void Render() override;
};

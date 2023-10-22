#pragma once

#include "ui/properties/UPropertiesTabBase.hpp"

#include <string>


class MJoint;
class MJointData;

class UJointPropertiesTab : public UPropertiesTabBase {
    MJointData* mJointData;

    void RenderJoints_Recursive(MJoint* jnt);

public:
    UJointPropertiesTab();
    UJointPropertiesTab(MJointData* jointData);

    ~UJointPropertiesTab();

    virtual void Render() override;
};

#pragma once

#include <string>


class UPropertiesTabBase {
protected:
    std::string mName;
    std::string mIcon;
    std::string mToolTip;

public:
    UPropertiesTabBase() : mName(""), mIcon(""), mToolTip("") { }
    ~UPropertiesTabBase() { }

    const char* GetName() const { return mName.data(); }
    const char* GetIcon() const { return mIcon.data(); }
    const char* GetTooltip() const { return mToolTip.data(); }

    virtual void Render() = 0;
};

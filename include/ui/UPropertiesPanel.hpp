#pragma once

#include <string>
#include <vector>


class UPropertiesTabBase;

class UPropertiesPanel {
    std::vector<UPropertiesTabBase*> mTabs;
    UPropertiesTabBase* mCurrentTab;

    void RenderTabBg();
    void RenderTabs();

    void RenderContent();

public:
    UPropertiesPanel();
    ~UPropertiesPanel();

    void AddTab(UPropertiesTabBase* tab) { mTabs.push_back(tab); }

    void Render();
};

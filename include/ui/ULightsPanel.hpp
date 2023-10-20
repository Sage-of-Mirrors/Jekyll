#pragma once


class AJ3DContext;
struct J3DLight;

class ULightsPanel {
    AJ3DContext* mJ3DContext;

    bool bIsOpen;

    void RenderLightControl(J3DLight& light, int index);

public:
    ULightsPanel() : ULightsPanel(nullptr) { }
    ULightsPanel(AJ3DContext* ctx) : mJ3DContext(ctx), bIsOpen(true) { }

    ~ULightsPanel() { }

    bool IsOpen() const { return bIsOpen; }

    void Render();
};

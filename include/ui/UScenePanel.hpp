#pragma once


class MScenegraph;
class MScenegraphNode;

class UScenePanel {
    void RenderScenegraphTree(MScenegraph* scenegraph);
    void RenderScenegraphNode_Recursive(const MScenegraphNode* node);

public:
    UScenePanel();
    ~UScenePanel();

    void Render(MScenegraph* scenegraph);
};

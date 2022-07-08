#pragma once

#include "ComponentGUI.h"
//#include <Engine/CCamera.h>

class TransformGUI :
    public ComponentGUI
{
    //CCamera* m_toolcam;

private:
    float m_arrData[3][3];

public:
    void EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition);
    virtual void init();
    virtual void lateupdate();
    virtual void render();


public:
    TransformGUI();
    ~TransformGUI();


};


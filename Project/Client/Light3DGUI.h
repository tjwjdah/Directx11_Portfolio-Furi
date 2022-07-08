#pragma once
#include "ComponentGUI.h"

class Light3DGUI :
    public ComponentGUI
{
private:
    int     m_iLightTypeIdx;

public:
    virtual void lateupdate();
    virtual void render();

public:
    Light3DGUI();
    ~Light3DGUI();
};


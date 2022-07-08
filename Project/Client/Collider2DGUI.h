#pragma once
#include "ComponentGUI.h"

class Collider2DGUI :
    public ComponentGUI
{
private:
    int     m_iColliderTypeIdx;

public:
    virtual void lateupdate();
    virtual void render();

    Collider2DGUI();
    ~Collider2DGUI();
};


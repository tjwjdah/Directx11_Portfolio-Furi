#pragma once
#include "ComponentGUI.h"
#include <Engine/CAnimator3D.h>

class Animator3DGUI :
    public ComponentGUI
{
private:
    void ShowExampleAppLayout(bool* p_open, CAnimator3D* _pani);

public:
    virtual void lateupdate();
    virtual void render();

    Animator3DGUI();
    ~Animator3DGUI();
};


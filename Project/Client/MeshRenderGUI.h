#pragma once
#include "ComponentGUI.h"

class ListGUI;

class MeshRenderGUI :
    public ComponentGUI
{
public:    
    virtual void update();
    virtual void render();

private:
    GUI* PopupListGUI(int i = 0);
    void SelectListItem(ListGUI* _pListGUI, const char* _pSelectName, int i = 0);



public:
    MeshRenderGUI();
    ~MeshRenderGUI();
};


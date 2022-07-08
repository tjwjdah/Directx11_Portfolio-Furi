#pragma once
#include "ResInfoGUI.h"

class ListGUI;

class MaterialGUI :
    public ResInfoGUI
{
private:
    SHADER_PARAM  m_eSelectParam;

public:
    virtual void render();

private:
    void SelectShader(ListGUI* _pGUI, const char* _strSelect);
    void SelectTexture(ListGUI* _pGUI, const char* _strSelect);
    void render_shaderparam();

public:
    MaterialGUI();
    virtual ~MaterialGUI();
};


#pragma once
#include "ComponentGUI.h"

class CScript;

class ScriptGUI :
    public ComponentGUI
{
private:
    CScript* m_pTargetScript;

public:
    void SetScript(CScript* _pScript){m_pTargetScript = _pScript;}

public:
    virtual void update();
    virtual void render();

public:
    ScriptGUI();
    virtual ~ScriptGUI();

};


#pragma once
#include "GUI.h"

class CGameObject;

class ComponentGUI :
    public GUI
{
private:
    CGameObject*    m_pTargetObj;
    COMPONENT_TYPE  m_eCT;

protected:
    Vec2            m_vSize;

public:
    void SetSize(Vec2 _vSize){m_vSize = _vSize;}


public:
    virtual void update();

    void SetTargetObj(CGameObject* _pTarget){m_pTargetObj = _pTarget;}
    CGameObject* GetTargetObj() { return m_pTargetObj; }

protected:
    void Start();
    void End();

public:
    ComponentGUI(COMPONENT_TYPE _eType);
    ~ComponentGUI();
};


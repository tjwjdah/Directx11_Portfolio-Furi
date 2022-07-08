#pragma once
#include "GUI.h"

class CRes;

class ResInfoGUI :
    public GUI
{
private:
    CRes*   m_pTargetRes;

public:
    void SetTargetRes(CRes* _pTargetRes) { m_pTargetRes = _pTargetRes; }
    CRes* GetTargetRes() { return m_pTargetRes; }

    void Start();
    void End();


public:
    ResInfoGUI();
    virtual ~ResInfoGUI();
};


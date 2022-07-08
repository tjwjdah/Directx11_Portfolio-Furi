#pragma once
#include "CCamera.h"
class CCamera;

class CUICam :
    public CCamera
{

private:
    map<ULONG64, vector<tInstObj>>		m_mapInstGroup_ui;

public:
    void SortUIObject();
    virtual void render_ui();

    virtual void finalupdate();

public:
    CUICam();
    ~CUICam();
};

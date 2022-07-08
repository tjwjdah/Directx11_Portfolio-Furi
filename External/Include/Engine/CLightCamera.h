#pragma once
#include "CCamera.h"
class CCamera;

class CLightCamera :
    public CCamera
{
private:   
    map<ULONG64, vector<tInstObj>>		m_mapInstGroup_S;	// Shadow

public:
    virtual void finalupdate();

public:
    void SortShadowObject();
    void render_shadowmap();

public:
    CLightCamera();
    ~CLightCamera();
};


#pragma once
#include <Engine\CScript.h>

class CToolCamScript :
    public CScript
{
private:
    float       m_fSpeed;

public:
    void update();    

public:
    CLONE(CToolCamScript);

public:
    CToolCamScript();
    ~CToolCamScript();
};


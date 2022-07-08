#pragma once
#include <Engine\CScript.h>

class CCameraScript :
    public CScript
{
private:

    float       m_fSpeed;
public:
    virtual void update();

public:
    CLONE(CCameraScript);

public:
    CCameraScript();
    ~CCameraScript();
};


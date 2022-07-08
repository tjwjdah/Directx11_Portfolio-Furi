#pragma once
#include <Engine\CScript.h>

class CWallScript :
    public CScript
{
private:
    Ptr<CPrefab> m_Bullet;

    int     m_iHP;

public:
    virtual void awake();
    virtual void update();

public:

public:
    CLONE(CWallScript);
    virtual void OnCollisionEnter(CGameObject* _pOther);
    //virtual void OnCollision(CGameObject* _pOther);

public:
    CWallScript();
    ~CWallScript();
};


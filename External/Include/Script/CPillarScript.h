#pragma once
#include <Engine\CScript.h>

class CPillarScript :
    public CScript
{
private:
    Ptr<CPrefab>    m_Yellow;

    int     m_iHP;

    float   m_fShootTimeMax;
    float   m_fShootTime;

    bool    m_bAwake;
    bool    m_bType;

public:
    virtual void update();

public:
    void SetType(bool _bType) { m_bType = _bType; }

public:
    CLONE(CPillarScript);
    virtual void OnCollisionEnter(CGameObject* _pOther);
    //virtual void OnCollision(CGameObject* _pOther);

public:
    CPillarScript();
    ~CPillarScript();
};

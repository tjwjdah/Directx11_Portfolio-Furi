#pragma once
#include <Engine\CScript.h>

class CMissileScript2 :
    public CScript
{
private:
    Ptr<CPrefab> m_Yellow;

    Vec3    m_vPos;

    float   m_fDeadTime;
    float   m_fDeadTimeMax;

    bool    m_bDead;

    bool    m_bAwake;

public:
    virtual void update();

public:
    void SetPos(Vec3 _vPos) { m_vPos = _vPos; }
public:
    CLONE(CMissileScript2);
    virtual void OnCollisionEnter(CGameObject* _pOther);

public:
    CMissileScript2();
    ~CMissileScript2();
};


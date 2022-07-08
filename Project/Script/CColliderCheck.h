#pragma once
#include <Engine\CScript.h>
class CColliderCheck :
    public CScript
{
private:
    float       m_fAngleStart;
    float       m_fAngleEnd;
public:
    virtual void update();

public:
    CLONE(CColliderCheck);

public:
    CColliderCheck();
    ~CColliderCheck();

public:
    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);
};


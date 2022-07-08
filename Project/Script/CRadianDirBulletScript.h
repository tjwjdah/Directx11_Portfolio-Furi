#pragma once
#include <Engine\CScript.h>

class CPrefab;
class CRadianDirBulletScript :
    public CScript
{
private:
    float m_fAtime;
    float m_fMaxtime;

    float   m_fSpeed;
    float   m_fRadian; //°¢µµ

    float   m_fAngle;


public:
    virtual void update();
    virtual void awake();

public:
    void SetSpeed(float _Speed) { m_fSpeed = _Speed; }
    void SetRadian(float _Radian) { m_fRadian = _Radian; };

    void SetMaxTime(float _MaxTime) { m_fMaxtime = _MaxTime; };


    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);
public:
    CLONE(CRadianDirBulletScript);

public:
    CRadianDirBulletScript();
    ~CRadianDirBulletScript();

};


#pragma once
#include <Engine\CScript.h>

class CPrefab;
class CYoyoScript :
    public CScript
{
private:

    Vec3 m_vStartPos;
    Vec3 m_vPos;
    Vec3  m_TarPos;

    Vec2 m_dir;

    float m_fSpeed;
    float m_Dis;

    bool m_bflag;
    bool m_bBack;

public:
    void SetDis(float _dis) { m_Dis = _dis; }
    void SetSpeed(float _Speed) { m_fSpeed = _Speed; }
    void SetTarPos(Vec3 _tarpos) { m_TarPos = _tarpos; }

public:
    virtual void update();
    virtual void awake();

public:
    CLONE(CYoyoScript);

public:
    CYoyoScript();
    ~CYoyoScript();
    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);
};


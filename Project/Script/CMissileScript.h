#pragma once
#include <Engine\CScript.h>

class CMissileScript :
    public CScript
{
private:
    Vec3    m_vPos;
    Vec3    m_vDir;
    float   m_fSpeed;

    float   m_fDeadTime;
    float   m_fDeadTimeMax;

    bool    m_bDead;
    bool    m_bBullet;
public:
    virtual void update();

public:
    void SetPos(Vec3 _vPos) { m_vPos = _vPos; }
    void SetDir(Vec3 _vDir) { m_vDir = _vDir; }
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }
    void SetTime(float _fTime) { m_fDeadTimeMax = _fTime; }
    void SetBullet(bool _bBullet) { m_bBullet = _bBullet; }
    void SetScale(Vec3 _vScale) { Transform()->SetLocalScale(_vScale); }

    Vec3 GetDir() { return m_vDir; }
    bool GetDead() { return m_bDead; }

public:
    CLONE(CMissileScript);
    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);

public:
    CMissileScript();
    ~CMissileScript();
};


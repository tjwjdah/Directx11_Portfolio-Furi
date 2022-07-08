#pragma once
#include <Engine\CScript.h>

class CStructuredBuffer;

class CMissileScript3 :
    public CScript
{
private:
    Vec3    m_vScale;
    float   m_fSpeed;
    float   m_fReduce;

    float   m_fDeadTime;
    float   m_fDeadTimeMax;

    bool    b_bAwake;

    CStructuredBuffer* m_pSectorBuffer;
    tSectorInfo m_tSectorInfo;

public:
    virtual void update();

public:
    void SetPos(Vec3 _vPos) { Transform()->SetLocalPos(_vPos); }
    void SetScale(Vec3 _vScale) { m_vScale = _vScale; }
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }
    void SetTime(float _fTime) { m_fDeadTimeMax = _fTime; }

    void SetSectorInfo(float _fAngle, float _fDistance, float _fReduce)
    {
        m_tSectorInfo.fAngle = _fAngle;
        m_tSectorInfo.fDistance = _fDistance;
        m_fReduce = _fReduce;
    }

public:
    CLONE(CMissileScript3);
    virtual void UpdateData();
    virtual void ClearData();

    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);

public:
    CMissileScript3();
    ~CMissileScript3();
};


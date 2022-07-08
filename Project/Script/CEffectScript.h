#pragma once
#include <Engine\CScript.h>

class CGameObject;

enum class EFFECT
{
    DASH,
    TELPO_UP,
    TELPO_DOWN,
    HIT,
    ROT,
    SCALE,
    NONE
};

class CEffectScript :
    public CScript
{
private:
    EFFECT  m_Type;

    Vec3    m_vPos;
    Vec3    m_vBaseScale;
    Vec3    m_vDir;
    Vec3    m_vDir2;
    float   m_fSpeed;

    float   m_fDeadTime;
    float   m_fDeadTimeMax;
    float   m_fStopParTime;

    float   m_ScaleMaxTime;
    Vec3   m_AddScale;
    float   m_ScaleTime;

    bool    m_bAwake;
    bool    m_bDead;

    float m_RotScale;
    bool    m_bROT;
    //외부 변수
    CGameObject* m_pPlayer;

public:
    virtual void update();

public:
    void SetType(EFFECT _Type) { m_Type = _Type; }
    void SetPos(Vec3 _vPos) { m_vPos = _vPos;  }
    void SetDir(Vec3 _vDir) { m_vDir = _vDir; }
    void SetDir2(Vec3 _vDir) { m_vDir2 = _vDir; }
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }
    void SetTime(float _fTime) { m_fDeadTimeMax = _fTime; }
    void SetStopTime(float _fTime) { m_fStopParTime = _fTime; }

    void SetAddScale(Vec3 _Scale) { m_AddScale = _Scale; }
    void SetScaleTime(float _Time) { m_ScaleMaxTime = _Time; }
    void SetScale(Vec3 _vScale) { m_vBaseScale = _vScale; }

    void SetbRot(bool _Rot) { m_bROT = _Rot; }
    Vec3 GetDir() { return m_vDir; }
    Vec3 GetDir2() { return m_vDir2; }
    bool GetDead() { return m_bDead; }
    void SetRotScale(float _Scale) { m_RotScale = _Scale; }
    
public:
    CLONE(CEffectScript);
    virtual void OnCollisionEnter(CGameObject* _pOther) {}
    virtual void OnCollision(CGameObject* _pOther) {}

public:
    CEffectScript();
    ~CEffectScript();
};
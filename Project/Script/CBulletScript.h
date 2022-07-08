#pragma once
#include <Engine\CScript.h>

class CPrefab;
class CBulletScript :
    public CScript
{
private:
    float m_fAtime;
    float m_fMaxtime;

    float   m_fSpeed;
    float   m_fRadian; //각도

    float   m_fAngle;

    bool m_bChangeDir;
    Vec3 m_TarPos;

    bool m_flag;
    Vec3 m_vPos;
    Vec2 m_dir;

    UINT m_iAttackNum; //*************************추가 

    //test
    bool  d;


    //부채꼴 
    bool b_bAwake;

    CStructuredBuffer* m_pSectorBuffer;
    tSectorInfo m_tSectorInfo;



public:

    void SetSectorInfo(float _fAngle, float _fDistance)
    {
        m_tSectorInfo.fAngle = _fAngle;
        m_tSectorInfo.fDistance = _fDistance;
    }



    virtual void UpdateData();
    void SetAttackNum(UINT _num) { m_iAttackNum = _num; }
    void SetChangeDir(bool _flag) { m_bChangeDir = _flag; }
    void SetTarPos(Vec3 _tarpos) { m_TarPos = _tarpos; }
    void SetMaxTime(float _time) { m_fMaxtime = _time; }

public:
    virtual void update();
    virtual void awake();

public:
    void SetSpeed(float _Speed) { m_fSpeed = _Speed; }
    void SetRadian(float _Radian) { m_fRadian = _Radian; };


public:
    CLONE(CBulletScript);

public:
    CBulletScript();
    ~CBulletScript();


    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);
};


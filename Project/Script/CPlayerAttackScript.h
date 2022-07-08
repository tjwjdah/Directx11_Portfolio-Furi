#pragma once
#include <Engine\CScript.h>


enum class Attack_Type
{
    SowrdAttack,
    ChargeSowrdAttack,
    FullChargeSowrdAttack,
    GunAttack,
    ChargeGunAttack,
    FullChargeGunAttack,
    MeleeAttack
};
class CPlayerAttackScript :
    public CScript
{
private:
    CGameObject*       m_Player;
    
    Attack_Type        m_AttackType;
    
    float              m_fAngleStart;
    float              m_fAngleEnd;

    float              m_fTime;
    float              m_fdeleteTime;
    Vec2               m_vDir;

    bool               m_bDead;

    bool               m_Attack4;
public:
    virtual void awake();
    virtual void update();
    
public:
    CLONE(CPlayerAttackScript);

public:
    void SetPlayer(CGameObject* _Player, Vec2 _AttackDir);

    void SetfAngleStart(float _fAngleStart);
    void SetfAngleEnd(float _fAngleEnd);
    void SetAttackType(Attack_Type _AttackType);
    Vec2 GetDir() { return m_vDir; }
    void Attac4() { m_Attack4 = true; }
public:
    CPlayerAttackScript();
    ~CPlayerAttackScript();

public:
    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);
};


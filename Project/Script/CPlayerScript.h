#pragma once
#include <Engine\CScript.h>

class CAnimator3D;
class CNavMeshManager;
class CCameraAnimaitorScript;
enum class HitScale{
    HitLight,
    HitMedium,
    HitBig
};
class CPlayerScript :
    public CScript
{
private:
    PLAYER_STATE        m_eCurState;            //플레이어의 현재 상태
    PLAYER_STATE        m_ePrevState;           //이전 상태

    DIR                 m_eCurDir;              //현재 방향
    DIR                 m_ePreDir;              //이전 방향
    DIR                 m_AnimDir;
    DIR                 m_PreAinmDir;

    float               m_fPlayerSpeed;         //플레이어 스피드
    float               m_fAtime;               //누적시간
    float               m_fMaxtime;


    int                  m_CurrentCombo;
    int                  m_PreCombo;
    int                  m_MaxCombo;
    int                  m_Combo4AttackCase;


    bool                 m_IsAttacking;
    bool                 m_CanNextCombo;
    bool                 m_IsComboInputOn;
    bool                 m_Combo4AttackOn;
    bool                 m_AttackMoveStop;



    bool                 m_AttackCharge;
    float                m_AttackChargeTime;
    bool                 m_AttackChargefull;
     
    bool                m_MeleeAtkOn;

    bool                m_MeleeAtKSound1;
    bool                m_MeleeAtKSound2;
    bool                m_MeleeAtKSound3;
    bool                m_MeleeAtKSoundKick;
    bool                m_AttackMiss;
    bool                m_Grap;
  
    int                 m_GrapSolve;
    float               m_Grapdelay;
    float               m_GrapTime;

    float               m_GunAttackTime;
    bool                m_GunChargeSound;
    float                m_DashChargeTime;

    float                m_ParryTime;

    Vec2                 m_AttackDir;
    float                m_AttackSpeed;
    CAnimator3D*       m_Animation;

    Ptr<CPrefab>             m_Attack;


    CGameObject*            m_Target;
    float                   m_TargetAngle;
    CNavMeshManager*        m_NavMeshManager;
    vector<NavMesh>*        m_NavVector;
    UINT                 m_NavNum;
    bool                 m_hit;
    Vec2                 m_vhitDir;

    bool                 m_bKnockdown;
    bool                 m_bKnockdownstandup;
    float                m_fKnockDownTime;
    float                 m_HitSpeed;


    tRay                 m_tRay;

    float               m_fHP;
    UINT                m_iLife;
    CCameraAnimaitorScript* m_CameraAnim;

    bool                m_Collignore;

    Vec3                m_CPrePos;


    bool                m_CloseBattle;

    CGameObject*        m_SwordCharge;
    CGameObject*    m_SwordCharge2;
    CGameObject*        m_GunCharge;
    CGameObject* PostEffect_Vignette;
    CGameObject* PostEffect_GlichLine;
    bool                m_SwordChargebool;
    bool                m_GunChargebool;
public:
    virtual void awake();
    virtual void update();

public:
    void SetTarGetObj(CGameObject* _OBj) { m_Target = _OBj; }
    void SetNaviManager(CNavMeshManager* _OBj) { m_NavMeshManager = _OBj; }

    GET(float, TargetAngle);
    GET_SET(float, fHP);
    GET_SET(UINT, iLife);
    GET_SET(bool, AttackMoveStop);
private:
    void CheckState();
    void PlayAnimation();

    void PlayerAction();
    void Attack();
    void Attacking();
    void AttackStartComboState();
    void AttackEndComboState();

    
   
    void CalRay();
    bool PlayerNavCheck(Vec3 _vertices1, Vec3 _vertices2, Vec3 _vertices3, Vec3 _vStart, Vec3 _vDir);
    void AllNavVectorCheck();
    bool NavVectorCheck();
    bool NavCheck();
    float MouseRote();
    const tRay& GetRay() { return m_tRay; }

 
public:
    bool KnockBack(Vec3 _CollPos,bool _Parrypossible, HitScale _HitScale,float _Damage);
    bool knockdown(Vec3 __CollPos,bool _Parrypossible, HitScale _HitScale, float _Damage);
    GET_SET(bool, Collignore);
    bool Grap();
    void MeleeAttack();
    void Monsterguard();

    void SetCloseBattle(bool _CloseBattle) { m_CloseBattle = _CloseBattle; }
    CCameraAnimaitorScript* GetCameraAnimitor() { return m_CameraAnim; }
public:
    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);
public:
    CLONE(CPlayerScript);
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);


public:
    CPlayerScript();
    ~CPlayerScript();
};


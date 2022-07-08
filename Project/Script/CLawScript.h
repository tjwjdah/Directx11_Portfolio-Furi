#pragma once
#include <Engine\CScript.h>
class CPrefab;
class CGameObject;
class CCameraAnimaitorScript;

enum class Pattern
{
    START,
    LOOP,
    PLAY,
    END,
    NONE
};

enum class LAWCAM
{
    CAM_FAR,   //탑뷰
    CAM_CLOSE, //근거리
};

enum class LAWSKILL
{
    //근접 melee
    ATK_SWIPE_SLASH_COMBO_A,  //0
    //ATK_SWIPE_SLASH_COMBO_B,  
    ATK_SWIPE_SLASH_COMBO_C,//1
    ATK_SWIPE_SLASH_COMBO_D,

    ATK_GUNSHOT,  //3
    ATK_GUNSHOTSWIPE,
    ATK_ANGULAR_WAVE,
    //==================================== phase 2
    ATK_WIDE_SLASH_RTOL,
    ATK_WIDE_SLASH_STICK,
    ATK_YOYO,
    //==================================== phase 3
    ATK_CIRCLE_SHOOT,//9
    ATK_GRAB,  //10

    NONE
};

enum class LAWSTATE
{
    IDLE,
    MOVE,
    ATTACK,

    HIT_RECEIVED, //맞기
    KNOCKDOWN,  // ->   STANDUP
    GUARD,
    REFLECTBULLET, // 패링공격_총
    //TAUNT, //도발
    CHANGE_PHASE,
    RUNAWAY, //도망

    PARRY_ATK, //패링공격

    REGEN, //리젠

    STUN, //기절
    WEAKENED, //지침
    BEGIN_FIGHT,
    MELEEATK,
    GRAB, //잡기

    NONE
};

enum class LAWDIR
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NONE
};

class CLawScript
    :public CScript
{
private:
    Ptr<CPrefab>             m_Attack1; //총알
    Ptr<CPrefab>             m_Attack2; //총알2
    Ptr<CPrefab>             m_Attack3; //부채꼴
    Ptr<CPrefab>             m_Attack4; //yoyo
    Ptr<CPrefab>             m_Attack5; //slash
    Ptr<CPrefab>          m_Attack6; //큰 부채꼴 공격
    Ptr<CPrefab>          m_Attack7; //빨간 장판 (충돌체x)
    CCameraAnimaitorScript* m_CameraAnim;

    LAWSTATE m_state;
    LAWSKILL m_skill;
    Pattern m_pattern;
    LAWDIR m_dir;

    bool m_HitRed;
    bool m_Hit;
    UINT m_HitCount;
    float m_HitTime;
    float m_HitRedTime;

    float m_ftime;
    float m_fMoveTime;
    float m_fAttackTime;
    float m_fmaxtime;
    float m_fspeed;

    //애니
    CGameObject* m_AniObj;
    const vector<tMTAnimClip>* m_pVecClip;
    bool m_bFind;
    bool m_anim;

    //target
    float m_TarAngle;
    Vec2 m_TargetVec;
    LAWDIR m_TargetDir;

    //attack
    int m_AttackCount;
    bool m_Didattack;
    Vec3 m_PlayerPos;
    Vec3 m_PlayerPos2;
    Vec3 m_StartPos;
    float m_AttackSpeed;
    UINT m_BulletNum;
    float m_RotAngle;

    int m_RandSkill; //선택된 스킬
    int m_Telpo; //1 tepo 0 move
    int m_TelpoCount;
    int m_iMeleeCount;

    float m_fMinDis; // 근접 이동시 플레이어와의 최소 거리

    bool m_CircleShootFlag;

    // HP
    float m_fLawHp;      //hp
    int m_Lawlife;      //life
    LAWCAM m_lawcam;   //피통

    //idle
    bool bMoveBack; //idle 뒤로 이동
    bool bIsEndLaw;

    //phase
    UINT m_LastPhase;
    bool m_PlayerIsGuard; //잡기
    Vec2 m_ColCenterPos; //충돌체 중심 위치
    float m_fMaxDis;  //보스가 중심에서 벗어날 수 없는 최대 거리

    UINT m_testskill;
    float m_Centerlength; //******************************** 9/4 수정
    void CalCenterPos();
    bool CalCenterPos2(Vec3 _TelPos);

    //test
    float m_testTardistance;

    CGameObject* portal_ring;
    bool         m_bPortalringOn;
public:
    LAWCAM GetLawCam() { return m_lawcam; } //피통
    UINT GetiLife() { return (UINT)m_Lawlife; }
    float GetLawHp() { return m_fLawHp; }

    bool IsGuard()
    {
        if (m_state == LAWSTATE::GUARD || m_state == LAWSTATE::PARRY_ATK || m_state == LAWSTATE::REFLECTBULLET) { return true; }
        return false;
    }

    void HitReceived(); // 일반공격, 차지공격
    void FullChargedHitReceived(); //풀차지 공격
    void GunHitReceived(); //총공격
    bool IsStun() { if (m_state == LAWSTATE::STUN) { return true; } return false; }
    void FullChargedGunHitReceived();
    void SetMELEEATK()
    {
        if (m_state != LAWSTATE::MOVE)

            m_state = LAWSTATE::MELEEATK;
        m_pattern = Pattern::START;
        m_ftime = 0.f;
        m_anim = false;
    }

    void SetPlayerGuardFail() { m_PlayerIsGuard = false; }
    void SetPlayerGuardSucc() { m_PlayerIsGuard = true; }

public:
    virtual void awake();
    virtual void update();

private:

    //state
    void Attack();
    void Move();
    void Idle();

    void Setphase();
    void MakeRedLaw();
    Vec3 GetRandomLawPos(UINT _MinDis, UINT _MaxDis); //플레이어 범위 거리 
    float CalTargetDis(Vec3 _TargetPos);
    void ChangeState();
    void LookAt(Vec3 _TargetPos);
    int PlayAnim(const wstring& _strName, bool _Repeat, Pattern _Nextpattern, LAWSTATE _curstate = LAWSTATE::NONE);//PlayAnim(int _idx, Pattern _ptn, LAWSTATE _state = LAWSTATE::NONE);
    void func(const vector<CGameObject*>& vecchild); //애니 클립 찾기

    //move
    void MOVE_WALK();
    void MOVE_RUN();
    void MOVE_DASH();
    void MOVE_TELPO();
    void MOVE_TELPO(Vec3 _Destination);
    void GAP_BACK(); // 간격 두기
    void GAP_CLOSER(); // 간격 좁히기

    //공격할때 가까이 이동
    float m_fCloserTime;
    bool GAP_CLOSER(float _dis, float _time = 0.5);

    //attack
    //원거리
    void ATK_GUNSHOT();
    void ATK_GUNSHOT_SWIPE();
    void ATK_ANGULAR_WAVE();

    //맵의 특정 위치로
    void ATK_CIRCLE_SHOOT();

    //중간
    void ATK_WIDE_SLASH_RTOL();
    void ATK_WIDE_SLASH_STICK();
    void ATK_YOYO();

    //근접
    void ATK_SWIPE_SLASH_COMBO_A();
    void ATK_SWIPE_SLASH_COMBO_B();
    void ATK_SWIPE_SLASH_COMBO_C();
    void ATK_SWIPE_SLASH_COMBO_D();

    void ATK_GRAB(); //잡기

    //패링공격
    void ATK_PARRY();

public:
    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);

public:
    CLONE(CLawScript);

    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);

    CLawScript();
    ~CLawScript();
};

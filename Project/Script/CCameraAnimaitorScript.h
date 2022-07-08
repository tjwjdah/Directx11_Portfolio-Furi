#pragma once
#include <Engine\CScript.h>

struct Transformstruct {
    Vec3  bnC_Translation;
    Vec3  bnC_Scali;
    Vec3  bnC_Rotation;
};

enum class CameraPlayType {
    NormalBattle,
    CloseBattle,
    AnimPlay,
    Recoding,
    ChargeAttackAnin,
    Zoomin,
    ZoomOut,
    None
};
struct CameraData {
    vector<Transformstruct> CameraTransform;
    wstring         CameraClipName;
};

class CCameraAnimaitorScript :
    public CScript
{
private:
    vector<CameraData>      m_ClipData;
    CameraData*             m_CupClipData;
    bool                    m_bnCamera;
    CCameraAnimaitorScript* m_bnChild;
    CameraPlayType          m_PlayingType;
    CameraPlayType          m_PrePlayeringType;
    Transformstruct         m_PreTransform;
    Transformstruct         m_PreRootTransform;

    bool                    m_bFinish;
    float                   m_dCurTime;
    int                     m_iFrameCount;
    UINT                    m_iFrameIdx;
    UINT                    m_iFrameEndIdx;
    int                     m_iFramespeed;

 
    CameraData              m_RecodeClip;
    float                   m_RecodeFrm;
    int                     m_RecodeNum;

 
    float                   m_ZoomSpeed;
    float                   m_ZoomYDistance;
    Vec3                    m_ZoomDir;
    bool                    m_ZoomXCheck;
    bool                    m_ZoomYCheck;
    bool                    m_ZoomZCheck;
    CGameObject*            m_TargetPlayerObj; 
    CGameObject*            m_TargetMonsterObj;


    float                   m_ChargeAttackAnimTime;

    float                   m_Distance;
    Vec2                    m_PlayerPos;
    Vec2                    m_TargetPos;

    float                   m_HitTime;
    Vec3                    m_HitPrePos;
    bool                    m_HitOn;
public:
    virtual void awake();
    virtual void update();
public:
    void LoadClipData(const wstring& _strFilePath);
    void SetbnChild(CGameObject* _bnChild);
    void SetCameraAnim(UINT _i, int _FrameSpeed=1);

    void ClipRecoding();

    void Recoding();
    void ClipShow();

    void Zoomin();
    void ZoomOut();
    void SetRecodeFrm(float _Frm) { m_RecodeFrm = _Frm; }
    int  GetRecodeFrm() { return m_RecodeFrm; }

    void ChangeBattle();
    void NormalBattle();
    void CloseBattle();

    void HiT();
    void HitOn() { m_HitOn = true; }

    vector<CameraData>* GetCameraClipData() {return &m_ClipData;}

    
    void MonsterZoomin(float _ZoominSpeed = 1000,float _ZoominDistance =400);
    void SetTargerPlayer(CGameObject* _Obj) { m_TargetPlayerObj = _Obj; }
    void SetTargerMonster(CGameObject* _Obj) { m_TargetMonsterObj = _Obj; }
public:
    CLONE(CCameraAnimaitorScript);

public:
    CCameraAnimaitorScript();
    ~CCameraAnimaitorScript();
};


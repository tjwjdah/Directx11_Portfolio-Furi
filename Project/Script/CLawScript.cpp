#include "pch.h"
#include "CLawScript.h"

#include <Engine\CSceneMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CScene.h>
#include <Engine\CTransform.h>
#include <Engine\CScript.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CCore.h>
#include <Engine\CLayer.h>
#include <Engine\CCamera.h>
#include <Engine\CEventMgr.h>
#include <Engine/CCollider2D.h>
#include <Engine\CParticleSystem.h>
#include "CBulletScript.h"
#include "CPlayerScript.h"
#include "CAnimatorScript.h"
#include "CCameraAnimaitorScript.h"
#include "CSecneChangeScript.h"
#include "CEffectScript.h"
CLawScript::CLawScript() :
    CScript((int)SCRIPT_TYPE::LAWSCRIPT)
    , m_state(LAWSTATE::NONE)
    , m_skill(LAWSKILL::NONE)
    , m_pattern(Pattern::NONE)
    , m_ftime(0.f)
    , m_fAttackTime(0.f)
    , m_fMoveTime(0.f)
    , m_fmaxtime(1.f)
    , m_fspeed(300.f)
    , m_anim(false)
    , m_TargetDir(LAWDIR::DOWN)
    , m_bFind(false)
    , m_dir(LAWDIR::DOWN)
    , m_Didattack(false)
    , m_AttackCount(1)
    , m_AniObj(nullptr)
    , m_BulletNum(0)
    , m_AttackSpeed(0.f)
    , m_RandSkill(0)
    , m_Telpo(0)
    , m_Hit(false)
    , m_HitCount(0)
    , m_HitTime(0.f)
    , m_TarAngle(0.f)
    , m_RotAngle(0.f)
    , m_fMinDis(130.f)
    , m_fLawHp(100.f)
    , m_HitRedTime(0.f)
    , m_TelpoCount(1)
    , m_iMeleeCount(0)
    , bMoveBack(false)
    , m_Lawlife(3) //목숨 개수
    , bIsEndLaw(false)
    , m_lawcam(LAWCAM::CAM_FAR) //카메라 시점
    , m_LastPhase(0)
    , m_CircleShootFlag(false)
    , m_testskill(0) ///9/8
    , m_fCloserTime(0.f) //9/10
    , m_testTardistance(0.f) //9/10 test
    , portal_ring(nullptr)
    , m_bPortalringOn(false)
{
    AddDesc(tDataDesc(SCRIPT_DATA_TYPE::INT, "ang", &m_iMeleeCount));//m_TarAngle));  //-2.5
    AddDesc(tDataDesc(SCRIPT_DATA_TYPE::FLOAT, "dis", &m_fCloserTime));//m_testTardistance));  //-2.5
}

CLawScript::~CLawScript() {}

void CLawScript::awake()
{
    CGameObject* pRootObj = GetObj();
    const vector<CGameObject*>& _vecchild = pRootObj->GetChild();
    m_AniObj = _vecchild[0];
    m_pVecClip = _vecchild[0]->Animator3D()->GetAnimClip();
    //func(_vecchild);

    m_Attack1 = CResMgr::GetInst()->FindRes<CPrefab>(L"spreadbullet1");
    m_Attack2 = CResMgr::GetInst()->FindRes<CPrefab>(L"spreadbullet2");
    m_Attack3 = CResMgr::GetInst()->FindRes<CPrefab>(L"sector");
    m_Attack4 = CResMgr::GetInst()->FindRes<CPrefab>(L"yoyo");
    m_Attack5 = CResMgr::GetInst()->FindRes<CPrefab>(L"slash");
    m_Attack6 = CResMgr::GetInst()->FindRes<CPrefab>(L"sector2");
    m_Attack7 = CResMgr::GetInst()->FindRes<CPrefab>(L"slashrtol");//slashrtol


    m_CameraAnim = (CCameraAnimaitorScript*)CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Camera_bn")->GetScript(L"CCameraAnimaitorScript");
    m_CameraAnim->SetTargerMonster(GetGameObject());
    m_ColCenterPos = Vec2(0, 300);
    m_fMaxDis = 1500.f; //원의 반지름
}
void CLawScript::Setphase()
{
    if (m_Lawlife != 0 && m_fLawHp <= 0.f)
    {
        m_fLawHp = 100.f;

        //m_state = LAWSTATE::CHANGE_PHASE;
        //m_pattern = Pattern::START;
        if (m_lawcam == LAWCAM::CAM_FAR)
        {
            m_CameraAnim->ChangeBattle();
            m_lawcam = LAWCAM::CAM_CLOSE;
        }
        else if (m_lawcam == LAWCAM::CAM_CLOSE)
        {
            m_Lawlife--;
            m_Telpo = 0;
            m_state = LAWSTATE::CHANGE_PHASE;
            m_pattern = Pattern::START;

            m_lawcam = LAWCAM::CAM_FAR;
            m_CameraAnim->ChangeBattle();

            if (m_Lawlife <= 0) //게임 오버
            {
                m_Lawlife = 0;
                m_state = LAWSTATE::WEAKENED;
                m_pattern = Pattern::START;
                return;
            }
        }
    }

    if (m_state == LAWSTATE::CHANGE_PHASE || m_state == LAWSTATE::WEAKENED) return;

    //탑뷰 (피통이 파랑일때)
    if (m_lawcam == LAWCAM::CAM_FAR)
    {
        switch (m_Lawlife)
        {
        case 3:  //1p
        {
            srand((unsigned int)time(NULL));
            m_RandSkill = m_testskill; //rand() % 7;             //0~ 6

            // 9/8
            {
                m_testskill++;
                if (m_testskill > 5) m_testskill = 0; //9/10***
            }

            m_Telpo = rand() % 2;
            m_TelpoCount = rand() % 3 + 1;

            m_skill = (LAWSKILL)m_RandSkill;
            m_state = LAWSTATE::IDLE;
            m_pattern = Pattern::START;
            m_Didattack = false;

            break;
        }
        case 2:
        {
            srand((unsigned int)time(NULL));
            m_RandSkill = m_testskill; // rand() % 10;            //0 ~ 9

             // 9/8
            {
                m_testskill++;
                if (m_testskill >= 8) m_testskill = 0; //9/10***
            }


            m_Telpo = rand() % 2;
            m_TelpoCount = rand() % 3 + 1;

            m_skill = (LAWSKILL)m_RandSkill;
            m_state = LAWSTATE::IDLE;
            m_pattern = Pattern::START;
            m_Didattack = false;
            break;
        }
        case 1:
        {
            switch (m_LastPhase)
            {
            case 0:
            {
                m_iMeleeCount = 5;
                m_skill = LAWSKILL::ATK_CIRCLE_SHOOT;
                m_state = LAWSTATE::IDLE;
                m_pattern = Pattern::START;
                m_Didattack = false;

                if (m_LastPhase == 0)
                {
                    m_LastPhase = 1;
                }
                break;
            }
            case 1:
            {
                srand((unsigned int)time(NULL));
                m_RandSkill = rand() % 9;            //0 ~ 8 //9/10***
                m_Telpo = rand() % 2;
                m_TelpoCount = rand() % 3 + 1;

                m_skill = (LAWSKILL)m_RandSkill;
                m_state = LAWSTATE::IDLE;
                m_pattern = Pattern::START;
                m_Didattack = false;
                break;
            }
            }

        }
        }
    }
    // 근거리 공격 (피통이 주황일떄)
    else if (m_lawcam == LAWCAM::CAM_CLOSE)
    {
        //int skill_arr[6] = { 0,1,2,3,7,8 };
        int skill_arr[5] = { 0,1,2,6,7 }; //9/10***

        srand((unsigned int)time(NULL));
        //m_RandSkill = skill_arr[rand() % 6];
        m_RandSkill = skill_arr[rand() % 5]; //9/8

        m_Telpo = rand() % 2;
        m_TelpoCount = rand() % 3 + 1;

        m_skill = (LAWSKILL)m_RandSkill;
        m_state = LAWSTATE::IDLE;
        m_pattern = Pattern::START;
        m_Didattack = false;

    }
    
}
void CLawScript::ChangeState() {

    if (m_state == LAWSTATE::MOVE || m_state == LAWSTATE::MELEEATK || m_state == LAWSTATE::CHANGE_PHASE
        || m_state == LAWSTATE::PARRY_ATK || m_state == LAWSTATE::RUNAWAY || m_state == LAWSTATE::GRAB) return;

    MakeRedLaw();

    {   // Hit

        if (m_HitCount > 2)//&& m_state != LAWSTATE::MOVE)
        {
            m_state = LAWSTATE::KNOCKDOWN;
            m_pattern = Pattern::START;
            m_HitCount = 0;
            m_HitTime = 0.f;

            return;
        }

        if (m_HitCount >= 1)
        {
            m_HitTime += fDT;
            if (m_HitTime > 0.7f)  //0.7초 이내로 연타시 콤보
            {
                m_HitCount = 0;
                m_HitTime = 0.f;
            }
        }

        if (true == m_Hit)
        {
            m_state = LAWSTATE::HIT_RECEIVED;
            m_pattern = Pattern::START;

            m_Didattack = false;
            m_anim = false;

            m_Hit = false;
            m_HitCount++;
            m_HitTime = 0.f;

            m_HitRed = true; //맞았을 때 붉게 
            return;
        }

    }

    //select skill num
    if (m_pattern == Pattern::NONE)
    {
        //test
        /*
        srand((unsigned int)time(NULL));
        m_RandSkill = rand() % 4;//10;
        m_Telpo = rand() % 2;
        m_TelpoCount = rand() % 3 + 1;
        m_skill = (LAWSKILL)1;// m_RandSkill;
        m_state = LAWSTATE::IDLE;
        m_pattern = Pattern::START;
        m_Didattack = false;
        */

        Setphase();

        //** 스킬 예외 
        if (m_skill == LAWSKILL::ATK_CIRCLE_SHOOT || m_skill == LAWSKILL::ATK_YOYO || m_skill == LAWSKILL::ATK_GUNSHOT)
            m_Telpo = 1;
        if (m_skill == LAWSKILL::ATK_GRAB) m_Telpo = 0;
    }

}

void CLawScript::update()
{
    //test
    
    return;

    if (KEY_TAP(KEY_TYPE::NUM_4) && (m_state == LAWSTATE::IDLE || m_state == LAWSTATE::MOVE || m_state == LAWSTATE::ATTACK))
    {
        m_state = LAWSTATE::IDLE;
        m_pattern = Pattern::START;
        m_skill = LAWSKILL::ATK_GRAB;
        m_ftime = 0.f;
        m_Telpo = 0;
        m_anim = false;
    }

    /*if (KEY_TAP(KEY_TYPE::ENTER) && m_state != LAWSTATE::MOVE)
    {
        m_Hit = true;
        m_fLawHp -= 30.f;
        //m_state = LAWSTATE::CHANGE_PHASE;
        //m_pattern = Pattern::START;
        //m_ftime = 0.f;
        //m_anim = false;
    }
    */
    /*
    if (KEY_TAP(KEY_TYPE::NUM_3))
    {
        CSecneChangeScript* Chagne = (CSecneChangeScript*)CSceneMgr::GetInst()->FindObjectByName(L"SceneChagne")->GetScript(L"CSecneChangeScript");
        Chagne->ChangeObjList(ObjList::Change);
        return;
    }
    */
    if (bIsEndLaw)   // 게임오버
    {
        if (m_AniObj->Animator3D()->GetFinish())
        {
            //CSecneChangeScript* Chagne = (CSecneChangeScript*)CSceneMgr::GetInst()->FindObjectByName(L"SceneChagne")->GetScript(L"CSecneChangeScript");
           //  Chagne->ChangeObjList(ObjList::Change);
        }
        return;
    }

#ifdef _DEBUG
    CGameObject * player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
    m_PlayerPos = player->Transform()->GetLocalPos();
    m_testTardistance = CalTargetDis(m_PlayerPos);
#endif

    CalCenterPos();
    ChangeState();

    switch (m_state)
    {
    case LAWSTATE::IDLE:
    {
        Idle();
        break;
    }
    case LAWSTATE::MOVE:
    {
        Move();
        break;
    }
    case LAWSTATE::ATTACK:
    {
        Attack();
        break;
    }
    case LAWSTATE::HIT_RECEIVED:
    {
        //좀 길게 바꾸기
        int ret = PlayAnim(L"NPC_Law=stance1_hitReceived1Light", false, Pattern::NONE);
        if (1 == ret) //애니가 다 끝나면
        {
            //절반 확률로 도망
            int randomove = rand() % 3;//10;
            if (randomove <= 1)
            {
                m_pattern = Pattern::START;
                m_state = LAWSTATE::RUNAWAY;
                m_HitCount = 0;
            }
            else
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
            }

            m_ftime = 0.f;
            m_anim = false;
        }
        break;
    }
    case LAWSTATE::RUNAWAY:
    {
        switch (m_pattern)
        {
        case Pattern::START:
        {
            if (!m_anim)
            {
                Transform()->SetLocalPos(Vec3(9999.f, 9999.f, 9999.f)); //Transform()->SetLocalPosY(9999.f);
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterStart", false);
                m_anim = true;
            }
            bool ret = m_AniObj->Animator3D()->GetFinish();
            if (ret)
            {
                m_pattern = Pattern::PLAY;
                m_ftime = 0.f;
                m_anim = false;
                m_fMoveTime = 0.3f;
            }
            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"telpo");  //*********
                pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterIdle", true);
                m_anim = true;
            }
            MOVE_TELPO(GetRandomLawPos(500, 1100));//텔레포트

            break;
        }
        case Pattern::END:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_TeleportCenterEnd", false, Pattern::NONE);
            if (ret == 1)
            {
                m_TelpoCount = 0;
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;

                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);
            }

            break;
        }
        }
        break;
    }
    case LAWSTATE::KNOCKDOWN:
    {
        switch (m_pattern)
        {
        case Pattern::START:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_hitKnockdown1Light", false, Pattern::PLAY);
            if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::PLAY;
                m_state = LAWSTATE::KNOCKDOWN;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        case Pattern::PLAY:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_standUp", false, Pattern::NONE);
            if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        }
        break;
    }
    case LAWSTATE::GUARD:
    {
        switch (m_pattern)
        {
        case Pattern::START:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_guard2Idle", false, Pattern::PLAY);
            if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::PLAY;
                m_state = LAWSTATE::GUARD;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        case Pattern::PLAY:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_guard3End", false, Pattern::NONE);
            if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        }
        break;
    }
    case LAWSTATE::REFLECTBULLET:
    {
        switch (m_pattern)
        {
        case Pattern::START:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_ReflectBullet2Idle", false, Pattern::PLAY);
            if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::PLAY;
                m_state = LAWSTATE::REFLECTBULLET;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        case Pattern::PLAY:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_ReflectBullet3End", false, Pattern::NONE);
            if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        }
        break;
    }
    case LAWSTATE::CHANGE_PHASE:
    {
        switch (m_pattern)
        {
        case Pattern::START:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_hitKnockdown1Light", false, Pattern::PLAY);
            if (1 == ret) //애니가 다 끝나면
            {

                float _Angle = atan2f(0, -1);
                Transform()->SetLocalRot(Vec3(0, _Angle, 0));

                m_CameraAnim->MonsterZoomin(800.f);
                m_pattern = Pattern::PLAY;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_changePhaseTo1", false, 1.2f);
                m_anim = true;
            }
            bool ret = m_AniObj->Animator3D()->GetFinish();
            if (ret)
            {
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                m_CameraAnim->MonsterZoomin(800.f);
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        }
        break;
    }
    case LAWSTATE::STUN:
    {
        if (!m_anim)
        {
            m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_stunLooseBalance", false, 1.5f);
            m_anim = true;
        }

        bool ret = m_AniObj->Animator3D()->GetFinish();
        if (true == ret) //애니가 다 끝나면
        {
            m_pattern = Pattern::NONE;
            m_state = LAWSTATE::NONE;
            m_ftime = 0.f;
            m_anim = false;
        }
        break;
    }
    case LAWSTATE::PARRY_ATK:
    {
        switch (m_pattern)
        {
        case Pattern::START:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_hitParry1Light", false, Pattern::PLAY);
            if (1 == ret) //애니가 다 끝나면
            {
                //m_state = LAWSTATE::PARRY_ATK;
                m_pattern = Pattern::PLAY;
                m_ftime = 0.f;
                m_anim = false;
                m_Didattack = false;
            }

            break;
        }
        case Pattern::PLAY:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_parry1Start", false, Pattern::END);
            if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::END;
                m_ftime = 0.f;
                m_anim = false;
                m_Didattack = false;
                //ATK_PARRY();
            }

            break;
        }
        case Pattern::END:
        {
            ATK_PARRY();
            /*
            m_pattern = Pattern::NONE;
            m_state = LAWSTATE::NONE;
            m_ftime = 0.f;
            m_anim = false;
            */
            break;
        }
        }
        break;
    }
    case LAWSTATE::MELEEATK:
    {
        if (!m_anim)
        {
            CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
            m_PlayerPos = player->Transform()->GetLocalPos();
            LookAt(m_PlayerPos);

            m_AniObj->Animator3D()->ChangeClip(L"NPC_Father=stance1_meleeAtk", false);
            m_anim = true;
        }

        bool ret = m_AniObj->Animator3D()->GetFinish();
        if (true == ret) //애니가 다 끝나면
        {
            m_pattern = Pattern::NONE;
            m_state = LAWSTATE::NONE;
            m_ftime = 0.f;
            m_anim = false;
        }
        break;
    }
    case LAWSTATE::WEAKENED:
    {
        switch (m_pattern)  //switch -> tab
        {
        case Pattern::START:
        {
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_weakened1Start", false);
                m_anim = true;
            }
            if (m_AniObj->Animator3D()->GetFinish())
            {
                m_state = LAWSTATE::WEAKENED;
                m_pattern = Pattern::END;
            }
            break;
        }
        case Pattern::END:
        {
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_weakened2Idle", false);
                m_anim = true;
            }
            bIsEndLaw = true;
            break;
        }
        }

        break;
    }
    case LAWSTATE::BEGIN_FIGHT:
    case LAWSTATE::NONE:
        break;
    }

}

void CLawScript::Attack()
{
    switch (m_skill) //스킬
    {
    case LAWSKILL::ATK_GUNSHOT:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {

            int ret = PlayAnim(L"NPC_Law=stance1_gunShot1Start", false, Pattern::PLAY);
            if (1 == ret) //애니가 다 끝나면
            {

                m_StartPos = Transform()->GetLocalPos();
                m_BulletNum = 7;
                m_AttackCount = 1;
                m_AttackSpeed = 600.f; // 총알 속도 조절

                // test 2 -------------------
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);
                if (m_TarAngle < 0) m_RotAngle = -1.f;
                else m_RotAngle = 1.f; //(m_TarAngle + XM_PI) - 0.52f; // 30도  
                // test 2 -------------------
            }

            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_gunShot3Shot", false);
                m_anim = true;
            }

            ATK_GUNSHOT();
            break;
        }
        case Pattern::END:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_gunShot4End", false, Pattern::NONE);
            if (1 == ret) //애니가 다 끝나면
            {
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
            }
            break;
        }
        }
        break;
    }
    case LAWSKILL::ATK_GUNSHOTSWIPE:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {
            int ret = PlayAnim(L"NPC_Law=stance2_gunShotswipe1Start", false, Pattern::PLAY);
            if (1 == ret) //애니가 다 끝나면
            {

                /*
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                Vec3 vPlayerPos = player->Transform()->GetLocalPos();
                LookAt(vPlayerPos);
                */
                m_StartPos = Transform()->GetLocalPos();
                m_BulletNum = 7;  //8개 이상 x 180도 넘어가므로.. 또는 각 조절 필요
                m_AttackCount = 2;
                m_AttackSpeed = 550.f; // 속도 조절
            }

            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance2_gunShotswipe2Idle", false);
                m_anim = true;
            }
            ATK_GUNSHOT_SWIPE();
            break;
        }
        case Pattern::END:
        {
            int ret = PlayAnim(L"NPC_Law=stance2_gunShotswipe3End", false, Pattern::NONE);
            if (1 == ret) //애니가 다 끝나면
            {
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
            }
            break;
        }
        }
        break;
    }
    case LAWSKILL::ATK_ANGULAR_WAVE: // 부채꼴 공격 6:10
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {

            m_pattern = Pattern::PLAY;
            //m_ftime = 0.f;
            //m_anim = false;
            m_StartPos = Transform()->GetLocalPos();
            m_AttackCount = 3;
            //m_fAttackTime = 0.8f;
            m_AttackSpeed = 1000.f; // 속도 조절
            break;
        }
        case Pattern::PLAY:
        {
            ATK_ANGULAR_WAVE();
            break;
        }
        case Pattern::END:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_idle", false, Pattern::NONE);
            if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;

            }
            break;
        }
        }
        break;
    }
    case LAWSKILL::ATK_SWIPE_SLASH_COMBO_A:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {
            m_pattern = Pattern::PLAY;
            m_StartPos = Transform()->GetLocalPos();
            m_AttackCount = 3;
            m_fCloserTime = 0.f; //9/10
            m_iMeleeCount = 0;  //9/10
            //m_fAttackTime = 4.5f;
            //m_AttackSpeed = 400.f; // 속도 조절

            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                m_Didattack = true;
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance2_swipeSlashComboA", false);
                m_anim = true;
            }
            ATK_SWIPE_SLASH_COMBO_A();
            break;
        }
        case Pattern::END:
        {
            //int ret = PlayAnim(L"NPC_Law=stance1_idle", false, Pattern::NONE);
            //if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        }
        break;
    }
    //case LAWSKILL::ATK_SWIPE_SLASH_COMBO_B:
    //{
    //    switch (m_pattern) //패턴
    //    {
    //    case Pattern::START:
    //    {
    //        m_pattern = Pattern::PLAY;
    //        m_StartPos = Transform()->GetLocalPos();
    //        m_AttackCount = 1;
    //        //m_fAttackTime = 4.5f;
    //        //m_AttackSpeed = 400.f; // 속도 조절
    //        break;
    //    }
    //    case Pattern::PLAY:
    //    {
    //        if (!m_anim)
    //        {
    //            //m_Didattack = true;
    //            m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance2_swipeSlashComboB", false);
    //            m_anim = true;
    //        }
    //        ATK_SWIPE_SLASH_COMBO_B();
    //        break;
    //    }
    //    case Pattern::END:
    //    {
    //        //int ret = PlayAnim(L"NPC_Law=stance1_idle", false, Pattern::NONE);
    //        //if (1 == ret) //애니가 다 끝나면
    //        {
    //            m_pattern = Pattern::NONE;
    //            m_state = LAWSTATE::NONE;
    //            m_ftime = 0.f;
    //            m_anim = false;
    //        }
    //        break;
    //    }
    //    }
    //    break;
    //}
    case LAWSKILL::ATK_SWIPE_SLASH_COMBO_C:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {
            m_pattern = Pattern::PLAY;
            m_StartPos = Transform()->GetLocalPos();
            m_AttackCount = 3;
            m_fCloserTime = 0.f; //9/10
            m_iMeleeCount = 0;  //9/10
            //m_fAttackTime = 4.5f;
            //m_AttackSpeed = 400.f; // 속도 조절
            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                m_Didattack = true;
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance2_swipeSlashComboC", false);
                m_anim = true;
            }
            ATK_SWIPE_SLASH_COMBO_C();
            break;
        }
        case Pattern::END:
        {
            //int ret = PlayAnim(L"NPC_Law=stance1_idle", false, Pattern::NONE);
            //if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        }
        break;
    }
    case LAWSKILL::ATK_SWIPE_SLASH_COMBO_D:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {
            m_pattern = Pattern::PLAY;
            m_StartPos = Transform()->GetLocalPos();
            m_AttackCount = 2;
            m_fCloserTime = 0.f; //9/10
            m_iMeleeCount = 0;  //9/10
            //m_fAttackTime = 4.5f;
            //m_AttackSpeed = 400.f; // 속도 조절
            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                m_Didattack = true;
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance2_swipeSlashComboD", false);
                m_anim = true;
            }
            ATK_SWIPE_SLASH_COMBO_D();
            break;
        }
        case Pattern::END:
        {
            //int ret = PlayAnim(L"NPC_Law=stance1_idle", false, Pattern::NONE);
            //if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;
            }
            break;
        }
        }
        break;
    }
    case LAWSKILL::ATK_WIDE_SLASH_RTOL:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {
            m_pattern = Pattern::PLAY;
            m_StartPos = Transform()->GetLocalPos();
            m_AttackCount = 1;
            m_fAttackTime = 1.5f;

            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance3_wideSlashRtoL", false);
                m_anim = true;
                // 장판
                {
                    
                    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                    m_PlayerPos = player->Transform()->GetLocalPos();
                    LookAt(m_PlayerPos);

                    CGameObject* pBullet = nullptr;
                    pBullet = m_Attack7->Instantiate();
                    pBullet->SetName(L"Sector");  /////////////////////////////////////////
                    Vec3 mon_pos = Transform()->GetLocalPos();

                    pBullet->Transform()->SetLocalScale(Vec3(1300.f, 1300.f, 1300.f));
                    pBullet->Transform()->SetLocalPos(Vec3(mon_pos));
                    pBullet->Transform()->SetLocalRot(Vec3(0.f, m_TarAngle, 0.f));

                    CBulletScript* tar = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
                    tar->SetChangeDir(false);
                    tar->SetTarPos(m_PlayerPos);
                    tar->SetSpeed(0);
                    tar->SetAttackNum(3);  //************************************** 수정
                    tar->SetMaxTime(1.5f);
                    tar->SetSectorInfo(1.57f, 0.01f);
                    {
                        tEvent even = {};
                        even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                        even.lParam = (DWORD_PTR)pBullet;
                        even.wParam = (DWORD_PTR)2;
                        CEventMgr::GetInst()->AddEvent(even);
                    }
                    m_PlayerPos2 = m_PlayerPos;
                }
            }
            ATK_WIDE_SLASH_RTOL();
            break;
        }
        case Pattern::END:
        {
            //int ret = PlayAnim(L"NPC_Law=stance1_idle", false, Pattern::NONE);
            //if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;

            }
            break;
        }
        }
        break;
    }
    case LAWSKILL::ATK_WIDE_SLASH_STICK:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {

            m_pattern = Pattern::PLAY;
            m_StartPos = Transform()->GetLocalPos();
            m_AttackCount = 1;
            m_fAttackTime = 1.5f;

            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_wideSlashFrontStickTutorial", false);
                m_anim = true;
                // 장판
                {
                    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                    m_PlayerPos = player->Transform()->GetLocalPos();
                    LookAt(m_PlayerPos);

                    CGameObject* pBullet = nullptr;
                    pBullet = m_Attack7->Instantiate();
                    pBullet->SetName(L"Sector");  /////////////////////////////////////////
                    Vec3 mon_pos = Transform()->GetLocalPos();

                    pBullet->Transform()->SetLocalScale(Vec3(1300.f, 1300.f, 1300.f));
                    pBullet->Transform()->SetLocalPos(Vec3(mon_pos));
                    pBullet->Transform()->SetLocalRot(Vec3(0.f, m_TarAngle, 0.f));

                    CBulletScript* tar = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
                    tar->SetChangeDir(false);
                    tar->SetTarPos(m_PlayerPos);
                    tar->SetSpeed(0);
                    tar->SetAttackNum(3);  //************************************** 수정
                    tar->SetMaxTime(1.5f);
                    tar->SetSectorInfo(1.57f / 3, 0.01f);
                    {
                        tEvent even = {};
                        even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                        even.lParam = (DWORD_PTR)pBullet;
                        even.wParam = (DWORD_PTR)2;
                        CEventMgr::GetInst()->AddEvent(even);
                    }
                    m_PlayerPos2 = m_PlayerPos;
                }
            }
            ATK_WIDE_SLASH_STICK();
            break;
        }
        case Pattern::END:
        {
            //int ret = PlayAnim(L"NPC_Law=stance1_idle", false, Pattern::NONE);
            //if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;

            }
            break;
        }
        }
        break;
    }
    case LAWSKILL::ATK_CIRCLE_SHOOT:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {
            if (!m_CircleShootFlag)
            {

                m_CircleShootFlag = true;
            }

            int ret = PlayAnim(L"NPC_Law=stance1_circleShoot1Start", false, Pattern::PLAY);
            if (ret == 1)
            {
                m_pattern = Pattern::PLAY;
                m_StartPos = Transform()->GetLocalPos();
                m_AttackCount = 5;
                //m_fAttackTime = 10.f;
                m_BulletNum = 30;
                //m_AttackSpeed = 400.f; // 속도 조절
            }
            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_circleShoot2Idle", true);
                m_anim = true;
                m_iMeleeCount--;
            }

            ATK_CIRCLE_SHOOT();
            break;
        }
        case Pattern::END:
        {
            int ret = PlayAnim(L"NPC_Law=stance1_circleShoot3End", false, Pattern::NONE);
            if (1 == ret) //애니가 다 끝나면
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;

            }
            break;
        }
        }
        break;
    }
    case LAWSKILL::ATK_YOYO:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {
            {
                m_AttackCount = 1;
                m_AttackSpeed = 1000.f; // 속도 조절
                m_fAttackTime = 2.6f;   // 애니 시간이 안맞을 때 조절
                m_pattern = Pattern::PLAY;
            }
            break;
        }
        case Pattern::PLAY:
        {
            ATK_YOYO();
            break;
        }
        case Pattern::END:
        {
            int ret = PlayAnim(L"NPC_Law=stance2_Yoyo2End", false, Pattern::NONE);
            if (1 == ret) //애니가 다 끝나면
            {
                m_state = LAWSTATE::NONE;
                m_pattern = Pattern::NONE;
                m_ftime = 0.f;
            }
            break;
        }
        }
        break;
    }
    case LAWSKILL::ATK_GRAB:
    {
        switch (m_pattern) //패턴
        {
        case Pattern::START:
        {
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TutoParryCounterAtk", false);
                m_anim = true;
            }
            bool _ret = m_AniObj->Animator3D()->GetFinish();
            if (_ret)
            {
                // Parry false or true?
                CGameObject* Obj = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                CPlayerScript* Player = (CPlayerScript*)Obj->GetScript(L"CPlayerScript");
                bool IsParry = Player->Grap();

                if (IsParry)
                {
                    m_ftime = 0.f;
                    m_anim = false;
                    m_pattern = Pattern::PLAY;
                }
                else
                {
                    m_ftime = 0.f;
                    m_anim = false;
                    m_state = LAWSTATE::NONE;
                    m_pattern = Pattern::NONE;
                }

            }

            break;
        }
        case Pattern::PLAY:
        {
            if (!m_anim) //잡고 흔들기 
            {

                CGameObject* player1 = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player1->Transform()->GetLocalPos();
                Vec3 _MonPos = Transform()->GetLocalPos();
                m_TargetVec.x = m_PlayerPos.x - _MonPos.x;
                m_TargetVec.y = m_PlayerPos.z - _MonPos.z;
                m_TargetVec.Normalize();


                Vec3 _pos = Transform()->GetLocalPos();
                _pos.x -= m_TargetVec.x * 450.f;
                _pos.z -= m_TargetVec.y * 450.f;
                Transform()->SetLocalPos(Vec3(_pos.x, _pos.y, _pos.z));
                //

                //CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                //m_PlayerPos = player->Transform()->GetLocalPos();
                //LookAt(m_PlayerPos);

                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_grabLawCharge1", false);
                m_anim = true;
                m_ftime = 0.f; //(22.1.5)
                m_Didattack = false; //(22.1.5)

            }
            ATK_GRAB();//(22.1.5)

            if (m_AniObj->Animator3D()->GetFinish())
            {
                m_pattern = Pattern::END;
                m_Didattack = false; //(22.1.5)
                m_anim = false;
                m_ftime = 0.f;
            }
            break;
        }
        case Pattern::END:
        {
            if (m_PlayerIsGuard) //끝
            {
                if (!m_anim)
                {
                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_grabLawCharge1Success", false);
                    m_anim = true;
                }
            }
            else //잡고 조르기
            {
                if (!m_anim)
                {
                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_grabLawCharge2", false);
                    m_anim = true;
                }
            }

            if (m_AniObj->Animator3D()->GetFinish())
            {
                m_pattern = Pattern::NONE;
                m_state = LAWSTATE::NONE;
                m_ftime = 0.f;
                m_anim = false;
            }

            break;
        }
        }
        break;
    }
    }
}

void CLawScript::Move()
{
    if (m_Telpo) //telpo
    {
        switch (m_skill) //스킬
        {
            //멀리 떨어지기
        case LAWSKILL::ATK_ANGULAR_WAVE:
        case LAWSKILL::ATK_GUNSHOTSWIPE:
        {
            switch (m_pattern)
            {
            case Pattern::START:
            {
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                if (!m_anim)
                {

                    Transform()->SetLocalPos(Vec3(9999.f, 0.f, 9999.f)); //Transform()->SetLocalPosY(9999.f);
                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterStart", false);
                    m_anim = true;

                }

                //m_ftime += fDT;
                //if (m_ftime >= 0.5f)
                {
                    //1초후
                    m_fMoveTime = 0.5f;
                    m_anim = false;
                    m_fspeed = 750.f;
                    m_ftime = 0.f;
                    m_pattern = Pattern::LOOP;
                }
                break;
            }
            case Pattern::LOOP:
            {
                if (!m_anim)
                {

                    Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"telpo");  //*********
                    pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterIdle", true);
                    m_anim = true;
                }
                if (m_lawcam == LAWCAM::CAM_FAR)
                    MOVE_TELPO(GetRandomLawPos(700, 1100));//텔레포트
                else if (m_lawcam == LAWCAM::CAM_CLOSE)
                    MOVE_TELPO(GetRandomLawPos(300, 300));//텔레포트
                

                break;
            }
            case Pattern::END:
            {
                int ret = PlayAnim(L"NPC_Law=stance1_TeleportCenterEnd", false, Pattern::START);
                if (ret == 1)
                {
                    if (m_TelpoCount != 0)
                    {
                        m_ftime = 0.f;
                        m_pattern = Pattern::START;
                        m_anim = false;
                    }
                    else
                    {
                        m_state = LAWSTATE::ATTACK;
                        m_ftime = 0.f;
                        m_anim = false;
                        m_pattern = Pattern::START;
                    }

                    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                    m_PlayerPos = player->Transform()->GetLocalPos();
                    LookAt(m_PlayerPos);
                }

                break;
            }
            }
            break;
        }

        //중간거리
        case LAWSKILL::ATK_GUNSHOT:
        //case LAWSKILL::ATK_WIDE_SLASH_RTOL:
        case LAWSKILL::ATK_YOYO:
        //case LAWSKILL::ATK_WIDE_SLASH_STICK:
        {
            switch (m_pattern)
            {
            case Pattern::START:
            {
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                if (!m_anim)
                {
                    Transform()->SetLocalPos(Vec3(9999.f, 0.f, 9999.f)); //Transform()->SetLocalPosY(9999.f);
                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterStart", false);
                    m_anim = true;
                }

                //m_ftime += fDT;
                //if (m_ftime >= 0.5f)
                {
                    //1초후
                    m_fMoveTime = 0.5f;
                    m_anim = false;
                    m_fspeed = 750.f;
                    m_ftime = 0.f;
                    m_pattern = Pattern::LOOP;
                }
                break;
            }
            case Pattern::LOOP:
            {
                if (!m_anim)
                {
                    Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"telpo");  //*********
                    pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterIdle", true);
                    m_anim = true;
                }
                if (m_skill == LAWSKILL::ATK_YOYO || m_skill == LAWSKILL::ATK_GUNSHOT)
                {
                    if(m_lawcam == LAWCAM::CAM_FAR)
                    MOVE_TELPO(GetRandomLawPos(500, 900));//텔레포트 //GAP_CLOSER(); //가까워지기
                    else if(m_lawcam == LAWCAM::CAM_CLOSE)
                        MOVE_TELPO(GetRandomLawPos(200, 250));//텔레포트 //GAP_CLOSER(); //가까워지기

                }
                else if (m_skill == LAWSKILL::ATK_WIDE_SLASH_RTOL || m_skill == LAWSKILL::ATK_WIDE_SLASH_STICK)
                {
                    if (m_lawcam == LAWCAM::CAM_FAR)
                        MOVE_TELPO(GetRandomLawPos(400, 500));//텔레포트 //GAP_CLOSER(); //가까워지기
                    else if (m_lawcam == LAWCAM::CAM_CLOSE)
                        MOVE_TELPO(GetRandomLawPos(200, 250));//텔레포트 //GAP_CLOSER(); //가까워지기
                }
                break;
            }
            case Pattern::END:
            {
                int ret = PlayAnim(L"NPC_Law=stance1_TeleportCenterEnd", false, Pattern::START);
                if (ret == 1)
                {
                    if (m_TelpoCount != 0)
                    {
                        m_ftime = 0.f;
                        m_pattern = Pattern::START;
                        m_anim = false;
                    }
                    else
                    {
                        m_state = LAWSTATE::ATTACK;
                        m_ftime = 0.f;
                        m_anim = false;
                        m_pattern = Pattern::START;
                    }

                    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                    m_PlayerPos = player->Transform()->GetLocalPos();
                    LookAt(m_PlayerPos);
                }

                break;
            }
            }
            break;
        }

        //근거리 이동
       //  case LAWSKILL::ATK_GRAB:
        case LAWSKILL::ATK_WIDE_SLASH_RTOL:
        case LAWSKILL::ATK_WIDE_SLASH_STICK:
        case LAWSKILL::ATK_SWIPE_SLASH_COMBO_A:
        //case LAWSKILL::ATK_SWIPE_SLASH_COMBO_B:
        case LAWSKILL::ATK_SWIPE_SLASH_COMBO_C:
        case LAWSKILL::ATK_SWIPE_SLASH_COMBO_D:
        {
            switch (m_pattern)
            {
            case Pattern::START:
            {
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                if (!m_anim)
                {

                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterStart", false);
                    m_anim = true;
                }

                float dis = CalTargetDis(m_PlayerPos); // 거리계산
                if (dis > m_fMinDis)
                {
                    m_fMoveTime = 0.3f;
                    m_anim = false;
                    m_pattern = Pattern::LOOP;
                    m_fspeed = 750.f;
                    Transform()->SetLocalPos(Vec3(9999999.f, 999999.f, 9999999.f)); //Transform()->SetLocalPosY(9999.f);
                }
                else
                {
                    m_pattern = Pattern::END;
                }
                break;
            }
            case Pattern::LOOP:
            {
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                if (!m_anim)
                {

                    Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"telpo");  //*********
                    pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterIdle", true);
                    m_anim = true;
                }
                MOVE_TELPO(GetRandomLawPos(140, 200));//텔레포트 //GAP_CLOSER(); //가까워지기

                break;
            }
            case Pattern::END:
            {
                int ret = PlayAnim(L"NPC_Law=stance1_TeleportCenterEnd", false, Pattern::START);
                if (ret == 1)
                {

                    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                    m_PlayerPos = player->Transform()->GetLocalPos();
                    LookAt(m_PlayerPos);

                    float dis = CalTargetDis(m_PlayerPos); // 거리계산

                    if (m_TelpoCount == 0 || dis < m_fMinDis)  //130
                    {
                        m_state = LAWSTATE::ATTACK;
                        m_ftime = 0.f;
                        m_anim = false;
                        m_pattern = Pattern::START;
                    }
                    else //if (m_TelpoCount != 0)
                    {
                        m_ftime = 0.f;
                        m_pattern = Pattern::START;
                        m_anim = false;
                    }


                }
                break;
            }
            }
            break;
        }
        //맵의 정해진 위치로 텔포
        case LAWSKILL::ATK_CIRCLE_SHOOT:
        {
            switch (m_pattern)
            {
            case Pattern::START:
            {
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                if (!m_anim)
                {
                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterStart", false);
                    m_anim = true;
                    m_TelpoCount = 1; /////
                }

                float dis = CalTargetDis(m_PlayerPos); // 거리계산
                if (dis > 70.f)
                {
                    m_fMoveTime = 0.3f;
                    m_anim = false;
                    m_pattern = Pattern::LOOP;
                    m_fspeed = 750.f;
                    Transform()->SetLocalPos(Vec3(9999.f, 0.f, 9999.f)); //Transform()->SetLocalPosY(9999.f);
                }
                else
                {
                    m_pattern = Pattern::END;
                }
                break;
            }
            case Pattern::LOOP:
            {
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                if (!m_anim)
                {

                    Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"telpo");  //*********
                    pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_TeleportCenterIdle", true);
                    m_anim = true;
                }
                MOVE_TELPO(Vec3(0.f, 100.f, 700.f)); //지정된 곳으로텔레포트 

                break;
            }
            case Pattern::END:
            {
                int ret = PlayAnim(L"NPC_Law=stance1_TeleportCenterEnd", false, Pattern::START);
                if (ret == 1)
                {
                    m_state = LAWSTATE::ATTACK;
                    m_ftime = 0.f;
                    m_anim = false;
                    m_pattern = Pattern::START;

                    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                    m_PlayerPos = player->Transform()->GetLocalPos();
                    LookAt(m_PlayerPos);
                }
                break;
            }
            }
            break;
            break;
        }
        }
    }
    else //대쉬
    {
        switch (m_skill)
        {
            //원거리 이동 
        case LAWSKILL::ATK_ANGULAR_WAVE:
            //case LAWSKILL::ATK_GUNSHOT:
        case LAWSKILL::ATK_GUNSHOTSWIPE:
        {
            switch (m_pattern)
            {
            case Pattern::START:
            {
                if (!m_anim)
                {
                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_dash4Backward1Start", false);
                    m_anim = true;
                }

                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                Vec3 vPlayerPos = player->Transform()->GetLocalPos();
                float dis = CalTargetDis(vPlayerPos); // 거리계산
                if (dis < 500.f)
                {
                    m_anim = false;
                    m_fMoveTime = 0.5f;
                    m_pattern = Pattern::LOOP;
                    m_fspeed = 1750.f;
                }
                else
                {
                    m_pattern = Pattern::END;
                }
                break;
            }
            case Pattern::LOOP:
            {
                if (!m_anim)
                {

                    Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"dash");  //*********
                    pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_dash4Backward2Idle", true);
                    m_anim = true;
                }
                GAP_BACK(); //몬스터랑 멀어지기

                break;
            }
            case Pattern::END:
            {
                if (!m_anim)
                {
                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_dash4Backward3End", true);
                    m_anim = true;
                }

                m_state = LAWSTATE::ATTACK;
                m_ftime = 0.f;
                m_anim = false;
                m_pattern = Pattern::START;

                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                break;
            }
            }
            break;
        }
        //근거리 이동
        case LAWSKILL::ATK_GRAB:
        case LAWSKILL::ATK_SWIPE_SLASH_COMBO_A:
        //case LAWSKILL::ATK_SWIPE_SLASH_COMBO_B:
        case LAWSKILL::ATK_SWIPE_SLASH_COMBO_C:
        case LAWSKILL::ATK_SWIPE_SLASH_COMBO_D:
        case LAWSKILL::ATK_WIDE_SLASH_RTOL:
        case LAWSKILL::ATK_WIDE_SLASH_STICK:
        {
            switch (m_pattern)
            {
            case Pattern::START:
            {
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                if (!m_anim)
                {
                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_dash1Forward1Start", false);
                    m_anim = true;
                }

                float dis = CalTargetDis(m_PlayerPos); // 거리계산
                if (dis > m_fMinDis)
                {
                    m_ftime = 0.f;
                    m_fMoveTime = 0.5f;
                    m_anim = false;
                    m_pattern = Pattern::LOOP;
                    m_fspeed = 1700.f;
                }
                else
                {
                    m_ftime = 0.f;
                    m_pattern = Pattern::END;
                    m_anim = false;
                }
                break;
            }
            case Pattern::LOOP:
            {
                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                if (!m_anim)
                {

                    Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"dash");  //*********
                    pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_dash1Forward2Idle", true);
                    m_anim = true;
                }
                GAP_CLOSER(); //가까워지기

                break;
            }
            case Pattern::END:
            {
                if (!m_anim)
                {
                    m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_dash1Forward3End", false);
                    m_anim = true;
                }

                m_state = LAWSTATE::ATTACK;
                m_ftime = 0.f;
                m_anim = false;
                m_pattern = Pattern::START;

                CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
                m_PlayerPos = player->Transform()->GetLocalPos();
                LookAt(m_PlayerPos);

                break;
            }
            }
            break;
        }
        }
    }

}

void CLawScript::Idle()
{
    switch (m_pattern)
    {
    case Pattern::START:
    {
        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        if (!player)
            return;
        Vec3 _PlayerPos = player->Transform()->GetLocalPos();
        float _dis = CalTargetDis(_PlayerPos);
        if (_dis < 250.f)
        {
            bMoveBack = true;
        }
        else
        {
            bMoveBack = false;
        }

        m_anim = false;
        m_ftime = 0.f;
        m_pattern = Pattern::PLAY;

        break;
    }
    case Pattern::PLAY:
    {
        float _testtime;
        if (bMoveBack)
        {
            //멀어질때
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_freeMoveWalk4Backward", true);
                m_anim = true;
            }
            MOVE_WALK();
            _testtime = 1.7f; //뒤로
        }
        else
        {
            //가만히
            if (!m_anim)
            {
                m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_idleCCM", true);
                m_anim = true;
            }
            _testtime = 0.45f; //idle
        }

        m_ftime += fDT;
        if (m_ftime > _testtime)//0.7f)  //idle시간 조절
        {
            m_pattern = Pattern::END;
            m_ftime = 0.f;
            m_anim = false;
        }
        break;
    }

    case Pattern::END:
    {
        m_state = LAWSTATE::MOVE;
        m_pattern = Pattern::START;
        m_ftime = 0.f;
        bMoveBack = false;

        break;
    }

    }
}

int CLawScript::PlayAnim(const wstring& _strName, bool _Repeat, Pattern _Nextpattern, LAWSTATE _curstate)
{

    if (!m_anim)
    {
        //const vector<CGameObject*>& Childs = GetObj()->GetChild();
        int _aniidx = m_AniObj->Animator3D()->ChangeClip(_strName, _Repeat);// (L"NPC_Law=stance2_gunShotswipe2Idle");
        m_fmaxtime = m_pVecClip->at(_aniidx).dEndTime;
        m_anim = true;

    }

    if (_curstate == LAWSTATE::NONE)
    {
        m_ftime += fDT;
        if (m_ftime > m_fmaxtime)
        {
            m_pattern = _Nextpattern; //Pattern::PLAY;
            m_ftime = 0.f;
            m_anim = false;

            return 1;
        }
    }
    else if (_curstate == LAWSTATE::MOVE)
    {
        m_ftime += fDT;
        if (m_ftime > m_fMoveTime)
        {
            m_pattern = _Nextpattern; //Pattern::PLAY;
            m_ftime = 0.f;
            m_anim = false;
            return 1;
        }
    }

    return -1;
}

void CLawScript::LookAt(Vec3 _TargetPos)
{
    Vec3 _MonPos = Transform()->GetLocalPos();
    m_TargetVec.x = _TargetPos.x - _MonPos.x;
    m_TargetVec.y = _TargetPos.z - _MonPos.z;
    m_TargetVec.Normalize();
    //float target_angle = atan2f(m_TargetVec.y, m_TargetVec.x) + 1.57f;
    m_TarAngle = atan2f(m_TargetVec.x, m_TargetVec.y);
    //testangle = atan2f(m_TargetVec.y, m_TargetVec.x);
    Transform()->SetLocalRot(Vec3(0, m_TarAngle, 0));
}

float CLawScript::CalTargetDis(Vec3 _TargetPos)
{
    Vec3 vLawPos = Transform()->GetLocalPos();
    float dis = Vec2(_TargetPos.x - vLawPos.x, _TargetPos.z - vLawPos.z).Length();
    return dis;
}

#include <random>
Vec3 CLawScript::GetRandomLawPos(UINT _MinDis, UINT _MaxDis) //******************************** 9/4 수정
{
    int FLOAT_MIN = -3.1f;
    int FLOAT_MAX = 3.1f;
    Vec3 _LawPos;

    while (1)
    {
        srand((unsigned int)time(NULL));

        //float _RanAngle = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX / (FLOAT_MAX - FLOAT_MIN)));

        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);

        float _RanAngle = distr(eng);

        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        Vec3 _PlayerPos = player->Transform()->GetLocalPos();

        float _RanDis = rand() % (_MaxDis - _MinDis + 1) + _MinDis;

        _LawPos.x = _PlayerPos.x + _RanDis * cosf(_RanAngle);
        _LawPos.z = _PlayerPos.z + _RanDis * sinf(_RanAngle);

        /*
        * 맵 밖 못나가게 예외처리
        */
        if (CalCenterPos2(_LawPos)) break;
    }

    return _LawPos;
}


void CLawScript::func(const vector<CGameObject*>& vecchild)
{
    if (m_bFind) return;

    int vecsize = vecchild.size();
    for (int i = 0; i < vecsize; ++i)
    {
        if (vecchild[i]->Animator3D() != nullptr)
        {
            m_pVecClip = vecchild[i]->Animator3D()->GetAnimClip();
            m_bFind = true;
            return;
        }

        func(vecchild[i]->GetChild());
    }
}


void CLawScript::OnCollisionEnter(CGameObject* _pOther)  //************************************** 수정
{
    if (_pOther->GetName() == L"spreadbullet2")
    {
        GunHitReceived();

        tEvent _temp = {};
        _temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
        _temp.lParam = (DWORD_PTR)_pOther;
        CEventMgr::GetInst()->AddEvent(_temp);
    }
}

void CLawScript::OnCollision(CGameObject* _pOther)
{
    //if (_pOther->GetScript(L"CTileCollsion")
    //   || _pOther->GetScript(L"CMonsterScript"))
    //{
    //}
}

void CLawScript::OnCollisionExit(CGameObject* _pOther)
{
    //if (_pOther->GetScript(L"CTileCollsion")
    //   || _pOther->GetScript(L"CMonsterScript"))
    //{
    //}
}

void CLawScript::MakeRedLaw()
{
    if (true == m_HitRed)
    {
        int a = 1;
        m_AniObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::INT_3, &a);
        m_HitRed = false;

        // ===================
        // Hit Effect (22.1.3)
        // ===================
        Vec3 _pos = Transform()->GetLocalPos();
        CGameObject* pHit = new CGameObject;
        pHit->SetName(L"HitEffect");
        pHit->AddComponent(new CTransform);
        pHit->AddComponent(new CParticleSystem);
        pHit->AddComponent(new CEffectScript);
        pHit->Transform()->SetLocalPos(Vec3(_pos.x, 100.f, _pos.z));
        pHit->ParticleSystem()->SetPaticleName(L"Hit");
        pHit->ParticleSystem()->SetShaderIdx(1);
        pHit->ParticleSystem()->SetiMaxCount(100);
        pHit->ParticleSystem()->SetvCreateRange(Vec4(2.f, 2.f, 1.f, 0.f));
        pHit->ParticleSystem()->SetvEndColor(Vec4(0.f, 0.f, 1.0f, 1.f));
        pHit->ParticleSystem()->SetvStartColor(Vec4(0.f, 0.5f, 0.5f, 0.5f));
        pHit->ParticleSystem()->SetfMinSpeed(2);
        pHit->ParticleSystem()->SetfMaxSpeed(5);
        pHit->ParticleSystem()->SetvStartScale(Vec3(50.f, 0.f, 0.f));
        pHit->ParticleSystem()->SetvEndScale(Vec3(400.f, 0.f, 0.f));
        pHit->ParticleSystem()->SetDirection(0);
        pHit->ParticleSystem()->SetfMinLifeTime(0.3f);
        pHit->ParticleSystem()->SetfMaxLifeTime(0.35f);
        pHit->ParticleSystem()->SetfFrequency(0.05f);

        CEffectScript* BulletS = (CEffectScript*)pHit->GetScript(L"CEffectScript");
        BulletS->SetType(EFFECT::NONE);
        BulletS->SetTime(0.5f);
        BulletS->SetStopTime(0.2f);

        tEvent even = {};
        even.eEvent = EVENT_TYPE::CREATE_OBJECT;
        even.lParam = (DWORD_PTR)pHit;
        even.wParam = (DWORD_PTR)2;
        CEventMgr::GetInst()->AddEvent(even);
    }

    m_HitRedTime += fDT;
    if (m_HitRedTime > 0.35f)
    {
        int a = 0;
        m_AniObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::INT_3, &a);
        m_HitRed = false;
        m_HitRedTime = 0.f;
    }
}





void CLawScript::HitReceived() //************************************** 수정
{
    if (!(m_state == LAWSTATE::IDLE || m_state == LAWSTATE::ATTACK ||
        m_state == LAWSTATE::HIT_RECEIVED || m_state == LAWSTATE::MELEEATK)) return;
    //if (m_state == LAWSTATE::PARRY_ATK ||m_state == LAWSTATE::GUARD) return;
    //if (m_state == LAWSTATE::REFLECTBULLET || m_state == LAWSTATE::PARRY_ATK || m_state == LAWSTATE::GUARD || m_state == LAWSTATE::STUN || m_state == LAWSTATE::KNOCKDOWN) return;

    UINT _Guard = rand() % 10;
    //가드
    if (_Guard >= 9)
    {
        m_anim = false;
        m_state = LAWSTATE::PARRY_ATK;
        m_pattern = Pattern::START;
    }
    //가드 실패
    else
    {
        m_Hit = true;
        m_fLawHp -= 10.f;
    }
}


void CLawScript::FullChargedHitReceived() //************************************** 수정
{
    if (!(m_state == LAWSTATE::IDLE || m_state == LAWSTATE::ATTACK ||
        m_state == LAWSTATE::HIT_RECEIVED || m_state == LAWSTATE::MELEEATK)) return;
    //if (m_state == LAWSTATE::PARRY_ATK||m_state == LAWSTATE::GUARD || m_state == LAWSTATE::STUN) return;
    //if (m_state == LAWSTATE::REFLECTBULLET || m_state == LAWSTATE::PARRY_ATK || m_state == LAWSTATE::GUARD || m_state == LAWSTATE::STUN || m_state == LAWSTATE::KNOCKDOWN) return;

    UINT _Guard = rand() % 10;

    //가드
    if (_Guard == 9)
    {
        m_anim = false;
        m_state = LAWSTATE::PARRY_ATK;
        m_pattern = Pattern::START;
    }
    //가드 실패
    else
    {
        m_HitRed = true;

        m_anim = false;
        m_state = LAWSTATE::STUN;
        m_pattern = Pattern::START;
        m_fLawHp -= 20.f;
    }

}

void CLawScript::GunHitReceived() //************************************** 수정
{
    if (!(m_state == LAWSTATE::IDLE || m_state == LAWSTATE::ATTACK ||
        m_state == LAWSTATE::HIT_RECEIVED || m_state == LAWSTATE::MELEEATK)) return;
    //if (m_state == LAWSTATE::REFLECTBULLET || m_state == LAWSTATE::PARRY_ATK || m_state == LAWSTATE::GUARD || m_state == LAWSTATE::STUN || m_state == LAWSTATE::KNOCKDOWN) return;
    UINT _Guard = rand() % 10;

    //가드
    if (_Guard == 9)
    {
        m_anim = false;
        m_state = LAWSTATE::REFLECTBULLET;
        m_pattern = Pattern::START;
    }
    //가드 실패
    else
    {
        m_fLawHp -= 10.f;
        m_HitRed = true;
    }

}
void CLawScript::FullChargedGunHitReceived() //************************************** 수정
{
    if (!(m_state == LAWSTATE::IDLE || m_state == LAWSTATE::ATTACK ||
        m_state == LAWSTATE::HIT_RECEIVED || m_state == LAWSTATE::MELEEATK)) return;
    //if (m_state == LAWSTATE::REFLECTBULLET || m_state == LAWSTATE::PARRY_ATK || m_state == LAWSTATE::GUARD || m_state == LAWSTATE::STUN || m_state == LAWSTATE::KNOCKDOWN) return;
    UINT _Guard = rand() % 10;

    //가드
    if (_Guard == 9)
    {
        m_anim = false;
        m_state = LAWSTATE::REFLECTBULLET;
        m_pattern = Pattern::START;

    }
    //가드 실패
    else
    {
        m_fLawHp -= 20.f;
        m_HitRed = true;

        m_anim = false;
        m_state = LAWSTATE::KNOCKDOWN;
        m_pattern = Pattern::START;
    }

}
//******************************** 9/4 수정
bool CLawScript::CalCenterPos2(Vec3 _TelPos) //텔포때 검사해서 이동할 수 있는 위치인지 검사
{
    Vec2 _Telpos;
    _Telpos.x = _TelPos.x;
    _Telpos.y = _TelPos.z;

    Vec2 C_Dir = _Telpos - m_ColCenterPos;
    float dis = Vec2::Distance(_Telpos, m_ColCenterPos);
    m_Centerlength = C_Dir.Length();
    if (m_Centerlength >= m_fMaxDis)
    {
        return false;
    }

    return true;
}

//******************************** 9/4 수정
void CLawScript::CalCenterPos() //업데이트때마다 돌면서 위치 조정
{
    Vec2 C_Dir = Collider2D()->GetPos() - m_ColCenterPos;
    m_Centerlength = C_Dir.Length();
    float dis = Vec2::Distance(Collider2D()->GetPos(), m_ColCenterPos);

    if (dis >= m_fMaxDis) //_pOther->Collider2D()->GetScale().x / 2.f)
    {
        C_Dir.Normalize();
        float x = m_ColCenterPos.x + C_Dir.x * m_fMaxDis;// / 2.f;//_pOther->Collider2D()->GetScale().x / 2.f;

        if (abs(Collider2D()->GetPos().x) >= abs(x))
        {
            Transform()->SetLocalPosX(x);
        }
    }

    if (dis >= m_fMaxDis)// _pOther->Collider2D()->GetScale().y / 2.f)
    {
        C_Dir.Normalize();
        float y = m_ColCenterPos.y + C_Dir.y * m_fMaxDis;// / 2.f;// _pOther->Collider2D()->GetScale().y / 2.f;
        if (abs(Collider2D()->GetPos().y) >= abs(y))
        {
            Transform()->SetLocalPosZ(y);
        }
    }
}


void CLawScript::SaveToScene(FILE* _pFile)
{
    CScript::SaveToScene(_pFile);
}

void CLawScript::LoadFromScene(FILE* _pFile)
{
    CScript::LoadFromScene(_pFile);
}
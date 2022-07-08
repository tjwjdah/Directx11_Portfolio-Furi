#include "pch.h"
#include "CLawScript.h"

#include <Engine\CSceneMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CScene.h>
#include <Engine\CTransform.h>
#include <Engine\CScript.h>
#include <Engine\CEventMgr.h>
#include <Engine\CAnimator3D.h>
#include <Engine\CParticleSystem.h>

#include "CEffectScript.h"
#include "CPlayerScript.h"
#include "CBulletScript.h"
#include "CRadianDirBulletScript.h"
#include "CSlashScript.h"
#include "CYoyoScript.h"
#include "CScaleUpScript.h"

// 원거리
void CLawScript::ATK_GUNSHOT()
{
    Vec3 law_pos = Transform()->GetLocalPos();

    // test 2 -------------------
    //m_testang += 0.008;
    if (m_TarAngle < 0) m_RotAngle -= 0.009;
    else m_RotAngle += 0.009;

    law_pos.x += -m_RotAngle * fDT * 400.f;
    law_pos.z += -m_RotAngle * fDT * 400.f;

    Transform()->SetLocalPos(law_pos);
    // test 2 -------------------

    if (false == m_Didattack)// && m_BulletNum != 0)
    {
        Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"bullet");  //*********
        pSound->Play(1, 0.3f, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

        Vec3 mon_pos = Transform()->GetLocalPos();
        //CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        //Vec3 vPlayerPos = player->Transform()->GetLocalPos();
        
        CGameObject* pBullet = nullptr;
        pBullet = m_Attack2->Instantiate();
        pBullet->Transform()->SetLocalPos(mon_pos);
        
        CBulletScript* tar = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
        tar->SetChangeDir(false);
        tar->SetTarPos(m_PlayerPos);
        tar->SetSpeed(m_AttackSpeed);
        tar->SetMaxTime(20.f);
        tar->SetAttackNum(1);        //************************************** 수정

        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pBullet;
            even.wParam = (DWORD_PTR)4;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

            CEventMgr::GetInst()->AddEvent(even);
        }
        
        m_BulletNum--;
        m_Didattack = true;
        if (m_BulletNum == 0)
        {
            m_AttackCount -= 1;
            m_BulletNum = 7;
            if (m_AttackCount == 0)
            {
                m_Didattack = false;
                m_pattern = Pattern::END;
                m_anim = false;
                m_ftime = 0.f;
            }
        }
    }

    m_ftime += fDT;
    if (m_ftime > 0.15f)
    {
        m_Didattack = false;
        m_ftime = 0.f;
    }
}

void CLawScript::ATK_GUNSHOT_SWIPE()
{
    if (false == m_Didattack)// && m_BulletNum != 0)
    {
        Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"bullet");  //*********
        pSound->Play(1, 0.3f, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

        // 1 ---
        float target_angle = atan2f(m_TargetVec.y, m_TargetVec.x) + 1.57f;
        CGameObject* pBullet = nullptr;
        pBullet = m_Attack2->Instantiate();
        CBulletScript* tar = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
        tar->SetAttackNum(2);         //************************************** 수정
        tar->SetChangeDir(true);
        tar->SetTarPos(m_StartPos);
        tar->SetSpeed(m_AttackSpeed);
        //m_AttackSpeed += 5.f; //속도 조절

        float r = 20.f; //반지름
        float ang = 0.f; //시작 각도
        float accang = 0.34f; //10 //총알사이의 각

        if (m_AttackCount == 1)
        {
            ang = 3.14;
            accang = -0.34f;
        }
        Vec3 bulletpos = Vec3(r * cosf(target_angle + ang - accang * m_BulletNum) + m_StartPos.x, 1.f, r * sinf(target_angle + ang - accang * m_BulletNum) + m_StartPos.z);  //-
        pBullet->Transform()->SetLocalPos(bulletpos);
        tar->SetRadian(3.14 + 0.174 * m_BulletNum);//6.f * i);
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pBullet;
            even.wParam = (DWORD_PTR)4;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

            CEventMgr::GetInst()->AddEvent(even);
        }

        // 2 ----
        //float target_angle = atan2f(m_TargetVec.y, m_TargetVec.x) + 1.57f;
        CGameObject* pBullet2 = nullptr;
        pBullet2 = m_Attack2->Instantiate();
        CBulletScript* tar2 = (CBulletScript*)pBullet2->GetScript(L"CBulletScript");
        tar2->SetAttackNum(2);         //************************************** 수정
        tar2->SetChangeDir(true);
        tar2->SetTarPos(m_StartPos);
        tar2->SetSpeed(m_AttackSpeed * 2.f);
        //m_AttackSpeed += 5.f; //속도 조절

        r = 20.f; //반지름
        ang = 0.f; //시작 각도
        accang = 0.34f; //10 //총알사이의 각

        if (m_AttackCount == 1)
        {
            ang = 3.14;
            accang = -0.34f;
        }
        Vec3 bulletpos2 = Vec3(r * cosf(target_angle + ang - accang * m_BulletNum) + m_StartPos.x, 1.f, r * sinf(target_angle + ang - accang * m_BulletNum) + m_StartPos.z);  //-
        pBullet2->Transform()->SetLocalPos(bulletpos2);
        tar2->SetRadian(3.14 + 0.174 * m_BulletNum);//6.f * i);
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pBullet2;
            even.wParam = (DWORD_PTR)4;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

            CEventMgr::GetInst()->AddEvent(even);
        }


        m_BulletNum--;
        m_Didattack = true;
        if (m_BulletNum == 0)
        {
            m_AttackCount -= 1;
            m_BulletNum = 7;
            if (m_AttackCount == 0)
            {
                m_Didattack = false;
                m_pattern = Pattern::END;
                m_anim = false;
                m_ftime = 0.f;
            }
        }
    }

    m_ftime += fDT;
    if (m_ftime > 0.2)
    {
        m_Didattack = false;
        m_ftime = 0.f;
    }
}

void CLawScript::ATK_ANGULAR_WAVE()
{
    if (!m_anim)
    {
        Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"angularwave93");  //*********
        pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

        m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_angularWave", false);
        m_anim = true;
    }

    float ani_ratio = m_AniObj->Animator3D()->GetAnimFrmRatio();
    //int ret = PlayAnim(L"NPC_Law=stance1_angularWave", false, Pattern::NONE);

    if (ani_ratio >= 0.49f && false == m_Didattack)// && m_BulletNum != 0)
    {
        Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"angularwave_cast188");  //*********
        pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        m_PlayerPos = player->Transform()->GetLocalPos();
        LookAt(m_PlayerPos);

        CGameObject* pBullet = nullptr;
        pBullet = m_Attack3->Instantiate();
        pBullet->SetName(L"Sector");  /////////////////////////////////////////
        Vec3 mon_pos = Transform()->GetLocalPos();
        //CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        //Vec3 vPlayerPos = player->Transform()->GetLocalPos();


        pBullet->Transform()->SetLocalPos(Vec3(mon_pos));
        pBullet->Transform()->SetLocalRot(Vec3(0.f, m_TarAngle, 0.f));

        CBulletScript* tar = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
        tar->SetChangeDir(false);
        tar->SetTarPos(m_PlayerPos);
        tar->SetSpeed(m_AttackSpeed);
        tar->SetAttackNum(3);  //************************************** 수정
        tar->SetMaxTime(1.5f);
        tar->SetSectorInfo(1.57f, 0.12f);
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pBullet;
            even.wParam = (DWORD_PTR)2;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

            CEventMgr::GetInst()->AddEvent(even);
        }

        /*
       CGameObject* m_SwordCharge = new CGameObject;
       m_SwordCharge->SetName(L"Particle");

       m_SwordCharge->AddComponent(new CTransform);
       m_SwordCharge->AddComponent(new CParticleSystem);

       m_SwordCharge->Transform()->SetLocalPosZ(0.3f);
       m_SwordCharge->ParticleSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
       m_SwordCharge->ParticleSystem()->SetfMinLifeTime(1.f);
       m_SwordCharge->ParticleSystem()->SetfMaxLifeTime(1.f);
       m_SwordCharge->ParticleSystem()->SetfMinSpeed(40.f);
       m_SwordCharge->ParticleSystem()->SetfMaxSpeed(60.f);
       m_SwordCharge->ParticleSystem()->SetfFrequency(0.001f);
       m_SwordCharge->ParticleSystem()->SetvCreateRange(Vec4(1500.f, 100.f, 100.f, 1.f));
       m_SwordCharge->ParticleSystem()->SetvEndColor(Vec4(0.0f, 0.2f, 0.7f, 0.8f));
       m_SwordCharge->ParticleSystem()->SetvStartColor(Vec4(0.0f, 0.124f, 0.5f, 0.3f));
       m_SwordCharge->ParticleSystem()->SetvStartScale(Vec3(20.f, 20.f, 1.f));
       m_SwordCharge->ParticleSystem()->SetvEndScale(Vec3(20.f, 20.f, 1.f));
      
       m_SwordCharge->ParticleSystem()->SetDirection(0);
       {
           tEvent even = {};
           even.eEvent = EVENT_TYPE::ADD_CHILD;
           even.lParam = (DWORD_PTR)pBullet;
           even.wParam = (DWORD_PTR)m_SwordCharge;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

           CEventMgr::GetInst()->AddEvent(even);
       }
       
       */
        m_Didattack = true;
        m_AttackCount--;

    }

    if (true == m_AniObj->Animator3D()->GetFinish())
    {
        m_Didattack = false;
        m_pattern = Pattern::END;
        m_anim = false;
        m_ftime = 0.f;

    }

}


// 중간
void CLawScript::ATK_WIDE_SLASH_RTOL() //collidercheck 수정
{
    float ani_ratio = m_AniObj->Animator3D()->GetAnimFrmRatio();
    bool ret = m_AniObj->Animator3D()->GetFinish();
    
    if (!m_Didattack && ani_ratio > 0.4)
    {
        Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"wideslashRL");  //*********
        pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

        Vec3 law_pos = Transform()->GetLocalPos();
        
        CGameObject* pBullet = nullptr;
        pBullet = m_Attack6->Instantiate();
        pBullet->Transform()->SetLocalPos(law_pos);
        pBullet->Transform()->SetLocalScale(Vec3(1100.f, 1100.f, 1100.f));

        CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
        tar->SetTarPos(m_PlayerPos2);
        tar->SetMaxTime(1.f);
        //tar->SetRange(0.f);
        tar->SetAttackNum(1);//************************************** 수정
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pBullet;
            even.wParam = (DWORD_PTR)2;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

            CEventMgr::GetInst()->AddEvent(even);
        }
        
        m_Didattack = true;
        //m_AttackCount--;
    }

    if (ret)//m_ftime > m_fAttackTime)
    {
        m_Didattack = false;
        m_pattern = Pattern::END;
        m_anim = false;
        m_ftime = 0.f;
    }
}

void CLawScript::ATK_CIRCLE_SHOOT()
{
    if (!m_bPortalringOn)
    {
        portal_ring = CResMgr::GetInst()->FindRes<CPrefab>(L"portal_ring_twisty")->Instantiate();
        portal_ring->Transform()->SetLocalPos(GetObj()->Transform()->GetLocalPos());
        tEvent even = {};
        even.eEvent = EVENT_TYPE::CREATE_OBJECT;
        even.lParam = (DWORD_PTR)portal_ring;
        even.wParam = (DWORD_PTR)2;
        CEventMgr::GetInst()->AddEvent(even);
        m_bPortalringOn = true;
    }
    if (false == m_Didattack)// && m_BulletNum != 0)
    {
        CGameObject* pObject[30];
        Vec3 mon_pos = Transform()->GetLocalPos();
        for (int i = 0; i < m_BulletNum; i++)
        {
            pObject[i] = m_Attack1->Instantiate();
            pObject[i]->Transform()->SetLocalPos(mon_pos);

            CRadianDirBulletScript* tar = (CRadianDirBulletScript*)pObject[i]->GetScript(L"CRadianDirBulletScript");
            tar->SetRadian(3.26 + 0.1 * i);
            tar->SetMaxTime(8.f);
        }

        for (int i = 0; i < m_BulletNum; i++)
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pObject[i];
            even.wParam = (DWORD_PTR)4;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

            CEventMgr::GetInst()->AddEvent(even);
        }

        m_Didattack = true;
        m_AttackCount -= 1;
        if (m_AttackCount == 0)
        {
            Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"angularwave93");  //*********
            pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)
            CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
            m_PlayerPos = player->Transform()->GetLocalPos();
            LookAt(m_PlayerPos);
            // sector
            CGameObject* pBullet = nullptr;
            pBullet = m_Attack3->Instantiate();
            pBullet->SetName(L"Sector");
            Vec3 mon_pos = Transform()->GetLocalPos();
            Vec3 vColor = Vec3(1.f, 0.f,0.f);
            
            pBullet->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor.x);
            pBullet->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor.y);
            pBullet->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor.z);
            pBullet->Transform()->SetLocalPos(Vec3(mon_pos));
            //pBullet->Transform()->SetLocalScale(Vec3(500.f,500.f,500.f));
            pBullet->Transform()->SetLocalRot(Vec3(0.f, m_TarAngle, 0.f));

            CBulletScript* tar = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
            //CScaleUpScript* scal = (CScaleUpScript*)pBullet->GetScript(L"CScaleUpScript");
            //scal->Setvalue(1.2f);
            tar->SetChangeDir(false);
            tar->SetTarPos(m_PlayerPos);
            tar->SetSpeed(m_AttackSpeed);
            tar->SetAttackNum(3);
            tar->SetMaxTime(3.0f);
            tar->SetSectorInfo(3.14f, 0.18f);
            {
                tEvent even = {};
                even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                even.lParam = (DWORD_PTR)pBullet;
                even.wParam = (DWORD_PTR)2;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

                CEventMgr::GetInst()->AddEvent(even);
            }
            // sector

            if (m_iMeleeCount <= 0)
            {
                m_Didattack = false;
                m_pattern = Pattern::END;
                m_anim = false;
                m_ftime = 0.f;
                portal_ring->Transform()->SetLocalPos(Vec3(999999.f, 999999.f, 999999.f));
                CEffectScript* BulletS = (CEffectScript*)portal_ring->GetScript(L"CEffectScript");
                m_bPortalringOn = false;
                BulletS->SetTime(3.0f);
                BulletS->SetStopTime(3.0f);
                portal_ring = nullptr;
                return;
            }

            m_Didattack = false;
            m_pattern = Pattern::START;
            m_anim = false;
            m_ftime = 0.f;
        }
    }

    m_ftime += fDT;
    if (m_ftime > 0.5)
    {
        m_Didattack = false;
        m_ftime = 0.f;
    }

    /*
    if (false == m_Didattack)// && m_BulletNum != 0)
    {
        CGameObject* pObject[30];
        Vec3 mon_pos = Transform()->GetLocalPos();
        for (int i = 0; i < m_BulletNum; i++)
        {
            pObject[i] = m_Attack1->Instantiate();
            pObject[i]->Transform()->SetLocalPos(mon_pos);

            CRadianDirBulletScript* tar = (CRadianDirBulletScript*)pObject[i]->GetScript(L"CRadianDirBulletScript");
            tar->SetRadian(3.26 + 0.1 * i);
            tar->SetMaxTime(5.f);
        }

        for (int i = 0; i < m_BulletNum; i++)
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pObject[i];
            even.wParam = (DWORD_PTR)4;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

            CEventMgr::GetInst()->AddEvent(even);
        }

        m_Didattack = true;
        m_AttackCount -= 1;
        if (m_AttackCount == 0)
        {
            // sector
            CGameObject* pBullet = nullptr;
            pBullet = m_Attack3->Instantiate();
            pBullet->SetName(L"Sector");
            Vec3 mon_pos = Transform()->GetLocalPos();

            pBullet->Transform()->SetLocalPos(Vec3(mon_pos));
            //pBullet->Transform()->SetLocalScale(Vec3(500.f,500.f,500.f));
            pBullet->Transform()->SetLocalRot(Vec3(0.f, m_TarAngle, 0.f));

            CBulletScript* tar = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
            //CScaleUpScript* scal = (CScaleUpScript*)pBullet->GetScript(L"CScaleUpScript");
            //scal->Setvalue(1.2f);
            tar->SetChangeDir(false);
            tar->SetTarPos(m_PlayerPos);
            tar->SetSpeed(m_AttackSpeed);
            tar->SetAttackNum(3);
            tar->SetMaxTime(1.5f);
            tar->SetSectorInfo(3.14f, 0.18f);
            {
                tEvent even = {};
                even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                even.lParam = (DWORD_PTR)pBullet;
                even.wParam = (DWORD_PTR)2;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

                CEventMgr::GetInst()->AddEvent(even);
            }
            // sector


            m_Didattack = false;
            m_pattern = Pattern::END;
            m_anim = false;
            m_ftime = 0.f;
        }
    }

    m_ftime += fDT;
    if (m_ftime > 0.5)
    {
        m_Didattack = false;
        m_ftime = 0.f;
    }
    */
}

void CLawScript::ATK_YOYO() //************************************** 수정, attack4 프리펩 수정
{
    if (!m_anim)
    {
        m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance2_Yoyo1Start", false);
        m_anim = true;
    }

    float ret = m_AniObj->Animator3D()->GetAnimFrmRatio();

    if (false == m_Didattack && ret >= 0.55)// && m_BulletNum != 0)
    {
        Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"yoyo");  //*********
        pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

        Vec3 law_pos = Transform()->GetLocalPos();
        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        m_PlayerPos = player->Transform()->GetLocalPos();
        LookAt(m_PlayerPos);

        CGameObject* pBullet = nullptr;
        pBullet = m_Attack4->Instantiate();
        pBullet->Transform()->SetLocalPos(law_pos);
        pBullet->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));

        CYoyoScript* tar = (CYoyoScript*)pBullet->GetScript(L"CYoyoScript");
        tar->SetDis(900.f);
        tar->SetTarPos(m_PlayerPos);
        tar->SetSpeed(m_AttackSpeed);
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pBullet;
            even.wParam = (DWORD_PTR)2;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

            CEventMgr::GetInst()->AddEvent(even);
        }

        m_Didattack = true;
        m_AttackCount--;
    }

    m_ftime += fDT;
    if (m_ftime > m_fAttackTime)
    {
        m_pattern = Pattern::END;
        m_Didattack = false;
        m_anim = false;
        m_ftime = 0.f;
    }

}

void CLawScript::ATK_WIDE_SLASH_STICK()
{
    float ani_ratio = m_AniObj->Animator3D()->GetAnimFrmRatio();
    bool ret = m_AniObj->Animator3D()->GetFinish();
    if (!m_Didattack && ani_ratio > 0.25)
    {
        Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"wideslashRL");  //*********
        pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

        Vec3 law_pos = Transform()->GetLocalPos();
        //CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        //m_PlayerPos = player->Transform()->GetLocalPos();
        //LookAt(m_PlayerPos);

        CGameObject* pBullet = nullptr;
        pBullet = m_Attack6->Instantiate();
        pBullet->Transform()->SetLocalPos(law_pos);
        pBullet->Transform()->SetLocalScale(Vec3(1100.f, 1100.f, 1100.f));

        CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
        tar->SetTarPos(m_PlayerPos2);
        tar->SetMaxTime(1.f);
        //tar->SetRange(0.f);
        tar->SetAttackNum(1);//************************************** 수정
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pBullet;
            even.wParam = (DWORD_PTR)2;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();

            CEventMgr::GetInst()->AddEvent(even);
        }

        m_Didattack = true;
        //m_AttackCount--;
    }

    if (ret)//m_ftime > m_fAttackTime)
    {
        m_Didattack = false;
        m_pattern = Pattern::END;
        m_anim = false;
        m_ftime = 0.f;
    }
}

// 근접
void CLawScript::ATK_SWIPE_SLASH_COMBO_A()
{
    float m_test2[4] = { 0.25f,0.5f,0.7f,0.95f };
    float ani_ratio = m_AniObj->Animator3D()->GetAnimFrmRatio();
    if (ani_ratio >= m_test2[m_iMeleeCount])
    {
        if (m_iMeleeCount > 3)
        {
            m_Didattack = true;
            m_iMeleeCount = 3;
        }
        m_fCloserTime = 0.f;
        m_Didattack = false;
        m_iMeleeCount++;
    }
    if (!m_Didattack && m_AttackCount > 0)
    {
        Vec3 law_pos = Transform()->GetLocalPos();
        CGameObject* pBullet = nullptr;
        

        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        m_PlayerPos = player->Transform()->GetLocalPos();
        LookAt(m_PlayerPos);

        bool ret1 = false;
        float _dis = CalTargetDis(m_PlayerPos);
        m_testTardistance = _dis;
        if (_dis > 130.f)
        {
            ret1 = GAP_CLOSER(110.f, 0.25f);
            if (ret1)
            {
                pBullet = m_Attack5->Instantiate();
                pBullet->Transform()->SetLocalPos(law_pos);
                pBullet->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));
                Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"melee_whoosh168");  //*********
                pSound->Play(1, 0.3f, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

                CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
                tar->SetTarPos(m_PlayerPos);
                tar->SetMaxTime(0.2f);
                tar->SetRange(70.f);
                {
                    tEvent even = {};
                    even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                    even.lParam = (DWORD_PTR)pBullet;
                    even.wParam = (DWORD_PTR)2;
                    CEventMgr::GetInst()->AddEvent(even);
                }
                m_AttackCount--;
                m_Didattack = true;
            }
            else
            {
                m_Didattack = false;
            }
        }
        else
        {
            Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"melee_whoosh181");  //*********
            pSound->Play(1, 0.3, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)
            pBullet = m_Attack5->Instantiate();
            pBullet->Transform()->SetLocalPos(law_pos);
            pBullet->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));
            CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
            tar->SetTarPos(m_PlayerPos);
            tar->SetMaxTime(0.2f);
            tar->SetRange(70.f);
            {
                tEvent even = {};
                even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                even.lParam = (DWORD_PTR)pBullet;
                even.wParam = (DWORD_PTR)2;
                CEventMgr::GetInst()->AddEvent(even);
            }
            m_AttackCount--;
            m_Didattack = true;
        }
    }

    bool ret = m_AniObj->Animator3D()->GetFinish();
    if (ret && m_AttackCount <= 0)
    {
        m_fCloserTime = 0.f;
        m_Didattack = false;
        m_pattern = Pattern::END;
        m_anim = false;
        m_ftime = 0.f;
        m_iMeleeCount = 0;
        m_AttackCount = 0;

    }
}

void CLawScript::ATK_SWIPE_SLASH_COMBO_B()
{

    bool ret = m_AniObj->Animator3D()->GetFinish();
    if (!m_Didattack)
    {
        Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"melee_whoosh181");  //*********
        pSound->Play(1, 0.3, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

        Vec3 law_pos = Transform()->GetLocalPos();
        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        m_PlayerPos = player->Transform()->GetLocalPos();
        LookAt(m_PlayerPos);

        CGameObject* pBullet = nullptr;
        pBullet = m_Attack5->Instantiate();
        pBullet->Transform()->SetLocalPos(law_pos);
        pBullet->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));

        CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
        tar->SetTarPos(m_PlayerPos);
        tar->SetMaxTime(0.4f);
        tar->SetRange(200.f);
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pBullet;
            even.wParam = (DWORD_PTR)2;

            CEventMgr::GetInst()->AddEvent(even);
        }

        m_Didattack = true;
        //m_AttackCount--;
    }

    //m_ftime += fDT;
    if (ret)//m_ftime > m_fAttackTime)
    {
        m_fCloserTime = 0.f;
        m_Didattack = false;
        m_pattern = Pattern::END;
        m_anim = false;
        m_ftime = 0.f;
    }
}

void CLawScript::ATK_SWIPE_SLASH_COMBO_C()
{
    float m_test2[4] = { 0.2f,0.45f,0.73f,0.1f };
    float ani_ratio = m_AniObj->Animator3D()->GetAnimFrmRatio();
    if (ani_ratio >= m_test2[m_iMeleeCount])
    {
        if (m_iMeleeCount > 3)
        {
            m_Didattack = true;
            m_iMeleeCount = 3;
        }
        m_fCloserTime = 0.f;
        m_Didattack = false;
        m_iMeleeCount++;

    }
    if (!m_Didattack && m_AttackCount > 0)
    {
        Vec3 law_pos = Transform()->GetLocalPos();
        CGameObject* pBullet = nullptr;
        

        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        m_PlayerPos = player->Transform()->GetLocalPos();
        LookAt(m_PlayerPos);

        bool ret1 = false;
        float _dis = CalTargetDis(m_PlayerPos);
        if (_dis > 130.f)
        {
            ret1 = GAP_CLOSER(110.f, 0.25f);
            if (ret1)
            {
                pBullet = m_Attack5->Instantiate();
                pBullet->Transform()->SetLocalPos(law_pos);
                pBullet->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));
                Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"melee_whoosh200");  //*********
                pSound->Play(1, 0.3, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)


                CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
                tar->SetTarPos(m_PlayerPos);
                tar->SetMaxTime(0.2f);
                tar->SetRange(70.f);
                {
                    tEvent even = {};
                    even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                    even.lParam = (DWORD_PTR)pBullet;
                    even.wParam = (DWORD_PTR)2;
                    CEventMgr::GetInst()->AddEvent(even);
                }
                m_AttackCount--;
                m_Didattack = true;
            }

        }
        else
        {
            pBullet = m_Attack5->Instantiate();
            pBullet->Transform()->SetLocalPos(law_pos);
            pBullet->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));
            Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"melee_whoosh191");  //*********
            pSound->Play(1, 0.3, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

            CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
            tar->SetTarPos(m_PlayerPos);
            tar->SetMaxTime(0.2f);
            tar->SetRange(70.f);
            {
                tEvent even = {};
                even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                even.lParam = (DWORD_PTR)pBullet;
                even.wParam = (DWORD_PTR)2;
                CEventMgr::GetInst()->AddEvent(even);
            }
            m_AttackCount--;
            m_Didattack = true;
        }
    }

    bool ret = m_AniObj->Animator3D()->GetFinish();
    if (ret && m_AttackCount <= 0)
    {
        m_fCloserTime = 0.f;
        m_Didattack = false;
        m_pattern = Pattern::END;
        m_anim = false;
        m_ftime = 0.f;
        m_iMeleeCount = 0;
        m_AttackCount = 0;
    }
}

void CLawScript::ATK_SWIPE_SLASH_COMBO_D()
{
    float m_test2[4] = { 0.3f,0.7f,0.88f,0.99f };
    float ani_ratio = m_AniObj->Animator3D()->GetAnimFrmRatio();
    if (ani_ratio >= m_test2[m_iMeleeCount])
    {
        if (m_iMeleeCount > 3)
        {
            m_Didattack = true;
            m_iMeleeCount = 3;
        }
        m_fCloserTime = 0.f;
        m_Didattack = false;
        m_iMeleeCount++;
    }

    if (!m_Didattack && m_AttackCount > 0)
    {
        Vec3 law_pos = Transform()->GetLocalPos();
        CGameObject* pBullet = nullptr;
       

        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        m_PlayerPos = player->Transform()->GetLocalPos();
        LookAt(m_PlayerPos);

        bool ret1 = false;
        float _dis = CalTargetDis(m_PlayerPos);
        if (_dis > 130.f)
        {
            ret1 = GAP_CLOSER(110.f, 0.2f);
            if (ret1)
            {
                pBullet = m_Attack5->Instantiate();
                pBullet->Transform()->SetLocalPos(law_pos);
                pBullet->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));
                Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"melee_whoosh184");  //*********
                pSound->Play(1, 0.3, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)


                CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
                tar->SetTarPos(m_PlayerPos);
                tar->SetMaxTime(0.2f);
                tar->SetRange(70.f);
                {
                    tEvent even = {};
                    even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                    even.lParam = (DWORD_PTR)pBullet;
                    even.wParam = (DWORD_PTR)2;
                    CEventMgr::GetInst()->AddEvent(even);
                }
                m_AttackCount--;
                m_Didattack = true;
            }
        }
        else
        {
            Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"melee_whoosh200");  //*********
            pSound->Play(1, 0.3, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)
            pBullet = m_Attack5->Instantiate();
            pBullet->Transform()->SetLocalPos(law_pos);
            pBullet->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));
            CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
            tar->SetTarPos(m_PlayerPos);
            tar->SetMaxTime(0.2f);
            tar->SetRange(70.f);
            {
                tEvent even = {};
                even.eEvent = EVENT_TYPE::CREATE_OBJECT;
                even.lParam = (DWORD_PTR)pBullet;
                even.wParam = (DWORD_PTR)2;
                CEventMgr::GetInst()->AddEvent(even);
            }
            m_AttackCount--;
            m_Didattack = true;
        }
    }

    bool ret = m_AniObj->Animator3D()->GetFinish();
    if (ret) //&& m_AttackCount <= 0)
    {
        m_fCloserTime = 0.f;
        m_Didattack = false;
        m_pattern = Pattern::END;
        m_anim = false;
        m_ftime = 0.f;
        m_iMeleeCount = 0;
        m_AttackCount = 0;
    }
}

void CLawScript::ATK_PARRY()
{
    if (!m_anim)
    {
        m_AniObj->Animator3D()->ChangeClip(L"NPC_Law=stance1_Parry4CounterAtkEnd", false, 2.f);
        m_anim = true;
    }
    float ret1 = m_AniObj->Animator3D()->GetAnimFrmRatio();

    //패링공격
    if (false == m_Didattack && ret1 >= 0.33)// && m_BulletNum != 0)
    {
        Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"impact_hurt259");  //*********
        pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

        Vec3 law_pos = Transform()->GetLocalPos();
        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        m_PlayerPos = player->Transform()->GetLocalPos();
        LookAt(m_PlayerPos);

        CGameObject* pBullet = nullptr;
        pBullet = m_Attack5->Instantiate();
        pBullet->Transform()->SetLocalPos(law_pos);
        pBullet->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));

        CSlashScript* tar = (CSlashScript*)pBullet->GetScript(L"CSlashScript");
        tar->SetTarPos(m_PlayerPos);
        tar->SetMaxTime(0.2f);
        tar->SetRange(200.f);
        tar->SetAttackNum(4);
        {
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pBullet;
            even.wParam = (DWORD_PTR)2;

            CEventMgr::GetInst()->AddEvent(even);
        }

        m_Didattack = true;
    }


    //끝
    bool ret = m_AniObj->Animator3D()->GetFinish();
    if (ret) // && m_AttackCount <= 0)
    {
        m_pattern = Pattern::NONE;
        m_state = LAWSTATE::NONE;
        m_ftime = 0.f;
        m_anim = false;
        m_Didattack = false;
    }


    /*
    m_ftime += fDT;
    if (m_ftime > 2.f)
    {
        m_pattern = Pattern::NONE;
        m_state = LAWSTATE::NONE;
        m_ftime = 0.f;
        m_anim = false;
        m_Didattack = false;
    }
    */

}

void CLawScript::ATK_GRAB()
{

    m_ftime += fDT;
    if (m_ftime > 1.1 && !m_Didattack)
    {
        // ===== Effect ====== //(22.1.5)
        {
            Vec3 _pos = Transform()->GetLocalPos();
            CGameObject* GunCharge = new CGameObject;
            GunCharge->SetName(L"22LightningEffect");
            GunCharge->AddComponent(new CTransform);
            GunCharge->AddComponent(new CParticleSystem);
            GunCharge->AddComponent(new CEffectScript);
            GunCharge->Transform()->SetLocalPos(Vec3(_pos.x, _pos.y + 350.f, _pos.z));
            GunCharge->ParticleSystem()->SetPaticleName(L"elec2");
            GunCharge->ParticleSystem()->SetShaderIdx(1);
            GunCharge->ParticleSystem()->SetiMaxCount(50);
            GunCharge->ParticleSystem()->SetvCreateRange(Vec4(5000.f, 200.f, 5000.f, 0.f)); //범위조절
            GunCharge->ParticleSystem()->SetvStartColor(Vec4(0.87f, 0.224f, 0.0f, 0.8f));
            GunCharge->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.0f, 1.0f, 1.f));
            GunCharge->ParticleSystem()->SetfMinSpeed(20);
            GunCharge->ParticleSystem()->SetfMaxSpeed(50);
            GunCharge->ParticleSystem()->SetvStartScale(Vec3(750.f, 0.f, 0.f));
            GunCharge->ParticleSystem()->SetvEndScale(Vec3(800.f, 0.f, 0.f));
            GunCharge->ParticleSystem()->SetDirection(0);
            GunCharge->ParticleSystem()->SetfMinLifeTime(0.3f);
            GunCharge->ParticleSystem()->SetfMaxLifeTime(0.4f);
            GunCharge->ParticleSystem()->SetfFrequency(0.03f);

            CEffectScript* BulletS = (CEffectScript*)GunCharge->GetScript(L"CEffectScript");
            BulletS->SetType(EFFECT::DASH);
            BulletS->SetTime(5.0f);
            BulletS->SetStopTime(6.3f);

            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)GunCharge;
            even.wParam = (DWORD_PTR)2;
            CEventMgr::GetInst()->AddEvent(even);

            m_Didattack = true;
        }
        // ===== ~ Effect ======

    }

    //bool IsFin = m_AniObj->Animator3D()->GetFinish();
    //if (IsFin)
    //{
    //    CGameObject* Obj = CSceneMgr::GetInst()->FindObjectByName(L"Player");
    //    CPlayerScript* Player = (CPlayerScript*)Obj->GetScript(L"CPlayerScript");
    //    bool IsParry = Player->Grap();

    //    m_pattern = Pattern::END;
    //    m_anim = false;
    //    m_ftime = 0.f;
    //}
}
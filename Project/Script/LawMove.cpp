#include "pch.h"
#include "CLawScript.h"

#include <Engine\CSceneMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CScene.h>
#include <Engine\CTransform.h>
#include <Engine\CScript.h>
#include <Engine/CAnimator3D.h>

#include <Engine\CParticleSystem.h>
#include <Engine\CEventMgr.h>
#include "CEffectScript.h"
void CLawScript::MOVE_TELPO(Vec3 _Destination)
{
    m_ftime += fDT;
    if (m_ftime > m_fMoveTime)
    {
        Transform()->SetLocalPos(_Destination);// Vec3(vPlayerPos.x, 100.f, vPlayerPos.z + 1000.f));

        {////////// telpo down ////=========================================
            Vec3 _pos = Transform()->GetLocalPos();

            // ===================
            // Telpo Effect (22.1.3)
            // ===================  
            CGameObject* pRain = new CGameObject;
            pRain->SetName(L"TelpoEffect");
            pRain->AddComponent(new CTransform);
            pRain->AddComponent(new CParticleSystem);
            pRain->AddComponent(new CEffectScript);

            pRain->Transform()->SetLocalPos(Vec3(_pos.x - 62.f, -100.f, _pos.z - 100.f));
            pRain->ParticleSystem()->SetPaticleName(L"flak2");
            pRain->ParticleSystem()->SetShaderIdx(2);
            pRain->ParticleSystem()->SetfMinLifeTime(0.1f);
            pRain->ParticleSystem()->SetfMaxLifeTime(0.2f);
            pRain->ParticleSystem()->SetfFrequency(0.03f);
            pRain->ParticleSystem()->SetvCreateRange(Vec4(100.f, 500.f, 100.f, 0.f));
            pRain->ParticleSystem()->SetvStartColor(Vec4(1.f, 1.f, 0.5f, 1.0f));
            pRain->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.f, 1.f, 1.f));
            pRain->ParticleSystem()->SetvStartScale(Vec3(180.f, 0.f, 0.f));
            pRain->ParticleSystem()->SetvEndScale(Vec3(10.f, 0.f, 0.f));
            pRain->ParticleSystem()->SetfMinSpeed(30.f);
            pRain->ParticleSystem()->SetfMaxSpeed(30.f);
            pRain->ParticleSystem()->SetiMaxCount(100);
            pRain->ParticleSystem()->SetDirection(-1);

            CEffectScript* BulletS = (CEffectScript*)pRain->GetScript(L"CEffectScript");
            BulletS->SetType(EFFECT::NONE);
            BulletS->SetTime(1.f);
            BulletS->SetStopTime(0.7f);

            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)pRain;
            even.wParam = (DWORD_PTR)2;
            CEventMgr::GetInst()->AddEvent(even);

        }////================================================================


        m_TelpoCount--;
        // if (m_TelpoCount <= 0)
        {
            m_ftime = 0.f;
            m_pattern = Pattern::END;
            m_anim = false;

        }

    }

}

void CLawScript::MOVE_TELPO()
{
    m_ftime += fDT;
    if (m_ftime > m_fMoveTime)
    {
        CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        Vec3 vPlayerPos = player->Transform()->GetLocalPos();
        //플레이어 dir 구해서 switch 처리 할것
        //Vec3 vMonPos = Vec3(vPlayerPos.x + 50.f, 100.f, vPlayerPos.z);
        Transform()->SetLocalPos(GetRandomLawPos(120, 250));//Vec3(vPlayerPos.x + 100.f, 100.f, vPlayerPos.z));
        m_ftime = 0.f;
        m_pattern = Pattern::END;
        m_anim = false;
    }

}

void CLawScript::GAP_CLOSER()
{

    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
    Vec3 vPlayerPos = player->Transform()->GetLocalPos();
    Vec3 vLawPos = Transform()->GetLocalPos();

    Vec2 vTarVec = Vec2(vPlayerPos.x - vLawPos.x, vPlayerPos.z - vLawPos.z);
    float _dis = vTarVec.Length();

    if (_dis >= m_fMinDis)
    {
        vTarVec.Normalize();

        vLawPos.x += vTarVec.x * fDT * m_fspeed;
        vLawPos.z += vTarVec.y * fDT * m_fspeed;
        Transform()->SetLocalPos(vLawPos);

        m_fspeed += 0.2f; //가속
    }
    else //일정 거리 이내일때
    {
        m_ftime = 0.f;
        m_pattern = Pattern::END;
        m_anim = false;
    }

    //또는 시간이 다 되었을ㄷ때
    m_ftime += fDT;
    if (m_ftime > m_fMoveTime)
    {
        m_ftime = 0.f;
        m_pattern = Pattern::START; //END;
        m_anim = false;
    }
}

bool CLawScript::GAP_CLOSER(float _dis, float _time) //공격시 이동 
{
    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
    Vec3 vPlayerPos = player->Transform()->GetLocalPos();
    Vec3 vLawPos = Transform()->GetLocalPos();

    Vec2 vTarVec = Vec2(vPlayerPos.x - vLawPos.x, vPlayerPos.z - vLawPos.z);
    float _distan = vTarVec.Length();

    m_fCloserTime += fDT;

    if (_dis <= _distan) //거리 구한것이 정해준 거리값보다 클때 가깝게 이동
    {
        vTarVec.Normalize();

        vLawPos.x += vTarVec.x * fDT * m_fspeed;
        vLawPos.z += vTarVec.y * fDT * m_fspeed;
        Transform()->SetLocalPos(vLawPos);

        m_fspeed += 0.33f; //가속
    }

    else if (_distan < _dis)
    {
        m_fCloserTime = 0.f;
        return true;
    }

    if (m_fCloserTime > _time)
    {
        m_fCloserTime = 0.f;
        return true;
    }
    return false;
}


void CLawScript::GAP_BACK()
{

    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
    Vec3 vPlayerPos = player->Transform()->GetLocalPos();
    Vec3 vLawPos = Transform()->GetLocalPos();

    Vec2 vTarVec = Vec2(vPlayerPos.x - vLawPos.x, vPlayerPos.z - vLawPos.z);
    vTarVec.Normalize();

    vLawPos.x += -vTarVec.x * fDT * m_fspeed;
    vLawPos.z += -vTarVec.y * fDT * m_fspeed;

    //이동 거리가 일정 이상이면 다음으로 넘어가도록 
    Transform()->SetLocalPos(vLawPos);

    m_ftime += fDT;
    if (m_ftime > m_fMoveTime)
    {
        m_ftime = 0.f;
        m_pattern = Pattern::START; ////일정 시간이 지나면 start로 가서 다시 검사
        m_anim = false;
    }

}


void CLawScript::MOVE_WALK()
{

    CGameObject* player = CSceneMgr::GetInst()->FindObjectByName(L"Player");
    Vec3 vPlayerPos = player->Transform()->GetLocalPos();
    Vec3 vLawPos = Transform()->GetLocalPos();

    Vec2 vTarVec = Vec2(vPlayerPos.x - vLawPos.x, vPlayerPos.z - vLawPos.z);
    vTarVec.Normalize();

    LookAt(vPlayerPos);  ////

    vLawPos.x += -vTarVec.x * fDT * 500.f;
    vLawPos.z += -vTarVec.y * fDT * 500.f;

    Transform()->SetLocalPos(vLawPos);

}

void CLawScript::MOVE_RUN() {}
void CLawScript::MOVE_DASH() {}


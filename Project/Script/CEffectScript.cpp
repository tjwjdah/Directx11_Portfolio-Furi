#include "pch.h"
#include "CEffectScript.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CParticleSystem.h>

CEffectScript::CEffectScript()
    : CScript((int)SCRIPT_TYPE::EFFECTSCRIPT)
    , m_Type(EFFECT::NONE)
    , m_vPos(Vec3(0.f, 0.f, 0.f))
    , m_vDir(Vec3(0.f, 0.f, 0.f))
    , m_vDir2(Vec3(0.f, 0.f, 0.f))
    , m_fSpeed(270.f)
    , m_fDeadTime(0.f)
    , m_fStopParTime(10.f)
    , m_fDeadTimeMax(5.f)
    , m_bAwake(true)
    , m_bDead(false)
    , m_pPlayer(nullptr)
    , m_bROT(false)
    , m_ScaleTime(0.f)
    , m_AddScale(Vec3(0.f, 0.f, 0.f))
    , m_RotScale(1.f)
{
 
}

CEffectScript::~CEffectScript()
{

}

void CEffectScript::update()
{
    m_vPos = Transform()->GetLocalPos();

    switch (m_Type)
    {
    case EFFECT::DASH:
    {
        if (m_bAwake)
        {
            m_bAwake = false;
            m_pPlayer = CSceneMgr::GetInst()->FindObjectByName(L"Player");
        }
        Vec3 vPlayerPos = m_pPlayer->Transform()->GetLocalPos();
        m_vPos.x = vPlayerPos.x;
        m_vPos.z = vPlayerPos.z;
    }
    break;
    case EFFECT::TELPO_UP:
    {
        m_vPos += m_vDir * fDT * m_fSpeed;
    }
    break;
    case EFFECT::TELPO_DOWN:
    {
        if (m_vPos.y <= 0)
            m_vPos += m_vDir2 * fDT * m_fSpeed;
        else
            m_vPos += m_vDir * fDT * m_fSpeed;
    }
    break;
    case EFFECT::HIT:
        break;
    case EFFECT::NONE:
        break;
    case EFFECT::ROT:
    {
        Vec3 rot = Transform()->GetLocalRot();
        if (m_bROT)
            rot.y += fDT * 20.f * m_RotScale;
        else
            rot.y -= fDT * 10.f* m_RotScale;

       Transform()->SetLocalRot(rot);
    }
        break;
    case EFFECT::SCALE:
    {
        m_ScaleTime += fDT;
        Transform()->SetLocalScale(m_vBaseScale +(m_AddScale * m_ScaleTime));
        if (m_ScaleMaxTime < m_ScaleTime)
        {
            m_ScaleTime = 0.f;
        }
     }
        break;
    default:
        break;
    }

    m_fDeadTime += fDT;
    if (m_fDeadTime >= m_fStopParTime)
    {
        if(GetObj()->ParticleSystem())
        GetObj()->ParticleSystem()->SetTest(true);
        if (m_fDeadTime >= m_fDeadTimeMax + 0.1f)
        {
            if (!m_bDead)
            {
                m_bDead = true;
                DeleteObject(GetObj());
            }
        }
    }

    Transform()->SetLocalPos(m_vPos);
}
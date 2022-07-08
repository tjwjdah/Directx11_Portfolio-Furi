#include "pch.h"
#include "CSlashScript.h"

#include <Engine\CTimeMgr.h>
#include <Engine\CEventMgr.h>
#include <Engine\CGameObject.h>

#include <Engine/CSceneMgr.h>

#include "CPlayerScript.h"

CSlashScript::CSlashScript()
    :CScript((int)SCRIPT_TYPE::SLASHSCRIPT)
    , m_fAtime(0.f)
    , m_fMaxtime(3.f)
    , m_bFlag(false)
    , m_fRange(100.f)
    , m_AttackNum(0)
{
}

CSlashScript::~CSlashScript()
{
}


void CSlashScript::awake()
{

}

void CSlashScript::update()
{
    if (GetGameObject()->IsDead())
        return;

    if (!m_bFlag)
    {
        Vec3 _SlashPos = Transform()->GetLocalPos();

        m_vDir = Vec2(m_TarPos.x - _SlashPos.x, m_TarPos.z - _SlashPos.z);
        m_vDir.Normalize();

        _SlashPos.x += m_fRange * m_vDir.x;
        _SlashPos.z += m_fRange * m_vDir.y;
        Transform()->SetLocalPos(_SlashPos);

        m_bFlag = true;
    }

    m_fAtime += fDT;
    if (m_fAtime > m_fMaxtime)
    {
        //d = true;
        m_fAtime = 0.f;
        tEvent _temp = {};
        _temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
        _temp.lParam = (DWORD_PTR)GetGameObject();
        CEventMgr::GetInst()->AddEvent(_temp);
    }

}

void CSlashScript::OnCollisionEnter(CGameObject* _pOther) //************************************** ¼öÁ¤
{
    if (m_AttackNum == 0 && _pOther->GetName() == L"Player") // R to L slash
    {

        CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
        bool ret = ColliderCheck(Transform()->GetLocalPos(), Player->Transform()->GetLocalPos(), Vec3(m_vDir.x, 0, m_vDir.y), 3.14f);

        if (ret) Player->knockdown(Transform()->GetLocalPos(), true, HitScale::HitBig, 10);
    }
    if (m_AttackNum == 1 && _pOther->GetName() == L"Player") // R to L slash
    {

        CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
        bool ret = ColliderCheck(Transform()->GetLocalPos(), Player->Transform()->GetLocalPos(), Vec3(m_vDir.x, 0, m_vDir.y), 1.57f/3.f);

        if (ret) Player->knockdown(Transform()->GetLocalPos(), true, HitScale::HitBig, 10);
    }
    if (m_AttackNum == 4 && _pOther->GetName() == L"Player") // R to L slash
    {
        CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
        Player->knockdown(Transform()->GetLocalPos(), true, HitScale::HitBig, 10);
    }
}

void CSlashScript::OnCollisionExit(CGameObject* _pOther)
{
}

void CSlashScript::OnCollision(CGameObject* _pOther)
{
}
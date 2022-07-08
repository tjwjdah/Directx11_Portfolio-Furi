#include "pch.h"
#include "CYoyoScript.h"

#include <Engine\CTimeMgr.h>
#include <Engine\CEventMgr.h>
#include <Engine\CGameObject.h>

#include <Engine/CSceneMgr.h>
#include "CPlayerScript.h"
CYoyoScript::CYoyoScript()
	:CScript((int)SCRIPT_TYPE::YOYOSCRIPT)
	, m_fSpeed(270.f)
	, m_TarPos(Vec3(0.f, 0.f, 0.f))
	, m_bflag(false)
	, m_bBack(false)
{
}

CYoyoScript::~CYoyoScript()
{
}

void CYoyoScript::awake()
{

}

void CYoyoScript::update()
{
	if (GetGameObject()->IsDead())
		return;

	if (!m_bBack)
	{
		// 설정한 타겟 방향으로 날아가게
		if (!m_bflag)
		{
			m_vPos = Transform()->GetLocalPos();
			m_vStartPos = m_vPos;
			m_dir = Vec2(m_TarPos.x - m_vPos.x, m_TarPos.z - m_vPos.z);
			m_dir.Normalize();

			m_bflag = true;
		}

		m_vPos.x += fDT * m_dir.x * m_fSpeed;
		m_vPos.z += fDT * m_dir.y * m_fSpeed;
		Transform()->SetLocalPos(m_vPos);

		float dis = Vec2(m_vPos.x - m_vStartPos.x, m_vPos.z - m_vStartPos.z).Length();

		if (dis >= m_Dis) //설정된 거리 이상 날아가면
			m_bBack = true;

		m_fSpeed -= 0.05f;

	}

	else if (m_bBack)
	{
		m_vPos.x += fDT * -m_dir.x * m_fSpeed;
		m_vPos.z += fDT * -m_dir.y * m_fSpeed;
		Transform()->SetLocalPos(m_vPos);

		float dis = Vec2(m_vPos.x - m_vStartPos.x, m_vPos.z - m_vStartPos.z).Length();

		if (dis <= 500.f)
		{
			//m_fAtime = 0.f;
			tEvent _temp = {};
			_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
			_temp.lParam = (DWORD_PTR)GetGameObject();
			CEventMgr::GetInst()->AddEvent(_temp);

		}

		m_fSpeed += 0.01f;
	}

}

void CYoyoScript::OnCollisionEnter(CGameObject* _pOther)
{
	if (_pOther->GetName() == L"Player")
	{
		CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
		Player->knockdown(Transform()->GetLocalPos(), true, HitScale::HitBig, 10);
	}
}

void CYoyoScript::OnCollisionExit(CGameObject* _pOther)
{
}

void CYoyoScript::OnCollision(CGameObject* _pOther)
{
}
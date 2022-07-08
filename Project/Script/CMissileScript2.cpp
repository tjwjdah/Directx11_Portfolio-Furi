#include "pch.h"
#include "CMissileScript2.h"
#include "CMissileScript.h"

#include <Engine/CEventMgr.h>
#include <Engine/CCollider2D.h>

#include "CPlayerScript.h"
CMissileScript2::CMissileScript2()
	: CScript((int)SCRIPT_TYPE::MISSILESCRIPT2)
	, m_vPos(Vec3(0.f, 0.f, 0.f))
	, m_fDeadTime(0.f)
	, m_fDeadTimeMax(3.f)
	, m_bDead(false)
	, m_bAwake(true)
{

}

CMissileScript2::~CMissileScript2()
{

}

void CMissileScript2::update()
{
	if (m_bAwake)
	{
		m_Yellow = CResMgr::GetInst()->FindRes<CPrefab>(L"YellowBullet");
		m_bAwake = false;
	}

	m_vPos = Transform()->GetLocalPos();

	m_fDeadTime += fDT;
	if (m_fDeadTime >= m_fDeadTimeMax)
	{
		for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 6))
		{
			CGameObject* pBullet = m_Yellow->Instantiate();
			pBullet->SetName(L"Bullet");
			pBullet->Transform()->SetLocalPos(Vec3(m_vPos.x, 0.f, m_vPos.z));

			CMissileScript* BulletS = (CMissileScript*)pBullet->GetScript(L"CMissileScript");

			Vec2 vDir;
			vDir.x = cos(Angle);
			vDir.y = sin(Angle);
			vDir.Normalize();

			BulletS->SetDir(Vec3(vDir.x, 0.f, vDir.y));
			BulletS->SetSpeed(600.f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)4;
			CEventMgr::GetInst()->AddEvent(even);
		}
		if (!m_bDead)
		{
			m_bDead = true;
			DeleteObject(GetObj());
		}
	}

	Transform()->SetLocalPos(m_vPos);
}

void CMissileScript2::OnCollisionEnter(CGameObject* _pOther)
{
	if (!m_bDead)
	{
		if (_pOther->GetName() == L"Player")
		{
			CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
			Player->KnockBack(Transform()->GetLocalPos(), true, HitScale::HitLight, 10);
		}

		m_bDead = true;
		DeleteObject(GetObj());
	}
}

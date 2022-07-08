#include "pch.h"
#include "CWallScript.h"
#include "CMissileScript.h"
#include "CPlayerAttackScript.h"

#include <Engine/CEventMgr.h>

CWallScript::CWallScript()
	: CScript((int)SCRIPT_TYPE::WALLSCRIPT)
	, m_iHP(2) // 원래 6
{

}

CWallScript::~CWallScript()
{
	
}

void CWallScript::awake()
{
	m_Bullet = CResMgr::GetInst()->FindRes<CPrefab>(L"YellowBullet");
}

void CWallScript::update()
{
}

void CWallScript::OnCollisionEnter(CGameObject* _pOther)
{
	wstring wstr = _pOther->GetName();

	if (wstr != L"Player" && wstr != L"Bullet")
	{
		m_iHP--;

		if (m_iHP == 1) // 원래 4
		{
			Vec3 m_vScale = Transform()->GetLocalScale();

			m_vScale.y /= 2.f;
			m_vScale.z /= 2.f;

			Transform()->SetLocalScale(m_vScale);
		}
		else if (m_iHP <= 0)
			DeleteObject(GetObj());


		CPlayerAttackScript* BulletS = (CPlayerAttackScript*)_pOther->GetScript(L"CPlayerAttackScript");

		Vec2 vDir = BulletS->GetDir();

		vDir.x = -vDir.x;
		vDir.y = -vDir.y;

		Vec3 vPos = Transform()->GetWorldPos();

		CGameObject* pBullet = nullptr;
		pBullet = m_Bullet->Instantiate();
		pBullet->SetName(L"Bullet");
		pBullet->Transform()->SetLocalPos(vPos);

		CMissileScript* MissileS = (CMissileScript*)pBullet->GetScript(L"CMissileScript");
		MissileS->SetPos(vPos);
		MissileS->SetDir(Vec3(vDir.x, 0.f, vDir.y));
		MissileS->SetSpeed(600.f);

		tEvent even = {};
		even.eEvent = EVENT_TYPE::CREATE_OBJECT;
		even.lParam = (DWORD_PTR)pBullet;
		even.wParam = (DWORD_PTR)5;
		CEventMgr::GetInst()->AddEvent(even);
	}
}

#include "pch.h"
#include "CMissileScript.h"
#include "CWiseScript.h"
#include "CPlayerScript.h"
#include "CBulletScript.h"
#include <Engine\CEventMgr.h>
CMissileScript::CMissileScript()
	: CScript((int)SCRIPT_TYPE::MISSILESCRIPT)
	, m_vPos(Vec3(0.f, 0.f, 0.f))
	, m_vDir(Vec3(0.f, 0.f, 0.f))
	, m_fSpeed(270.f)
	, m_fDeadTime(0.f)
	, m_fDeadTimeMax(5.f)
	, m_bDead(false)
	, m_bBullet(true)
{

}

CMissileScript::~CMissileScript()
{

}

void CMissileScript::update()
{
	m_vPos = Transform()->GetLocalPos();

	m_vPos += m_vDir * fDT * m_fSpeed;

	m_fDeadTime += fDT;
	if (m_fDeadTime >= m_fDeadTimeMax)
	{
		if (!m_bDead)
		{
			m_bDead = true;
			DeleteObject(GetObj());
		}
	}

	Transform()->SetLocalPos(m_vPos);
}

void CMissileScript::OnCollisionEnter(CGameObject* _pOther)
{
	if (!m_bDead)
	{
		if (_pOther->GetName() == L"Player")
		{
			CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
			if (m_bBullet)
			{
				if (Player->KnockBack(Transform()->GetLocalPos(), true, HitScale::HitLight, 10))
				{
					CGameObject* pBullet = CResMgr::GetInst()->FindRes<CPrefab>(L"spreadbullet2")->Instantiate();
					pBullet->Transform()->SetLocalPos(Vec3(Transform()->GetLocalPos()));

					CBulletScript* Bull = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
					Bull->SetChangeDir(false);

					Vec3 _Pos = _pOther->Transform()->GetLocalPos();
					Bull->SetTarPos(_Pos);
					Bull->SetSpeed(-m_fSpeed * 2);
					Bull->SetMaxTime(7.f);

					tEvent even = {};
					even.eEvent = EVENT_TYPE::CREATE_OBJECT;
					even.lParam = (DWORD_PTR)pBullet;
					even.wParam = (DWORD_PTR)3;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();
					CEventMgr::GetInst()->AddEvent(even);
				}
			}
			else
				Player->KnockBack(Transform()->GetLocalPos(), true, HitScale::HitBig, 10);
				
		}
		m_bDead = true;
		DeleteObject(GetObj());
	}
}

void CMissileScript::OnCollision(CGameObject* _pOther)
{
}

#include "pch.h"
#include "CRadianDirBulletScript.h"

#include <Engine\CTimeMgr.h>
#include <Engine\CEventMgr.h>
#include <Engine\CGameObject.h>

#include <Engine/CSceneMgr.h>
#include "CPlayerScript.h"
#include "CBulletScript.h"

CRadianDirBulletScript::CRadianDirBulletScript()
	:CScript((int)SCRIPT_TYPE::RADIANDIRBULLETSCRIPT)
	, m_fAtime(0.f)
	, m_fMaxtime(5.f)
	, m_fSpeed(270.f)
	, m_fRadian(0.f)
{
}

CRadianDirBulletScript::~CRadianDirBulletScript()
{
}

void CRadianDirBulletScript::awake()
{

}

void CRadianDirBulletScript::update()
{
	if (GetGameObject()->IsDead())
		return;

	Vec3 vPos = Transform()->GetLocalPos();
	Vec2 dir;
	dir.x = cosf(m_fRadian);
	dir.y = sinf(m_fRadian);
	dir.Normalize();

	vPos.x += fDT * dir.x * m_fSpeed;
	vPos.z += fDT * dir.y * m_fSpeed;


	m_fAngle = -atan2f(dir.y, dir.x);	//접선의 기울기

	Transform()->SetLocalRot(Vec3(0, m_fAngle, 0));
	Transform()->SetLocalPos(vPos);

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
void CRadianDirBulletScript::OnCollisionEnter(CGameObject* _pOther)
{
	if (_pOther->GetName() == L"Player")
	{
		CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
		if (Player->KnockBack(Transform()->GetLocalPos(), true, HitScale::HitLight, 10))
		{
			CGameObject* pBullet = CResMgr::GetInst()->FindRes<CPrefab>(L"spreadbullet1")->Instantiate();
			CRadianDirBulletScript* tar = (CRadianDirBulletScript*)pBullet->GetScript(L"CRadianDirBulletScript");
			tar->SetSpeed(-m_fSpeed * 2);
			tar->SetMaxTime(10.f);
			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)4;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();
			CEventMgr::GetInst()->AddEvent(even);

		}

		tEvent _temp = {};
		_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
		_temp.lParam = (DWORD_PTR)GetGameObject();
		CEventMgr::GetInst()->AddEvent(_temp);
	}
}

void CRadianDirBulletScript::OnCollisionExit(CGameObject* _pOther)
{
}

void CRadianDirBulletScript::OnCollision(CGameObject* _pOther)
{
}
#include "pch.h"
#include "CPillarScript.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CEventMgr.h>

#include "CWiseScript.h"
#include "CMissileScript.h"

#include <random>

CPillarScript::CPillarScript()
	: CScript((int)SCRIPT_TYPE::PILLARSCRIPT)
	, m_iHP(5)
	, m_fShootTimeMax(3.f)
	, m_fShootTime(0.f)
	, m_bAwake(true)
	, m_bType(true)
{

}

CPillarScript::~CPillarScript()
{

}

void CPillarScript::update()
{
	if (m_bAwake)
	{
		m_Yellow = CResMgr::GetInst()->FindRes<CPrefab>(L"YellowBullet");
		m_bAwake = false;
	}

	if (m_bType)
	{
		m_fShootTime += fDT;

		if (m_fShootTime >= m_fShootTimeMax)
		{
			m_fShootTime = 0.f;

			for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 30))
			{
				CGameObject* pBullet = m_Yellow->Instantiate();
				pBullet->SetName(L"Bullet");
				pBullet->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));

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
		}
	}
}

void CPillarScript::OnCollisionEnter(CGameObject* _pOther)
{
	if (_pOther->GetName() == L"Player")
		return;
	m_iHP--;
	if (m_iHP <= 0)
	{
		CWiseScript* WiseScript = (CWiseScript*)CSceneMgr::GetInst()->FindObjectByName(L"Wise")->GetScript(L"CWiseScript");
		WiseScript->SetPatternRepeat(1);

		DeleteObject(GetObj());
	}

	if (!m_bType)
	{
		int Count = 0;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> MoveX(1, 5);
		int Value = MoveX(gen);

		for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 30))
		{
			Count++;
			if (Count <= Value * 6 && Count >= (Value - 1) * 6)
			{
				CGameObject* pBullet = m_Yellow->Instantiate();
				pBullet->SetName(L"Bullet");

				Vec3 vPos = Transform()->GetLocalPos();
				pBullet->Transform()->SetLocalPos(Vec3(vPos.x, 0.f, vPos.z));

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
		}
	}
}
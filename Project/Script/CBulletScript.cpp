#include "pch.h"
#include "CBulletScript.h"

#include <Engine\CTimeMgr.h>
#include <Engine\CEventMgr.h>
#include <Engine\CGameObject.h>
#include <Engine/CResMgr.h>
#include <Engine/CPrefab.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CStructuredBuffer.h>

#include "CPlayerScript.h"
CBulletScript::CBulletScript()
	:CScript((int)SCRIPT_TYPE::BULLETSCRIPT)
	, m_fAtime(0.f)
	, m_fMaxtime(5.5f)  //***********************************수정
	, m_fSpeed(270.f)
	, m_fRadian(0.f)
	, m_fAngle(0.f)
	, d(false)  //test
	, m_TarPos(Vec3(0.f, 0.f, 0.f))
	, m_bChangeDir(false)
	, m_flag(false)
	, m_iAttackNum(0)  //***********************************수정
	, b_bAwake(true)
	, m_pSectorBuffer(nullptr)
{

}

CBulletScript::~CBulletScript()
{
	SAFE_DELETE(m_pSectorBuffer);
}

void CBulletScript::awake()
{

}

void CBulletScript::update()
{
	if (b_bAwake && m_iAttackNum == 3)
	{
		b_bAwake = false;
		if (m_pSectorBuffer == nullptr) {
			m_pSectorBuffer = new CStructuredBuffer;
			m_pSectorBuffer->Create(sizeof(tSectorInfo), 1, nullptr);
		}
	}


	if (GetGameObject()->IsDead())
		return;

	// 설정한 타겟 방향으로 날아가게
	if (m_bChangeDir == true)
	{
		Vec3 vPos = Transform()->GetLocalPos();
		Vec2 dir = Vec2(vPos.x - m_TarPos.x, vPos.z - m_TarPos.z);
		dir.Normalize();

		vPos.x += fDT * dir.x * m_fSpeed;
		vPos.z += fDT * dir.y * m_fSpeed;

		Transform()->SetLocalPos(vPos);

		m_fAtime += fDT;
		if (m_fAtime > m_fMaxtime && !d)
		{
			//d = true;
			m_fAtime = 0.f;
			tEvent _temp = {};
			_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
			_temp.lParam = (DWORD_PTR)GetGameObject();
			CEventMgr::GetInst()->AddEvent(_temp);
		}
	}
	else
	{
		if (!m_flag)
		{
			m_vPos = Transform()->GetLocalPos();
			m_dir = Vec2(m_TarPos.x - m_vPos.x, m_TarPos.z - m_vPos.z);
			m_dir.Normalize();

			m_flag = true;
		}

		m_vPos.x += fDT * m_dir.x * m_fSpeed;
		m_vPos.z += fDT * m_dir.y * m_fSpeed;
		Transform()->SetLocalPos(m_vPos);
		m_fAtime += fDT;

		if (m_fAtime > m_fMaxtime && !d)
		{
			//d = true;
			m_fAtime = 0.f;
			tEvent _temp = {};
			_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
			_temp.lParam = (DWORD_PTR)GetGameObject();
			CEventMgr::GetInst()->AddEvent(_temp);
		}
	}
}

void CBulletScript::OnCollisionEnter(CGameObject* _pOther)  //***********************************수정
{
	if (m_iAttackNum == 1 && _pOther->GetName() == L"Player") //총알
	{
		CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
		if (Player->KnockBack(Transform()->GetLocalPos(), true, HitScale::HitLight, 10))
		{
			CGameObject* pBullet = CResMgr::GetInst()->FindRes<CPrefab>(L"spreadbullet2")->Instantiate();
			pBullet->Transform()->SetLocalPos(Vec3(Transform()->GetLocalPos()));

			CBulletScript* Bull = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
			Bull->SetChangeDir(false);

			Vec3 _Pos = m_TarPos;
			Bull->SetTarPos(_Pos);
			Bull->SetSpeed(-m_fSpeed * 2);
			Bull->SetMaxTime(7.f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)3;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();
			CEventMgr::GetInst()->AddEvent(even);

		}

		tEvent _temp = {};
		_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
		_temp.lParam = (DWORD_PTR)GetGameObject();
		CEventMgr::GetInst()->AddEvent(_temp);
	}
	if (m_iAttackNum == 2 && _pOther->GetName() == L"Player") //총알
	{
		CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
		if (Player->KnockBack(Transform()->GetLocalPos(), true, HitScale::HitLight, 10))
		{
			CGameObject* pBullet = CResMgr::GetInst()->FindRes<CPrefab>(L"spreadbullet2")->Instantiate();
			pBullet->Transform()->SetLocalPos(Vec3(Transform()->GetLocalPos()));

			CBulletScript* Bull = (CBulletScript*)pBullet->GetScript(L"CBulletScript");
			Bull->SetChangeDir(true);

			Vec3 _Pos = m_TarPos;
			Bull->SetTarPos(_Pos);
			Bull->SetSpeed(-m_fSpeed * 2);
			Bull->SetMaxTime(7.f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)3;//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();
			CEventMgr::GetInst()->AddEvent(even);
		}

		tEvent _temp = {};
		_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
		_temp.lParam = (DWORD_PTR)GetGameObject();
		CEventMgr::GetInst()->AddEvent(_temp);
	}
	else if (m_iAttackNum == 3 && _pOther->GetName() == L"Player") //부채꼴 공격
	{
		CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");

		bool ret = ColliderCheck(Transform()->GetLocalPos(), Player->Transform()->GetLocalPos(), Vec3(m_dir.x, 0, m_dir.y), 0.1);

		if (ret) Player->knockdown(Transform()->GetLocalPos(), true, HitScale::HitBig, 30);
	}
}

void CBulletScript::OnCollisionExit(CGameObject* _pOther)
{
}

void CBulletScript::OnCollision(CGameObject* _pOther)
{
}


void CBulletScript::UpdateData()
{
	vector<tSectorInfo> vecSector;
	vecSector.push_back(m_tSectorInfo);

	m_pSectorBuffer->SetData(vecSector.data(), sizeof(tSectorInfo), (UINT)1);
	m_pSectorBuffer->UpdateData(75);
}

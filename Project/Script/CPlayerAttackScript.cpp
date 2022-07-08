#include "pch.h"
#include "CPlayerAttackScript.h"

#include <Engine\CTransform.h>
#include <Engine\CGameObject.h>
#include <Engine\CEventMgr.h>
#include <Engine\CTimeMgr.h>

#include "CCameraAnimaitorScript.h"
#include "CPlayerScript.h"
#include "CLawScript.h"
#include "CWiseScript.h"
CPlayerAttackScript::CPlayerAttackScript() :CScript((int)SCRIPT_TYPE::PLAYERATTACKSCRIPT)
, m_Player(nullptr)
, m_fAngleStart(0.f)
, m_fAngleEnd(0.f)
, m_fTime(0.f)
, m_fdeleteTime(0.f)
, m_AttackType(Attack_Type::SowrdAttack)
, m_bDead(false)
, m_Attack4(false)
{
}


CPlayerAttackScript::~CPlayerAttackScript()
{
}


void CPlayerAttackScript::awake()
{
	
}

void CPlayerAttackScript::SetPlayer(CGameObject* _Player, Vec2 _AttackDir)
{
	m_Player = _Player; m_vDir = _AttackDir;
	Vec3 mPos;
	mPos.x = m_Player->Transform()->GetWorldPos().x + m_vDir.x * 100.f;
	mPos.z = m_Player->Transform()->GetWorldPos().z + m_vDir.y * 100.f;


	Transform()->SetLocalPos(mPos);
	
}

void CPlayerAttackScript::update()
{
	if (m_Player == nullptr || GetGameObject()->IsDead() || m_bDead==true)
		return;

	
	Vec3 mPos;
	switch (m_AttackType)
	{
	case Attack_Type::SowrdAttack:
	{
		m_fTime += fDT;
		mPos.x = m_Player->Transform()->GetWorldPos().x + m_vDir.x * 100;
		mPos.z = m_Player->Transform()->GetWorldPos().z + m_vDir.y * 100;
	}
		break;
	case Attack_Type::ChargeSowrdAttack:
	{
	
		m_fTime += fDT;
		mPos.x = m_Player->Transform()->GetWorldPos().x + m_vDir.x * 100;
		mPos.z = m_Player->Transform()->GetWorldPos().z + m_vDir.y * 100;
	}
		break;
	case Attack_Type::FullChargeSowrdAttack:
	{
		m_fTime += fDT;
		mPos.x = m_Player->Transform()->GetWorldPos().x + m_vDir.x * 100;
		mPos.z = m_Player->Transform()->GetWorldPos().z + m_vDir.y * 100;
	}
		break;
	case Attack_Type::GunAttack:
	{
		m_fTime += fDT;
		
		mPos.x = Transform()->GetLocalPos().x + m_vDir.x * 1200.f*fDT;
		mPos.z = Transform()->GetLocalPos().z + m_vDir.y * 1200.f*fDT;
	}
		break;
	case Attack_Type::ChargeGunAttack:
	{
		m_fTime += fDT;
		mPos.x = Transform()->GetLocalPos().x + m_vDir.x * 2500.f * fDT;
		mPos.z = Transform()->GetLocalPos().z + m_vDir.y * 2500.f * fDT;
	}
		break;
	case Attack_Type::FullChargeGunAttack:
	{
		m_fTime += fDT;
		mPos.x = Transform()->GetLocalPos().x + m_vDir.x * 3000.f * fDT;
		mPos.z = Transform()->GetLocalPos().z + m_vDir.y * 3000.f * fDT;
	}
		break;
	}
	
	Transform()->SetLocalPos(mPos);

	if (m_fTime > m_fdeleteTime)
	{
		m_fTime = 0.f;
		tEvent _temp = {};
		_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
		_temp.lParam = (DWORD_PTR)GetGameObject();
		CEventMgr::GetInst()->AddEvent(_temp);
		m_bDead = true;
	}

}



void CPlayerAttackScript::OnCollisionEnter(CGameObject* _pOther)
{
	if (m_bDead == true)
		return;
	if (_pOther->GetName() == L"Law")
	{
		CLawScript* Law = (CLawScript*)_pOther->GetScript(L"CLawScript");
		switch (m_AttackType)
		{
		case Attack_Type::SowrdAttack:
			Law->HitReceived();
			if (Law->IsGuard())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->Monsterguard();
			}

			if (Law->IsStun() )
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->MeleeAttack();
				Law->SetMELEEATK();
			}
			else {
				Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SlashElectric744");
				pSound->Play(1, 0.3f);
			}
			break;
		case Attack_Type::ChargeSowrdAttack:
			Law->HitReceived();
			if (Law->IsGuard())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->Monsterguard();
			}
			if (Law->IsStun())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->MeleeAttack();
				Law->SetMELEEATK();
			}
			else {
				Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SlashElectric744");
				pSound->Play(1, 0.3f);
			}
			break;
		case Attack_Type::FullChargeSowrdAttack:
			Law->FullChargedHitReceived();
			if (Law->IsStun())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->GetCameraAnimitor()->SetCameraAnim(34);
			}
			if (Law->IsGuard())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->Monsterguard();
			}
			else {
				Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SlashElectric744");
				pSound->Play(1, 0.3f);
			}
			
			break;
		case Attack_Type::GunAttack:
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"ProjectileAimingImpact207");
			pSound->Play(1, 0.3f);
			Law->GunHitReceived();
		}
			break;
		case Attack_Type::ChargeGunAttack:
		{	Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"ProjectileAimingImpact207");
		pSound->Play(1, 0.3f);
			Law->GunHitReceived();

		}
			break;
		case Attack_Type::FullChargeGunAttack:
		{	Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"ProjectileAimingImpact207");
			pSound->Play(1, 0.6f);
			Law->FullChargedGunHitReceived();
		}
			break;
		}
		m_fTime = 0.f;
		m_bDead = true;
		tEvent _temp = {};
		_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
		_temp.lParam = (DWORD_PTR)GetGameObject();
		CEventMgr::GetInst()->AddEvent(_temp);
	}

	if (_pOther->GetName() == L"Wise")
	{
		CWiseScript* Wise = (CWiseScript*)_pOther->GetScript(L"CWiseScript");
		switch (m_AttackType)
		{
		case Attack_Type::SowrdAttack:
			if(!m_Attack4)
			Wise->HitMoment();
			else
				Wise->KnockdownMoment();
			/*
			if (Law->IsGuard())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->Monsterguard();
			}
			if (Law->IsStun())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->MeleeAttack();
				Law->SetMELEEATK();
			}
			*/
			break;
		case Attack_Type::ChargeSowrdAttack:
			Wise->HitMoment();
			/*
			if (Law->IsGuard())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->Monsterguard();
			}
			if (Law->IsStun())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->MeleeAttack();
				Law->SetMELEEATK();
			}
			*/
			break;
		case Attack_Type::FullChargeSowrdAttack:
			
			//Wise->KnockdownMoment();
			/*
			if (Law->IsStun())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->GetCameraAnimitor()->SetCameraAnim(34);
			}
			if (Law->IsGuard())
			{
				CPlayerScript* Player = (CPlayerScript*)m_Player->GetScript(L"CPlayerScript");
				Player->Monsterguard();
			}
			*/
			break;
		case Attack_Type::GunAttack:
			Wise->HitMoment();
			break;
		case Attack_Type::ChargeGunAttack:
			Wise->HitMoment();
			break;
		case Attack_Type::FullChargeGunAttack:
			Wise->KnockdownMoment();
			break;
		}
		m_fTime = 0.f;
		m_bDead = true;
		tEvent _temp = {};
		_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
		_temp.lParam = (DWORD_PTR)GetGameObject();
		CEventMgr::GetInst()->AddEvent(_temp);
	}
	//if (ColliderCheck(Transform()->GetWorldPos(), _pOther->Transform()->GetWorldPos(), m_fAngleStart, m_fAngleEnd))
	//{
	if (_pOther->GetName() == L"spreadbullet1" || _pOther->GetName() == L"spreadbullet2" )
	{
		switch (m_AttackType)
		{
		case Attack_Type::GunAttack:
		{	
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"ProjectileAimingImpact207");
			pSound->Play(1, 0.3f);
			m_fTime = 0.f;
			tEvent _temp = {};
			_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
			_temp.lParam = (DWORD_PTR)_pOther;
			CEventMgr::GetInst()->AddEvent(_temp);

		}
			break;
		case Attack_Type::ChargeGunAttack:
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"ProjectileAimingImpact207");
			pSound->Play(1, 0.3f);
			m_fTime = 0.f;
			tEvent _temp = {};
			
			_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
			_temp.lParam = (DWORD_PTR)_pOther;
			CEventMgr::GetInst()->AddEvent(_temp);

		}
			break;
		case Attack_Type::FullChargeGunAttack:
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"ProjectileAimingImpact207");
			pSound->Play(1, 0.6f);
			m_fTime = 0.f;
			tEvent _temp = {};
			_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
			_temp.lParam = (DWORD_PTR)_pOther;
			CEventMgr::GetInst()->AddEvent(_temp);

		}
		break;
		}
		m_bDead = true;
		m_fTime = 0.f;
		tEvent _temp = {};
		_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
		_temp.lParam = (DWORD_PTR)GetGameObject();
		CEventMgr::GetInst()->AddEvent(_temp);

	}
	if (_pOther->GetName() == L"Bullet" || _pOther->GetName() == L"Wall")
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"ProjectileAimingImpact207");
		pSound->Play(1, 0.6f);
		m_bDead = true;
		m_fTime = 0.f;
		tEvent _temp = {};
		_temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
		_temp.lParam = (DWORD_PTR)GetGameObject();
		CEventMgr::GetInst()->AddEvent(_temp);

	}
	//}
}


void CPlayerAttackScript::SetfAngleStart(float _fAngleStart)
{
	if (_fAngleStart < 0)
		m_fAngleStart = 6.28 + _fAngleStart;
	else
		m_fAngleStart = _fAngleStart;
	
}

void CPlayerAttackScript::SetfAngleEnd(float _fAngleEnd)
{
	if (_fAngleEnd < 0)
		m_fAngleEnd = 6.28 + _fAngleEnd;
	else
		m_fAngleEnd = _fAngleEnd;
	
}

void CPlayerAttackScript::SetAttackType(Attack_Type _AttackType)
{
	m_AttackType = _AttackType;
	switch (m_AttackType)
	{
	case Attack_Type::SowrdAttack:
	{
		Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		m_fdeleteTime = 0.5f;
		//m_AngleOn = true;
	}
	break;
	case Attack_Type::ChargeSowrdAttack:
	{
		Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		m_fdeleteTime = 0.5f;
	}
	break;
	case Attack_Type::FullChargeSowrdAttack:
	{
		Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		m_fdeleteTime = 0.5f;
	}
	break;
	case Attack_Type::GunAttack:
	{
		Transform()->SetLocalScale(Vec3(80.f, 80.f, 80.f));
		m_fdeleteTime =5.f;
		
	}
	break;
	case Attack_Type::ChargeGunAttack:
	{
		Transform()->SetLocalScale(Vec3(110.f, 110.f, 110.f));
		m_fdeleteTime = 5.f;
		
	}
	break;
	case Attack_Type::FullChargeGunAttack:
	{
		Transform()->SetLocalScale(Vec3(150.f, 150.f, 150.f));
		m_fdeleteTime = 5.f;
	
	}
	break;
	}
}

void CPlayerAttackScript::OnCollisionExit(CGameObject* _pOther)
{
}

void CPlayerAttackScript::OnCollision(CGameObject* _pOther)
{
}

#include "pch.h"
#include "CPlayerScript.h"
#include <Engine\CEventMgr.h>
#include "CPlayerAttackScript.h"
#include <Engine\CParticleSystem.h>
void CPlayerScript::Attack() {

	if (m_CurrentCombo != m_PreCombo && m_CurrentCombo != 0 && PLAYER_STATE::SWORDATTACK == m_eCurState)
	{
		CGameObject* Attack = m_Attack->Instantiate();
		CPlayerAttackScript* AttackScript = (CPlayerAttackScript*)Attack->GetScript(L"CPlayerAttackScript");
		if (m_CloseBattle)
		{
			Vec3 TargetDir = m_Target->Transform()->GetLocalPos() - Transform()->GetLocalPos();
			TargetDir.Normalize();
			Vec2 TargetDir2 = Vec2(TargetDir.x, TargetDir.z);
			AttackScript->SetPlayer(GetGameObject(), TargetDir2);
		}
		else
		AttackScript->SetPlayer(GetGameObject(), m_AttackDir);

		AttackScript->SetAttackType(Attack_Type::SowrdAttack);

		 tEvent even = {};
		 even.eEvent = EVENT_TYPE::CREATE_OBJECT;
		 even.lParam = (DWORD_PTR)Attack;
		 //even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();
		 even.wParam = (DWORD_PTR)1;
		 CEventMgr::GetInst()->AddEvent(even);
	}
	else if(PLAYER_STATE::SWORDCHAGERATTACK == m_eCurState && m_eCurState != m_ePrevState)
	{
		CGameObject* Attack = m_Attack->Instantiate();
		CPlayerAttackScript* AttackScript = (CPlayerAttackScript*)Attack->GetScript(L"CPlayerAttackScript");
		if (m_CloseBattle)
		{
			Vec3 TargetDir = m_Target->Transform()->GetLocalPos() - Transform()->GetLocalPos();
			TargetDir.Normalize();
			Vec2 TargetDir2 = Vec2(TargetDir.x, TargetDir.z);
			AttackScript->SetPlayer(GetGameObject(), TargetDir2);
		}
		else
		AttackScript->SetPlayer(GetGameObject(), m_AttackDir);
		AttackScript->SetAttackType(Attack_Type::GunAttack);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sword");
		pSound->Play(1, 0.3f);
			if(m_AttackChargefull)
		AttackScript->SetAttackType(Attack_Type::FullChargeSowrdAttack);
		else
			AttackScript->SetAttackType(Attack_Type::ChargeSowrdAttack);

		tEvent even = {};
		even.eEvent = EVENT_TYPE::CREATE_OBJECT;
		even.lParam = (DWORD_PTR)Attack;
		//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();
		even.wParam = (DWORD_PTR)1;
		CEventMgr::GetInst()->AddEvent(even);
	}
	else if (PLAYER_STATE::GUNATTACK == m_eCurState )
	{
		
		if (m_GunAttackTime > 0.3f)
		{
			m_GunAttackTime = 0.f;
			CGameObject * Attack = m_Attack->Instantiate();
			Attack->AddComponent(new CParticleSystem);
			CParticleSystem* Par = Attack->ParticleSystem();
			Par->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
			Par->SetvEndColor(Vec4(0.4f, 0.4f, 0.8f, 0.3f));
			Par->SetvStartColor(Vec4(0.0f, 0.224f, 0.87f, 0.8f));
			Par->SetvStartScale(Vec3(80.f, 80.f, 80.f));
			CPlayerAttackScript* AttackScript = (CPlayerAttackScript*)Attack->GetScript(L"CPlayerAttackScript");
			AttackScript->SetPlayer(GetGameObject(), m_AttackDir);
		
			AttackScript->SetAttackType(Attack_Type::GunAttack);
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"gunshot2"); 
			pSound->Play(1, 0.3f,true);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)Attack;
			//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();
			even.wParam = (DWORD_PTR)3;
			CEventMgr::GetInst()->AddEvent(even);
		}
			m_GunAttackTime += fDT;
		
	}
	else if (PLAYER_STATE::GUNCHAGERATTACK == m_eCurState && m_eCurState != m_ePrevState)
	{
		CGameObject* Attack = m_Attack->Instantiate();
		Attack->AddComponent(new CParticleSystem);

		CPlayerAttackScript* AttackScript = (CPlayerAttackScript*)Attack->GetScript(L"CPlayerAttackScript");
		AttackScript->SetPlayer(GetGameObject(), m_AttackDir);
		
		if (m_AttackChargefull)
		{
			m_AttackChargefull = false;
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"Gun_Shot_V1_05");
			pSound->Play(1, 0.3f);
			AttackScript->SetAttackType(Attack_Type::FullChargeGunAttack);
			CParticleSystem* Par = Attack->ParticleSystem();
			Par->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
			Par->SetvEndColor(Vec4(0.1f, 0.1f, 1.0f, 0.5f));
			Par->SetvStartColor(Vec4(0.0f, 0.224f, 0.87f, 0.8f));
			Par->SetvStartScale(Vec3(150.f,150.f, 150.f));
		}
		else
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"gunshot1");
			pSound->Play(1, 0.3f);
			AttackScript->SetAttackType(Attack_Type::ChargeGunAttack);
			CParticleSystem* Par = Attack->ParticleSystem();
			Par->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
			Par->SetvEndColor(Vec4(0.3f, 0.3f, 0.9f, 0.4f));
			Par->SetvStartColor(Vec4(0.0f, 0.224f, 0.87f, 0.8f));
			Par->SetvStartScale(Vec3(110.f, 110.f, 110.f));
		}
		tEvent even = {};
		even.eEvent = EVENT_TYPE::CREATE_OBJECT;
		even.lParam = (DWORD_PTR)Attack;
		//even.wParam = (DWORD_PTR)GetGameObject()->GetLayerIndex();
		even.wParam = (DWORD_PTR)3;
		CEventMgr::GetInst()->AddEvent(even);
	}
}
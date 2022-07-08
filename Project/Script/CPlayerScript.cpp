#include "pch.h"
#include "CPlayerScript.h"

#include <Engine\CSceneMgr.h>
#include <Engine\CScene.h>
#include <Engine/CCore.h>
#include <Engine\CRenderMgr.h>
#include <Engine\CCamera.h>
#include <Engine\CAnimator3D.h>
#include <Engine\CResMgr.h>
#include <Engine\CPrefab.h>
#include <Engine\CCollider2D.h>
#include <Engine\CGameObject.h>
#include <Engine\CParticleSystem.h>
#include <Engine\CEventMgr.h>
#include "CEffectScript.h"
#include "CMissileScript.h"
#include "CAnimatorScript.h"
#include "AnimNameList.h"
#include "CNavMeshManager.h"
#include "CCameraAnimaitorScript.h"
#include "CLawScript.h"
const float ChargefullTime = 1.f;

CPlayerScript::CPlayerScript()
	: CScript((int)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_CurrentCombo(0)
	, m_PreCombo(0)
	, m_MaxCombo(4)
	, m_IsAttacking(false)
	, m_CanNextCombo(false)
	, m_IsComboInputOn(false)
	, m_fPlayerSpeed(400.f)
	, m_fAtime(0.f)
	, m_fMaxtime(0.5f)
	, m_Animation(nullptr)
	, m_Combo4AttackCase(0)
	, m_AnimDir(DIR::NONE)
	, m_PreAinmDir(DIR::NONE)
	, m_eCurDir(DIR::UP)
	, m_ePreDir(DIR::UP)
	, m_eCurState(PLAYER_STATE::IDLE)
	, m_ePrevState(PLAYER_STATE::IDLE)
	, m_Combo4AttackOn(false)
	, m_AttackDir(0.f, 0.f)
	, m_AttackSpeed(0.f)
	, m_AttackCharge(false)
	, m_AttackChargeTime(0.f)
	, m_AttackChargefull(false)
	, m_DashChargeTime(0.3f)
	, m_ParryTime(0.f)
	, m_GunAttackTime(0.f)
	, m_AttackMoveStop(false)
	, m_Target(nullptr)
	, m_hit(false)
	, m_vhitDir(Vec2(0.f, 0.f))
	, m_NavMeshManager(nullptr)
	, m_NavNum(0)
	, m_MeleeAtkOn(false)
	, m_CameraAnim(nullptr)
	, m_fHP(100.f)
	, m_iLife(5)
	, m_AttackMiss(false)
	, m_Grap(false)
	, m_GrapSolve(0)
	, m_HitSpeed(0.f)
	, m_Collignore(false)
	, m_fKnockDownTime(0.f)
	, m_bKnockdown(false)
	, m_TargetAngle(0.f)
	, m_GrapTime(0.f)
	, m_CPrePos(Vec3(0.f, 0.f, 0.f))
	, m_CloseBattle(false)
	, m_bKnockdownstandup(false)
	, m_GunChargeSound(true)
	, m_MeleeAtKSound1(true)
	, m_MeleeAtKSound2(true)
	, m_MeleeAtKSound3(true)
	, m_MeleeAtKSoundKick(true)
	, m_SwordCharge(nullptr)
	, m_SwordCharge2(nullptr)
	, m_GunCharge(nullptr)
	, m_SwordChargebool(true)
	, m_GunChargebool(true)
	, m_Grapdelay(0.f)
	, PostEffect_Vignette(nullptr)
	, PostEffect_GlichLine(nullptr)
{
	m_tRay.vDir = Vec3(0.f, -1.f, 0.f);
	m_tRay.vDir.Normalize();
}

CPlayerScript::~CPlayerScript()
{

}

void CPlayerScript::awake()
{
	for (UINT i = 0; i < GetGameObject()->GetChild().size(); i++)
	{
		if (GetGameObject()->GetChild()[i]->Animator3D() != nullptr)
		{
			m_Animation = GetGameObject()->GetChild()[i]->Animator3D();
			break;
		}
	}
	m_Attack = CResMgr::GetInst()->FindRes<CPrefab>(L"PlayerAttack");

	assert(m_Animation);
	m_CameraAnim = (CCameraAnimaitorScript*)CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Camera_bn")->GetScript(L"CCameraAnimaitorScript");
	m_CameraAnim->SetTargerPlayer(GetGameObject());
	PostEffect_GlichLine = CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"PostEffect_GlichLine");
	PostEffect_Vignette = CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"PostEffect_Vignette");
	m_Target = CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Law");
	if (m_Target == nullptr)
		m_Target = CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Wise");

	AllNavVectorCheck();
}

void CPlayerScript::update()
{
	//키 입력 받음

	CalRay();

	Vec3 _MonPos = Transform()->GetLocalPos();
	_MonPos.x = m_Target->Transform()->GetLocalPos().x - _MonPos.x;
	_MonPos.z = m_Target->Transform()->GetLocalPos().z - _MonPos.z;
	_MonPos.Normalize();
	m_TargetAngle = atan2f(_MonPos.x, _MonPos.z);

	CheckState();


	//현재 상태에 맞는 애니메이션 재생
	PlayAnimation();


	//바뀐 상태에서 움직임
	PlayerAction();


	Attack();


	m_PreCombo = m_CurrentCombo;

	/*
	if (KEY_TAP((KEY_TYPE::KEY_Z)))
		Grap();
	//if (KEY_TAP((KEY_TYPE::NUM_1)))
	//	KnockBack(Vec3(300.f, 0.f, 0.f), false, HitScale::HitLight, 0);
	if (KEY_TAP((KEY_TYPE::NUM_2)))
		KnockBack(Vec3(0.f, 0.f, -300.f), false, HitScale::HitLight, 0);
	if (KEY_TAP((KEY_TYPE::NUM_3)))
		KnockBack(Vec3(-300.f, 0.f, 0.f), false, HitScale::HitLight, 0);
		*/
}

void CPlayerScript::CheckState()
{

	m_ePrevState = m_eCurState;
	m_ePreDir = m_eCurDir;
	m_PreAinmDir = m_AnimDir;

	bool finish = m_Animation->GetFinish();
	float Ratio = m_Animation->GetAnimFrmRatio();
	if (Ratio > 0.6f && m_eCurState == PLAYER_STATE::SWORDATTACK && m_Combo4AttackOn == false)
	{
		if (m_CurrentCombo == m_MaxCombo)
		{
			AttackEndComboState();
		}
		else if (m_IsComboInputOn)
		{
			AttackStartComboState();
		}
		else if (!m_IsComboInputOn)
		{
			m_fAtime += fDT;
			if (m_fMaxtime < m_fAtime)
			{
				AttackEndComboState();
				m_fAtime = 0.f;
			}
		}

	}
	if (m_MeleeAtkOn)
	{
		m_CameraAnim->SetCameraAnim(37);
		m_eCurState = PLAYER_STATE::MELEEATTACK;
		m_MeleeAtkOn = false;
	}
	if (m_AttackMiss)
	{
		//m_CameraAnim->SetCameraAnim(37);
		m_eCurState = PLAYER_STATE::SWORDATTACKMISS;
		m_AttackMiss = false;
	}
	if (m_Grap) {
		m_Grapdelay += fDT;
		if (m_Grapdelay > 1.1f)
		{
			PostEffect_GlichLine->Transform()->SetLocalScale(Vec3(20000.f, 20000.f, 1.f));
			m_eCurState = PLAYER_STATE::GRAP;
			m_CameraAnim->SetCameraAnim(28);
			m_Grap = false;
			m_Grapdelay = 0.f;
			
		}
	}
	if (m_eCurState == PLAYER_STATE::GRAP)
	{

		if (KEY_TAP(KEY_TYPE::KEY_A) || (KEY_TAP(KEY_TYPE::KEY_D)))
			m_GrapSolve++;

		if (m_GrapSolve > 5)
		{
			m_eCurState = PLAYER_STATE::GRAPSOLVESUCCESS;
			m_CameraAnim->SetCameraAnim(29);
			m_GrapSolve = 0;
			CLawScript* Law = (CLawScript*)m_Target->GetScript(L"CLawScript");
			Law->SetPlayerGuardSucc();

		}
		if (finish)
		{
			m_eCurState = PLAYER_STATE::GRAPSOLVEFAILURE;
			m_CameraAnim->SetCameraAnim(30);
			m_GrapSolve = 0;
			CLawScript* Law = (CLawScript*)m_Target->GetScript(L"CLawScript");
			Law->SetPlayerGuardFail();

		}
	}
	if (m_eCurState == PLAYER_STATE::GRAPSOLVEFAILURE)
	{
		if (finish)
		{
			m_GrapTime += fDT;
			if (m_GrapTime > 0.5f) {
				m_GrapTime = 0.f;
				m_eCurState = PLAYER_STATE::IDLE;
				PostEffect_GlichLine->Transform()->SetLocalScale(Vec3(0.f, 0.f, 0.f));
				AttackEndComboState();
				m_Collignore = false;
			}
		}
	}
	if (m_eCurState == PLAYER_STATE::GRAPSOLVESUCCESS)
	{
		if (finish)
		{
			m_GrapTime += fDT;
			if (m_GrapTime > 0.5f) {
				m_GrapTime = 0.f;
				m_eCurState = PLAYER_STATE::IDLE;

				AttackEndComboState();
				m_Collignore = false;
			}
		}

	}
	if (m_eCurState == PLAYER_STATE::MELEEATTACK)
	{
		if (m_MeleeAtKSound1)
		{
			m_MeleeAtKSound1 = false;
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"lawmeleehit1");  //*********
			pSound->Play(1, 0.3f);
		}
		if (Ratio > 0.12f && m_MeleeAtKSound2)
		{
			m_MeleeAtKSound2 = false;
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"lawmeleehit2");  //*********
			pSound->Play(1, 0.3f);
		}
		if (Ratio > 0.25f && m_MeleeAtKSoundKick)
		{
			m_MeleeAtKSoundKick = false;
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"lawmeleehitkick");  //*********
			pSound->Play(1, 0.3f);
		}
		if (Ratio > 0.4f && m_MeleeAtKSound3)
		{
			m_MeleeAtKSound3 = false;
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"lawmeleehit3");  //*********
			pSound->Play(1, 0.3f);
		}

	}
	if (finish && m_eCurState == PLAYER_STATE::MELEEATTACK)
	{
		m_eCurState = PLAYER_STATE::IDLE;
		m_MeleeAtKSound1 = true;
		m_MeleeAtKSound2 = true;
		m_MeleeAtKSound3 = true;
		m_MeleeAtKSoundKick = true;
	}

	if (finish && m_eCurState == PLAYER_STATE::SWORDATTACKMISS)
	{
		m_eCurState = PLAYER_STATE::IDLE;
	}

	if (m_hit && m_bKnockdownstandup)
	{
		if (finish)
		{
			m_hit = false;
			m_Collignore = false;
			m_bKnockdownstandup = false;
			m_eCurState = PLAYER_STATE::IDLE;
		}
	}
	if (m_hit && !m_bKnockdownstandup)
	{
		if (finish)
		{

			if (m_bKnockdown) {
				m_fKnockDownTime += fDT;
				if (m_fKnockDownTime > 0.25f) {
					m_bKnockdownstandup = true;
					m_fKnockDownTime = 0.f;
					m_Animation->ChangeClip(UINT(MC_PLAYER::standUp), false);
					int a = 0;
					UINT ret = m_Animation->MeshRender()->GetMtrlCount();
					PostEffect_Vignette->Transform()->SetLocalScale(Vec3(0.f, 0.f, 0.f));
					for (int i = 0; i < ret; i++)
					{
						m_Animation->MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &a);
					}
					AttackEndComboState();
					m_eCurState = PLAYER_STATE::HIT;
				}
				//m_Collignore = false;
			}
			else
			{
				m_hit = false;
				int a = 0;
				UINT ret = m_Animation->MeshRender()->GetMtrlCount();
				PostEffect_Vignette->Transform()->SetLocalScale(Vec3(0.f, 0.f, 0.f));
				for (int i = 0; i < ret; i++)
				{
					m_Animation->MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &a);
				}
				AttackEndComboState();
				m_Collignore = false;
			}
		}
	}


	if ((m_eCurState == PLAYER_STATE::SWORDCHAGERATTACK || m_eCurState == PLAYER_STATE::GUNCHAGERATTACK) && Ratio > 0.5f)
	{
		m_AttackSpeed = 0.f;
		if (finish)
		{
			m_eCurState = PLAYER_STATE::IDLE;
			AttackEndComboState();
		}
	}

	if (m_eCurState == PLAYER_STATE::DASH && finish)
	{
		m_DashChargeTime = 0.3f;
		m_eCurState = PLAYER_STATE::IDLE;
		m_Collignore = false;
	}

	if (m_eCurState == PLAYER_STATE::PARRYING)
	{
		m_ParryTime += fDT;
		if (m_ParryTime > 0.5f)
		{
			m_ParryTime = 0.f;
			m_eCurState = PLAYER_STATE::PARRYEND;
		}
	}
	if (m_eCurState == PLAYER_STATE::PARRYEND && finish)
	{
		m_eCurState = PLAYER_STATE::IDLE;
	}
	if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE || m_eCurState == PLAYER_STATE::SWORDATTACK ||
		m_eCurState == PLAYER_STATE::GUNATTACK || m_eCurState == PLAYER_STATE::SWORDCHAGER || m_eCurState == PLAYER_STATE::GUNCHAGER
		|| m_eCurState == PLAYER_STATE::DASHCHAGER)
	{
		if (KEY_HOLD(KEY_TYPE::KEY_W))
		{
			if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE)
				m_eCurState = PLAYER_STATE::MOVE;
			m_eCurDir = DIR::UP;
		}
		if (KEY_HOLD(KEY_TYPE::KEY_S))
		{
			if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE)
				m_eCurState = PLAYER_STATE::MOVE;
			m_eCurDir = DIR::DOWN;

		}
		if (KEY_HOLD(KEY_TYPE::KEY_A))
		{
			if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE)
				m_eCurState = PLAYER_STATE::MOVE;
			m_eCurDir = DIR::LEFT;
		}
		if (KEY_HOLD(KEY_TYPE::KEY_D))
		{
			if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE)
				m_eCurState = PLAYER_STATE::MOVE;
			m_eCurDir = DIR::RIGHT;
		}
		if (KEY_HOLD(KEY_TYPE::KEY_W) && KEY_HOLD(KEY_TYPE::KEY_A))
		{
			if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE)
				m_eCurState = PLAYER_STATE::MOVE;
			m_eCurDir = DIR::UPLEFT;
		}
		if (KEY_HOLD(KEY_TYPE::KEY_W) && KEY_HOLD(KEY_TYPE::KEY_D))
		{
			if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE)
				m_eCurState = PLAYER_STATE::MOVE;
			m_eCurDir = DIR::UPRIGHT;
		}
		if (KEY_HOLD(KEY_TYPE::KEY_S) && KEY_HOLD(KEY_TYPE::KEY_A))
		{
			if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE)
				m_eCurState = PLAYER_STATE::MOVE;
			m_eCurDir = DIR::DOWNLEFT;
		}
		if (KEY_HOLD(KEY_TYPE::KEY_S) && KEY_HOLD(KEY_TYPE::KEY_D))
		{
			if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE)
				m_eCurState = PLAYER_STATE::MOVE;
			m_eCurDir = DIR::DOWNRIGHT;
		}
		if (KEY_NONE(KEY_TYPE::KEY_A) && KEY_NONE(KEY_TYPE::KEY_S) && KEY_NONE(KEY_TYPE::KEY_D) && KEY_NONE(KEY_TYPE::KEY_W))
		{
			m_eCurDir = DIR::NONE;
		}
		if (KEY_HOLD(KEY_TYPE::LBTN) && m_eCurState != PLAYER_STATE::SWORDATTACK)
		{
			Vec3 _pos = Transform()->GetLocalPos();
			if (m_SwordChargebool)
			{
				// ===== Effect ======
				// (22.1.3)
				if(m_AttackCharge)
				{
					m_SwordCharge =CResMgr::GetInst()->FindRes<CPrefab>(L"portal_ring_twisty2")->Instantiate();
					tEvent even = {};
					even.eEvent = EVENT_TYPE::CREATE_OBJECT;
					even.lParam = (DWORD_PTR)m_SwordCharge;
					even.wParam = (DWORD_PTR)2;
					CEventMgr::GetInst()->AddEvent(even);
					even = {};
					even.eEvent = EVENT_TYPE::ADD_CHILD;
					even.lParam = (DWORD_PTR)GetObj();
					even.wParam = (DWORD_PTR)m_SwordCharge;
					even.m_ChildNum = 0;
					CEventMgr::GetInst()->AddEvent(even);
					m_SwordCharge2 = CResMgr::GetInst()->FindRes<CPrefab>(L"magic_line")->Instantiate();
					even = {};
					even.eEvent = EVENT_TYPE::CREATE_OBJECT;
					even.lParam = (DWORD_PTR)m_SwordCharge2;
					even.wParam = (DWORD_PTR)2;
					CEventMgr::GetInst()->AddEvent(even);
					even = {};
					even.eEvent = EVENT_TYPE::ADD_CHILD;
					even.lParam = (DWORD_PTR)GetObj();
					even.wParam = (DWORD_PTR)m_SwordCharge2;
					even.m_ChildNum = 1;
					CEventMgr::GetInst()->AddEvent(even);
					/*
					m_SwordCharge = new CGameObject;
					m_SwordCharge->SetName(L"ElecEffect");
					m_SwordCharge->AddComponent(new CTransform);
					m_SwordCharge->AddComponent(new CParticleSystem);
					m_SwordCharge->AddComponent(new CEffectScript);

					if (m_SwordCharge)
						m_SwordCharge->Transform()->SetLocalPos(Vec3(_pos.x - 20.f, 100.f, _pos.z));

					m_SwordCharge->ParticleSystem()->SetPaticleName(L"eeeeeee");
					m_SwordCharge->ParticleSystem()->SetShaderIdx(3);

					m_SwordCharge->ParticleSystem()->SetiMaxCount(10);
					m_SwordCharge->ParticleSystem()->SetvCreateRange(Vec4(5.f, 30.f, 5.f, 0.f));
					m_SwordCharge->ParticleSystem()->SetvStartColor(Vec4(0.8f, 0.8f, 0.5f, 0.8f));
					m_SwordCharge->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.f, 1.f, 1.f));
					m_SwordCharge->ParticleSystem()->SetfMinSpeed(200);
					m_SwordCharge->ParticleSystem()->SetfMaxSpeed(200);
					m_SwordCharge->ParticleSystem()->SetvStartScale(Vec3(200.f, 0.f, 0.f));
					m_SwordCharge->ParticleSystem()->SetvEndScale(Vec3(100.f, 0.f, 0.f));
					m_SwordCharge->ParticleSystem()->SetDirection(-1);
					m_SwordCharge->ParticleSystem()->SetfMinLifeTime(0.1f);
					m_SwordCharge->ParticleSystem()->SetfMaxLifeTime(0.5f);
					m_SwordCharge->ParticleSystem()->SetfFrequency(0.1f);

					CEffectScript* BulletS = (CEffectScript*)m_SwordCharge->GetScript(L"CEffectScript");
					BulletS->SetType(EFFECT::DASH);
					BulletS->SetTime(6.0f);
					BulletS->SetStopTime(5.f);

					tEvent even = {};
					even.eEvent = EVENT_TYPE::ADD_CHILD;
					even.lParam = (DWORD_PTR)m_SwordCharge;
					even.wParam = (DWORD_PTR)2;
					CEventMgr::GetInst()->AddEvent(even);
					
					m_SwordCharge = new CGameObject;
					m_SwordCharge->SetName(L"attackEff");

					m_SwordCharge->AddComponent(new CTransform);
					m_SwordCharge->AddComponent(new CParticleSystem);
					m_SwordCharge->AddComponent(new CEffectScript);
					m_SwordCharge->Transform()->SetLocalPos(Vec3(_pos.x-20.f, 100.f, _pos.z));
					m_SwordCharge->ParticleSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
					m_SwordCharge->ParticleSystem()->SetfMinLifeTime(1.f);
					m_SwordCharge->ParticleSystem()->SetfMaxLifeTime(1.f);
					m_SwordCharge->ParticleSystem()->SetfMinSpeed(40.f);
					m_SwordCharge->ParticleSystem()->SetfMaxSpeed(60.f);
					m_SwordCharge->ParticleSystem()->SetfFrequency(0.001f);
					m_SwordCharge->ParticleSystem()->SetvCreateRange(Vec4(300.f, 300.f, 300.f, 1.f));
					m_SwordCharge->ParticleSystem()->SetvEndColor(Vec4(0.0f, 0.2f, 0.7f, 0.8f));
					m_SwordCharge->ParticleSystem()->SetvStartColor(Vec4(0.0f, 0.124f, 0.5f, 0.3f));
					m_SwordCharge->ParticleSystem()->SetvStartScale(Vec3(10.f, 10.f, 1.f));
					m_SwordCharge->ParticleSystem()->SetvEndScale(Vec3(10.f, 10.f, 1.f));
					m_SwordCharge->ParticleSystem()->SetDirection(0);

					CEffectScript* BulletS = (CEffectScript*)m_SwordCharge->GetScript(L"CEffectScript");
					BulletS->SetType(EFFECT::DASH);
					BulletS->SetTime(6.0f);
					BulletS->SetStopTime(5.f);

					tEvent even = {};
					even.eEvent = EVENT_TYPE::CREATE_OBJECT;
					even.lParam = (DWORD_PTR)m_SwordCharge;
					even.wParam = (DWORD_PTR)2;
					CEventMgr::GetInst()->AddEvent(even);
					*/
					m_SwordChargebool = false;
				}
				// ===== ~ Effect ======
				

			}

			if (m_AttackChargeTime > 0.3f)
			{
				m_AttackCharge = true;
				m_AttackSpeed = 2000.f;
				m_eCurState = PLAYER_STATE::SWORDCHAGER;
			}
			if (m_AttackChargeTime > ChargefullTime)
				m_AttackChargefull = true;
			m_AttackChargeTime += fDT;
			float Rot;
			if (m_CloseBattle)
				Rot = m_TargetAngle;
			else
				Rot = MouseRote();
			switch (m_eCurDir)
			{
			case DIR::UP:
			{
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::UP;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::LEFT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::DOWN;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::RIGHT;
				}
			}
			break;
			case DIR::DOWN:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::DOWN;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::RIGHT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::UP;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::LEFT;
				}
				break;
			case DIR::RIGHT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::RIGHT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::UP;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::LEFT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::DOWN;
				}
				break;
			case DIR::LEFT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::LEFT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::DOWN;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::RIGHT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::UP;
				}
				break;
			case DIR::UPRIGHT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::UPRIGHT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::UPLEFT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::DOWNLEFT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::DOWNRIGHT;
				}
				break;
			case DIR::UPLEFT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::UPLEFT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::DOWNLEFT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::DOWNRIGHT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::UPRIGHT;
				}
				break;
			case DIR::DOWNRIGHT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::DOWNRIGHT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::UPRIGHT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::UPLEFT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::DOWNLEFT;
				}
				break;
			case DIR::DOWNLEFT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::DOWNLEFT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::DOWNRIGHT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::UPRIGHT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::UPLEFT;
				}
				break;
			case DIR::NONE:
			{
				m_AnimDir = DIR::NONE;
			}
			break;
			}
		}

		if (KEY_AWAY(KEY_TYPE::LBTN))
		{

			if (m_SwordCharge)
			{
				
				m_SwordCharge->Transform()->SetLocalPos(Vec3(999999.f, 999999.f, 999999.f));
				m_SwordCharge2->Transform()->SetLocalPos(Vec3(999999.f, 999999.f, 999999.f));
				
				CEffectScript* BulletS = (CEffectScript*)m_SwordCharge->GetScript(L"CEffectScript");
				
				BulletS->SetTime(3.0f);
				BulletS->SetStopTime(3.0f);
				BulletS = (CEffectScript*)m_SwordCharge2->GetScript(L"CEffectScript");
				
				BulletS->SetTime(3.0f);
				BulletS->SetStopTime(3.0f);
				
				
				m_SwordCharge = nullptr;
			
				m_SwordCharge2 = nullptr;
			}
			m_SwordChargebool = true;
			m_AttackChargeTime = 0.f;
			if (!m_AttackCharge)
				Attacking();
			else
			{

				m_eCurState = PLAYER_STATE::SWORDCHAGERATTACK;
				m_AttackCharge = false;
			}
		}

		if (KEY_HOLD(KEY_TYPE::RBTN) || KEY_HOLD(KEY_TYPE::MIDDLEBTN))
		{
			if (m_eCurState == PLAYER_STATE::IDLE || m_eCurState == PLAYER_STATE::MOVE || m_eCurState == PLAYER_STATE::GUNATTACK
				|| m_eCurState == PLAYER_STATE::GUNCHAGER)
			{
				if (KEY_HOLD(KEY_TYPE::RBTN) && m_eCurState != PLAYER_STATE::GUNCHAGER)
				{
					m_eCurState = PLAYER_STATE::GUNATTACK;
					m_AttackSpeed = 300.f;
				}
				else if (KEY_HOLD(KEY_TYPE::MIDDLEBTN))
				{
					Vec3 _pos = Transform()->GetLocalPos();
					if (m_GunChargeSound)
					{
						Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"GunCharge");
						pSound->Play(0, 0.4f);
						m_GunChargeSound = false;


						// ===== Effect ======(22.1.3) 총

						{
							m_GunCharge = new CGameObject;
							m_GunCharge->SetName(L"attackEff");

							m_GunCharge->AddComponent(new CTransform);
							m_GunCharge->AddComponent(new CParticleSystem);
							m_GunCharge->AddComponent(new CEffectScript);
							m_GunCharge->Transform()->SetLocalPos(Vec3(0.f, 1.4f, 0.5f));
							//m_GunCharge->ParticleSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
							m_GunCharge->ParticleSystem()->SetShaderIdx(1);
							m_GunCharge->ParticleSystem()->SetPaticleName(L"Hit");//flak1");
							m_GunCharge->ParticleSystem()->SetfMinLifeTime(1.f);
							m_GunCharge->ParticleSystem()->SetfMaxLifeTime(1.f);
							m_GunCharge->ParticleSystem()->SetfMinSpeed(40.f);
							m_GunCharge->ParticleSystem()->SetfMaxSpeed(60.f);
							m_GunCharge->ParticleSystem()->SetfFrequency(0.001f);
							m_GunCharge->ParticleSystem()->SetvCreateRange(Vec4(30.f, 30.f, 30.f, 1.f));
							m_GunCharge->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.f, 1.f, 1.f));//
							m_GunCharge->ParticleSystem()->SetvStartColor(Vec4(0.f, 0.f, 0.8f, 0.8f));
							m_GunCharge->ParticleSystem()->SetvStartScale(Vec3(170.f, 10.f, 1.f));
							m_GunCharge->ParticleSystem()->SetvEndScale(Vec3(10.f, 10.f, 1.f));
							m_GunCharge->ParticleSystem()->SetDirection(0);


							CEffectScript* BulletS = (CEffectScript*)m_GunCharge->GetScript(L"CEffectScript");
							
							BulletS->SetTime(10.0f);
							BulletS->SetStopTime(10.f);
							
							tEvent even = {};
							even.eEvent = EVENT_TYPE::CREATE_OBJECT;
							even.lParam = (DWORD_PTR)m_GunCharge;
							even.wParam = (DWORD_PTR)2;
							CEventMgr::GetInst()->AddEvent(even);
							
							even = {};
							even.eEvent = EVENT_TYPE::ADD_CHILD;
							even.lParam = (DWORD_PTR)GetObj();
							even.wParam = (DWORD_PTR)m_GunCharge;
							even.m_ChildNum = 0;
							CEventMgr::GetInst()->AddEvent(even);
							
						}
						// ===== ~ Effect ======

					}
					if (m_AttackChargeTime > ChargefullTime)
						m_AttackChargefull = true;
					m_AttackChargeTime += fDT;
					m_eCurState = PLAYER_STATE::GUNCHAGER;
					m_AttackSpeed = 200.f;

					
				}
			}
			float Rot = MouseRote();

			switch (m_eCurDir)
			{
			case DIR::UP:
			{
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::UP;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::LEFT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::DOWN;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::RIGHT;
				}
			}
			break;
			case DIR::DOWN:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::DOWN;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::RIGHT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::UP;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::LEFT;
				}
				break;
			case DIR::RIGHT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::RIGHT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::UP;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::LEFT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::DOWN;
				}
				break;
			case DIR::LEFT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::LEFT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::DOWN;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::RIGHT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::UP;
				}
				break;
			case DIR::UPRIGHT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::UPRIGHT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::UPLEFT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::DOWNLEFT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::DOWNRIGHT;
				}
				break;
			case DIR::UPLEFT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::UPLEFT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::DOWNLEFT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::DOWNRIGHT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::UPRIGHT;
				}
				break;
			case DIR::DOWNRIGHT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::DOWNRIGHT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::UPRIGHT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::UPLEFT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::DOWNLEFT;
				}
				break;
			case DIR::DOWNLEFT:
				if (Rot > -0.785f && Rot < 0.785f)
				{
					m_AnimDir = DIR::DOWNLEFT;
				}
				else if (Rot >= 0.785f && Rot < 2.355f)
				{
					m_AnimDir = DIR::DOWNRIGHT;
				}
				else if (Rot > 2.355f || Rot < -2.355f)
				{
					m_AnimDir = DIR::UPRIGHT;
				}
				else if (Rot > -2.355f && Rot < -0.785f)
				{
					m_AnimDir = DIR::UPLEFT;
				}
				break;
			case DIR::NONE:
			{
				m_AnimDir = DIR::NONE;
			}
			break;
			}



		}
		if (KEY_AWAY(KEY_TYPE::RBTN))
			m_eCurState = PLAYER_STATE::IDLE;
		if (KEY_AWAY(KEY_TYPE::MIDDLEBTN) && m_eCurState == PLAYER_STATE::GUNCHAGER)
		{

			if (m_GunCharge)
			{
				m_GunCharge->Transform()->SetLocalPos(Vec3(99999.f, 99999.f, 99999.f));
			}
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"GunCharge");
			pSound->Stop();
			m_GunChargeSound = true;
			m_eCurState = PLAYER_STATE::GUNCHAGERATTACK;

			m_AttackChargeTime = 0.f;
		}

		if (KEY_HOLD(KEY_TYPE::SPACE) && !m_IsAttacking)
		{
			//AttackEndComboState();
			m_eCurState = PLAYER_STATE::DASHCHAGER;
			MouseRote();
			m_DashChargeTime += fDT;
			if (m_DashChargeTime > 1.f)
				m_DashChargeTime = 1.f;
		}
		if (KEY_AWAY(KEY_TYPE::SPACE) && !m_IsAttacking)
		{
			m_eCurState = PLAYER_STATE::DASH;

		}
		if (KEY_TAP(KEY_TYPE::KEY_F) && m_eCurState != PLAYER_STATE::PARRYING)
		{
			m_eCurState = PLAYER_STATE::PARRYING;
			if (m_Target != nullptr) {

				Transform()->SetLocalRot(Vec3(0, m_TargetAngle, 0));
			}
		}
		if (KEY_NONE(KEY_TYPE::KEY_A) && KEY_NONE(KEY_TYPE::KEY_S) && KEY_NONE(KEY_TYPE::KEY_D) && KEY_NONE(KEY_TYPE::KEY_W)
			&& KEY_NONE(KEY_TYPE::RBTN) && KEY_NONE(KEY_TYPE::LBTN)
			&& (m_eCurState != PLAYER_STATE::SWORDATTACK) && KEY_NONE(KEY_TYPE::MIDDLEBTN)
			&& KEY_NONE(KEY_TYPE::SPACE) && KEY_NONE(KEY_TYPE::KEY_F)
			)
		{
			m_AttackSpeed = 400.f;
			m_eCurState = PLAYER_STATE::IDLE;
		}
	}

	if (m_eCurState == PLAYER_STATE::SWORDATTACK)
	{
		if (m_Combo4AttackOn)
		{
			if (m_Combo4AttackCase < 2)
				m_AttackSpeed = 1200.f;
			else
				m_AttackSpeed = 200.f;
		}
		else
		{
			if (Ratio > 0.5)
				m_AttackSpeed = 300.f;
			else
			{
				m_AttackSpeed = 1200.f;
			}
		}
	}
	if (m_eCurState == PLAYER_STATE::PARRYING)
	{
		m_AttackSpeed = 0.f;
	}
	if (m_CloseBattle && m_eCurState == PLAYER_STATE::MOVE)
	{
		switch (m_eCurDir)
		{
		case DIR::UP:
		{
			if (m_TargetAngle > -0.785f && m_TargetAngle < 0.785f)
			{
				m_AnimDir = DIR::UP;
			}
			else if (m_TargetAngle >= 0.785f && m_TargetAngle < 2.355f)
			{
				m_AnimDir = DIR::LEFT;
			}
			else if (m_TargetAngle > 2.355f || m_TargetAngle < -2.355f)
			{
				m_AnimDir = DIR::DOWN;
			}
			else if (m_TargetAngle > -2.355f && m_TargetAngle < -0.785f)
			{
				m_AnimDir = DIR::RIGHT;
			}
		}
		break;
		case DIR::DOWN:
			if (m_TargetAngle > -0.785f && m_TargetAngle < 0.785f)
			{
				m_AnimDir = DIR::DOWN;
			}
			else if (m_TargetAngle >= 0.785f && m_TargetAngle < 2.355f)
			{
				m_AnimDir = DIR::RIGHT;
			}
			else if (m_TargetAngle > 2.355f || m_TargetAngle < -2.355f)
			{
				m_AnimDir = DIR::UP;
			}
			else if (m_TargetAngle > -2.355f && m_TargetAngle < -0.785f)
			{
				m_AnimDir = DIR::LEFT;
			}
			break;
		case DIR::RIGHT:
			if (m_TargetAngle > -0.785f && m_TargetAngle < 0.785f)
			{
				m_AnimDir = DIR::RIGHT;
			}
			else if (m_TargetAngle >= 0.785f && m_TargetAngle < 2.355f)
			{
				m_AnimDir = DIR::UP;
			}
			else if (m_TargetAngle > 2.355f || m_TargetAngle < -2.355f)
			{
				m_AnimDir = DIR::LEFT;
			}
			else if (m_TargetAngle > -2.355f && m_TargetAngle < -0.785f)
			{
				m_AnimDir = DIR::DOWN;
			}
			break;
		case DIR::LEFT:
			if (m_TargetAngle > -0.785f && m_TargetAngle < 0.785f)
			{
				m_AnimDir = DIR::LEFT;
			}
			else if (m_TargetAngle >= 0.785f && m_TargetAngle < 2.355f)
			{
				m_AnimDir = DIR::DOWN;
			}
			else if (m_TargetAngle > 2.355f || m_TargetAngle < -2.355f)
			{
				m_AnimDir = DIR::RIGHT;
			}
			else if (m_TargetAngle > -2.355f && m_TargetAngle < -0.785f)
			{
				m_AnimDir = DIR::UP;
			}
			break;
		case DIR::UPRIGHT:
			if (m_TargetAngle > -0.785f && m_TargetAngle < 0.785f)
			{
				m_AnimDir = DIR::UPRIGHT;
			}
			else if (m_TargetAngle >= 0.785f && m_TargetAngle < 2.355f)
			{
				m_AnimDir = DIR::UPLEFT;
			}
			else if (m_TargetAngle > 2.355f || m_TargetAngle < -2.355f)
			{
				m_AnimDir = DIR::DOWNLEFT;
			}
			else if (m_TargetAngle > -2.355f && m_TargetAngle < -0.785f)
			{
				m_AnimDir = DIR::DOWNRIGHT;
			}
			break;
		case DIR::UPLEFT:
			if (m_TargetAngle > -0.785f && m_TargetAngle < 0.785f)
			{
				m_AnimDir = DIR::UPLEFT;
			}
			else if (m_TargetAngle >= 0.785f && m_TargetAngle < 2.355f)
			{
				m_AnimDir = DIR::DOWNLEFT;
			}
			else if (m_TargetAngle > 2.355f || m_TargetAngle < -2.355f)
			{
				m_AnimDir = DIR::DOWNRIGHT;
			}
			else if (m_TargetAngle > -2.355f && m_TargetAngle < -0.785f)
			{
				m_AnimDir = DIR::UPRIGHT;
			}
			break;
		case DIR::DOWNRIGHT:
			if (m_TargetAngle > -0.785f && m_TargetAngle < 0.785f)
			{
				m_AnimDir = DIR::DOWNRIGHT;
			}
			else if (m_TargetAngle >= 0.785f && m_TargetAngle < 2.355f)
			{
				m_AnimDir = DIR::UPRIGHT;
			}
			else if (m_TargetAngle > 2.355f || m_TargetAngle < -2.355f)
			{
				m_AnimDir = DIR::UPLEFT;
			}
			else if (m_TargetAngle > -2.355f && m_TargetAngle < -0.785f)
			{
				m_AnimDir = DIR::DOWNLEFT;
			}
			break;
		case DIR::DOWNLEFT:
			if (m_TargetAngle > -0.785f && m_TargetAngle < 0.785f)
			{
				m_AnimDir = DIR::DOWNLEFT;
			}
			else if (m_TargetAngle >= 0.785f && m_TargetAngle < 2.355f)
			{
				m_AnimDir = DIR::DOWNRIGHT;
			}
			else if (m_TargetAngle > 2.355f || m_TargetAngle < -2.355f)
			{
				m_AnimDir = DIR::UPRIGHT;
			}
			else if (m_TargetAngle > -2.355f && m_TargetAngle < -0.785f)
			{
				m_AnimDir = DIR::UPLEFT;
			}
			break;
		case DIR::NONE:
		{
			m_AnimDir = DIR::NONE;
		}
		break;
		}


		Transform()->SetLocalRot(Vec3(0, m_TargetAngle, 0));
	}

}

void CPlayerScript::PlayAnimation()
{
	if (m_ePrevState != m_eCurState || m_PreAinmDir != m_AnimDir)
	{
		if (PLAYER_STATE::MOVE == m_eCurState)
		{
			if (m_CloseBattle)
			{




				switch (m_AnimDir)
				{
				case DIR::UP:
					m_Animation->ChangeClip(UINT(MC_PLAYER::swordLockWalk3m));
					break;
				case DIR::DOWN:
					m_Animation->ChangeClip(UINT(MC_PLAYER::swordLockWalkBack3m));
					break;
				case DIR::RIGHT:
					m_Animation->ChangeClip(UINT(MC_PLAYER::swordLockStrafeRight3m));
					break;
				case DIR::LEFT:
					m_Animation->ChangeClip(UINT(MC_PLAYER::swordLockStrafeLeft3m));
					break;
				case DIR::UPRIGHT:
					m_Animation->ChangeClip(UINT(MC_PLAYER::swordLockWalkStrafeRight3m));
					break;
				case DIR::UPLEFT:
					m_Animation->ChangeClip(UINT(MC_PLAYER::swordLockWalkStrafeLeft3m));
					break;
				case DIR::DOWNRIGHT:
					m_Animation->ChangeClip(UINT(MC_PLAYER::swordLockWalkBackStrafeRight3m));
					break;
				case DIR::DOWNLEFT:
					m_Animation->ChangeClip(UINT(MC_PLAYER::swordLockWalkBackStrafeLeft3m));
					break;
				case DIR::NONE:
					m_Animation->ChangeClip(UINT(MC_PLAYER::SwordIdle));
					break;
				}
			}
			else
				m_Animation->ChangeClip(UINT(MC_PLAYER::run12m));

		}
		else if (PLAYER_STATE::IDLE == m_eCurState)
		{
			if (m_CloseBattle)
			{
				m_Animation->ChangeClip(UINT(MC_PLAYER::SwordIdle));
				Transform()->SetLocalRot(Vec3(0, m_TargetAngle, 0));
			}
			else
				m_Animation->ChangeClip(UINT(MC_PLAYER::idle));
		}
		else if (PLAYER_STATE::SWORDATTACK == m_eCurState && m_CurrentCombo != 0)
		{

			switch (m_CurrentCombo)
			{
			case 1:
			{	Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SwordSlash_V3_SF_01");
			pSound->Play(1, 0.3f);

			m_Animation->ChangeClip(UINT(MC_PLAYER::SwordComboATK1), false);
			}
			break;
			case 2:
			{
				Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SwordSlash_V3_SF_02");
				pSound->Play(1, 0.3f);

				m_Animation->ChangeClip(UINT(MC_PLAYER::SwordComboATK2), false);
			}
			break;
			case 3:
			{	Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SwordSlash_V3_SF_03");
			pSound->Play(1, 0.3f);

			m_Animation->ChangeClip(UINT(MC_PLAYER::SwordComboATK3), false);
			}
			break;
			case 4:
				m_Combo4AttackOn = true;
				break;
			}

		}
		else if (PLAYER_STATE::GUNATTACK == m_eCurState)
		{
			switch (m_AnimDir)
			{
			case DIR::UP:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalk3m));
				break;
			case DIR::DOWN:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkBack3m));
				break;
			case DIR::RIGHT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimStrafeRight3m));
				break;
			case DIR::LEFT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimStrafeLeft3m));
				break;
			case DIR::UPRIGHT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkStrafeRight3m));
				break;
			case DIR::UPLEFT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkStrafeLeft3m));
				break;
			case DIR::DOWNRIGHT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkBackStrafeRight3m));
				break;
			case DIR::DOWNLEFT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkBackStrafeLeft3m));
				break;
			case DIR::NONE:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAim));
				break;
			}
		}
		else if (PLAYER_STATE::SWORDCHAGER == m_eCurState)
		{
			switch (m_AnimDir)
			{
			case DIR::UP:
				m_Animation->ChangeClip(UINT(MC_PLAYER::swordChargeWalk2m));
				break;
			case DIR::DOWN:
				m_Animation->ChangeClip(UINT(MC_PLAYER::swordChargeWalkBack2m));
				break;
			case DIR::RIGHT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::swordChargeStrafeRight2m));
				break;
			case DIR::LEFT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::swordChargeStrafeLeft2m));
				break;
			case DIR::UPRIGHT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::swordChargeWalkStrafeRight2m));
				break;
			case DIR::UPLEFT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::swordChargeWalkBackStrafeLeft2m));
				break;
			case DIR::DOWNRIGHT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::swordChargeWalkBackStrafeRight2m));
				break;
			case DIR::DOWNLEFT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::swordChargeWalkBackStrafeLeft2m));
				break;
			case DIR::NONE:
				m_Animation->ChangeClip(UINT(MC_PLAYER::SwordCharge));
				break;
			}
		}
		else if (PLAYER_STATE::SWORDCHAGERATTACK == m_eCurState)
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::SwordATK_Charged), false);
		}
		else if (PLAYER_STATE::GUNCHAGER == m_eCurState)
		{
			switch (m_AnimDir)
			{
			case DIR::UP:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalk1m));
				break;
			case DIR::DOWN:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkBack1m));
				break;
			case DIR::RIGHT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimStrafeRight1m));
				break;
			case DIR::LEFT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimStrafeLeft1m));
				break;
			case DIR::UPRIGHT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkStrafeRight1m));
				break;
			case DIR::UPLEFT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkStrafeLeft1m));
				break;
			case DIR::DOWNRIGHT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkBackStrafeRight1m));
				break;
			case DIR::DOWNLEFT:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunAimWalkBackStrafeLeft1m));
				break;
			case DIR::NONE:
				m_Animation->ChangeClip(UINT(MC_PLAYER::GunCharge));
				break;
			}
		}
		else if (PLAYER_STATE::GUNCHAGERATTACK == m_eCurState)
		{

			m_Animation->ChangeClip(UINT(MC_PLAYER::GunShot_Charged), false);
		}
		else if (PLAYER_STATE::DASHCHAGER == m_eCurState)
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::DodgeCharging));
		}
		else if (PLAYER_STATE::DASH == m_eCurState)
		{
			{
				CGameObject* pPar = new CGameObject;
				pPar->SetName(L"Dash");

				pPar->AddComponent(new CTransform);
				pPar->AddComponent(new CParticleSystem);
				pPar->AddComponent(new CEffectScript);
				pPar->ParticleSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
				pPar->ParticleSystem()->SetfMinLifeTime(1.f);
				pPar->ParticleSystem()->SetfMaxLifeTime(1.5f);
				pPar->ParticleSystem()->SetfFrequency(0.001f);
				pPar->ParticleSystem()->SetvCreateRange(Vec4(1.f, 1.f, 1.f, 1.f));
				pPar->ParticleSystem()->SetvStartColor(Vec4(1.f, 1.f, 1.f, 0.1f));
				pPar->ParticleSystem()->SetvEndColor(Vec4(0.1f, 0.1f, 0.1f, 1.f));
				pPar->ParticleSystem()->SetvStartScale(Vec3(10.f, 10.f, 1.f));
				pPar->ParticleSystem()->SetvEndScale(Vec3(50.f, 50.f, 1.f));

				CEffectScript* BulletS = (CEffectScript*)pPar->GetScript(L"CEffectScript");
				BulletS->SetType(EFFECT::DASH);
				BulletS->SetTime(1.6f);
				BulletS->SetStopTime(0.2f);

				tEvent even = {};
				even.eEvent = EVENT_TYPE::CREATE_OBJECT;
				even.lParam = (DWORD_PTR)pPar;
				even.wParam = (DWORD_PTR)1;
				CEventMgr::GetInst()->AddEvent(even);
			}
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"Dash");
			pSound->Play(1, 0.3f, true);
			m_Animation->ChangeClip(UINT(MC_PLAYER::DodgeInstantGun));
		}
		else if (PLAYER_STATE::PARRYING == m_eCurState)
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::ParryIdle));
		}
		else if (PLAYER_STATE::PARRYEND == m_eCurState)
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::ParryEnd));
		}
		else if (PLAYER_STATE::SWORDATTACKMISS == m_eCurState)
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::swordParried), false);
		}
		else if (PLAYER_STATE::MELEEATTACK == m_eCurState)
		{
			if (m_Target->GetName() == L"Law")
				m_Animation->ChangeClip(UINT(MC_PLAYER::meleeAtkLaw_Stance1), false);
			else if (m_Target->GetName() == L"")
				m_Animation->ChangeClip(UINT(MC_PLAYER::meleeAtkWise_Stance2), false);
		}
		else if (PLAYER_STATE::GRAP == m_eCurState)
		{
			if (m_Target->GetName() == L"Law")
				m_Animation->ChangeClip(UINT(MC_PLAYER::grabLawStance1Charge1), false);
		}
		else if (PLAYER_STATE::GRAPSOLVESUCCESS == m_eCurState)
		{
			if (m_Target->GetName() == L"Law")
				m_Animation->ChangeClip(UINT(MC_PLAYER::grabLawStance1Charge1Success), false);
		}
		else if (PLAYER_STATE::GRAPSOLVEFAILURE == m_eCurState)
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::grabLawStance1Charge2), false);
		}
	}



	if (PLAYER_STATE::SWORDATTACK == m_eCurState && m_CurrentCombo != m_PreCombo && m_CurrentCombo != 0)
	{
		switch (m_CurrentCombo)
		{
		case 1:
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::SwordComboATK1), false);
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SwordSlash_V3_SF_01");
			pSound->Play(1, 0.3f);

		}
		break;
		case 2:
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::SwordComboATK2), false);
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SwordSlash_V3_SF_02");
			pSound->Play(1, 0.3f);

		}
		break;
		case 3:
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::SwordComboATK3), false);
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SwordSlash_V3_SF_03");
			pSound->Play(1, 0.3f);

		}
		break;
		case 4:
			m_Combo4AttackOn = true;
			break;
		}
	}
	if (m_Combo4AttackOn == true && m_eCurState == PLAYER_STATE::SWORDATTACK)
	{
		switch (m_Combo4AttackCase)
		{
		case 0:
		{

			Ptr<CSound> pSound2 = CResMgr::GetInst()->FindRes<CSound>(L"AttackLight550");
			pSound2->Play(1, 0.3f);
			m_Animation->ChangeClip(UINT(MC_PLAYER::SwordComboATK4Start), false);
			m_Combo4AttackCase++;
		}
		break;
		case 1:
		{
			if (m_Animation->GetFinish())
				m_Combo4AttackCase++;
		}
		break;
		case 2:
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"SwordSlash_V3_Slice_06");
			pSound->Play(1, 0.3f);
			m_Animation->ChangeClip(UINT(MC_PLAYER::SwordComboATK4Idle), false);
			Transform()->SetLocalPosY(100.f);
			m_Combo4AttackCase++;
		}
		break;
		case 3:
		{
			if (m_Animation->GetFinish())
				m_Combo4AttackCase++;

		}
		break;
		case 4:
		{
			m_Animation->ChangeClip(UINT(MC_PLAYER::SwordComboATK4End), false);
			Transform()->SetLocalPosY(0.f);
			m_Combo4AttackCase++;
		}
		break;
		case 5:
		{
			if (m_Animation->GetFinish())
			{
				m_Combo4AttackCase = 0;
				m_Combo4AttackOn = false;
			}
		}
		break;
		}
	}
}

void CPlayerScript::PlayerAction()
{
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();

	if (m_hit)
	{
		vPos.x += -m_vhitDir.x * fDT * m_HitSpeed;
		vPos.z += -m_vhitDir.y * fDT * m_HitSpeed;
		Transform()->SetLocalPos(vPos);
		return;
	}

	if (m_eCurState == PLAYER_STATE::SWORDATTACK && !m_AttackMoveStop)
	{
		if (m_CloseBattle) {
			Vec3 TargetDir = m_Target->Transform()->GetLocalPos() - Transform()->GetLocalPos();
			TargetDir.Normalize();
			vPos.x += TargetDir.x * fDT * m_AttackSpeed;
			vPos.z += TargetDir.z * fDT * m_AttackSpeed;
		}
		else
		{
			float CDistance = Vec3::Distance(m_Target->Transform()->GetLocalPos(), Transform()->GetLocalPos());
			Vec3 TargetDir = m_Target->Transform()->GetLocalPos() - Transform()->GetLocalPos();
			TargetDir.Normalize();
			Vec3 AttackDir = Vec3(m_AttackDir.x, 0.f, m_AttackDir.y);
			float angle = AttackDir.Dot(TargetDir);
			if (angle < 0.f)
				angle = 6.28f - acosf(angle);
			else
				angle = acosf(angle);
			if (CDistance < 400 && angle < 0.52)
			{
				vPos.x += TargetDir.x * fDT * m_AttackSpeed;
				vPos.z += TargetDir.z * fDT * m_AttackSpeed;

			}
			else
			{
				vPos.x += m_AttackDir.x * fDT * m_AttackSpeed;
				vPos.z += m_AttackDir.y * fDT * m_AttackSpeed;
			}
		}
		Transform()->SetLocalPos(vPos);
	}

	else if (m_eCurState == PLAYER_STATE::SWORDCHAGERATTACK && !m_AttackMoveStop)
	{
		if (m_CloseBattle) {
			Vec3 TargetDir = m_Target->Transform()->GetLocalPos() - Transform()->GetLocalPos();
			TargetDir.Normalize();
			vPos.x += TargetDir.x * fDT * m_AttackSpeed;
			vPos.z += TargetDir.z * fDT * m_AttackSpeed;
		}
		else
		{
			float CDistance = Vec3::Distance(m_Target->Transform()->GetLocalPos(), Transform()->GetLocalPos());
			Vec3 TargetDir = m_Target->Transform()->GetLocalPos() - Transform()->GetLocalPos();
			TargetDir.Normalize();
			Vec3 AttackDir = Vec3(m_AttackDir.x, 0.f, m_AttackDir.y);
			float angle = AttackDir.Dot(TargetDir);
			if (angle < 0.f)
				angle = 6.28f - acosf(angle);
			else
				angle = acosf(angle);
			if (CDistance < 800 && angle < 0.26)
			{
				vPos.x += TargetDir.x * fDT * m_AttackSpeed;
				vPos.z += TargetDir.z * fDT * m_AttackSpeed;

			}
			else
			{
				vPos.x += m_AttackDir.x * fDT * m_AttackSpeed;
				vPos.z += m_AttackDir.y * fDT * m_AttackSpeed;
			}
		}
		Transform()->SetLocalPos(vPos);
	}

	if (m_eCurState == PLAYER_STATE::DASH)
	{
		m_Collignore = true;
		vPos.x += m_AttackDir.x * fDT * m_DashChargeTime * 6000.f;
		vPos.z += m_AttackDir.y * fDT * m_DashChargeTime * 6000.f;
		Transform()->SetLocalPos(vPos);
	}

	if (KEY_NONE(KEY_TYPE::KEY_W) && KEY_NONE(KEY_TYPE::KEY_S) && KEY_NONE(KEY_TYPE::KEY_A) && KEY_NONE(KEY_TYPE::KEY_D) ||
		m_eCurState == PLAYER_STATE::SWORDATTACK || m_eCurState == PLAYER_STATE::PARRYING || m_eCurState == PLAYER_STATE::DASHCHAGER
		|| m_eCurState == PLAYER_STATE::GUNCHAGERATTACK || m_eCurState == PLAYER_STATE::SWORDCHAGERATTACK
		)
		return;
		CalRay();
		if (!NavVectorCheck())
			return;
	switch (m_eCurDir)
	{
	case DIR::UP:
	{
		vPos.z += fDT * m_fPlayerSpeed;//* CurCamera->Transform()->GetWorldDir(DIR_TYPE::FRONT);
		if (m_eCurState == PLAYER_STATE::MOVE && !m_CloseBattle)
			vRot = { 0.f,0.f,0.f };
	}
	break;
	case DIR::DOWN:
	{
		vPos.z -= fDT * m_fPlayerSpeed;
		if (m_eCurState == PLAYER_STATE::MOVE && !m_CloseBattle)
			vRot = { 0.f,3.14f,0.f };
	}
	break;
	case DIR::LEFT:
	{
		vPos.x -= fDT * m_fPlayerSpeed;
		if (m_eCurState == PLAYER_STATE::MOVE && !m_CloseBattle)
			vRot = { 0.f,4.71f,0.f };
	}
	break;
	case DIR::RIGHT:
	{
		vPos.x += fDT * m_fPlayerSpeed;
		if (m_eCurState == PLAYER_STATE::MOVE && !m_CloseBattle)
			vRot = { 0.f,1.57f,0.f };
	}
	break;
	case DIR::DOWNLEFT:
	{

		vPos.z -= fDT * m_fPlayerSpeed / 1.4f;
		vPos.x -= fDT * m_fPlayerSpeed / 1.4f;
		if (m_eCurState == PLAYER_STATE::MOVE && !m_CloseBattle)
			vRot = { 0.f,3.925f,0.f };
	}
	break;
	case DIR::DOWNRIGHT:
	{

		vPos.z -= fDT * m_fPlayerSpeed / 1.4f;
		vPos.x += fDT * m_fPlayerSpeed / 1.4f;
		if (m_eCurState == PLAYER_STATE::MOVE && !m_CloseBattle)
			vRot = { 0.f,2.355f,0.f };
	}
	break;
	case DIR::UPLEFT:
	{

		vPos.z += fDT * m_fPlayerSpeed / 1.4f;
		vPos.x -= fDT * m_fPlayerSpeed / 1.4f;
		if (m_eCurState == PLAYER_STATE::MOVE && !m_CloseBattle)
			vRot = { 0.f,5.495f,0.f };
	}
	break;
	case DIR::UPRIGHT:
	{

		vPos.z += fDT * m_fPlayerSpeed / 1.4f;
		vPos.x += fDT * m_fPlayerSpeed / 1.4f;
		if (m_eCurState == PLAYER_STATE::MOVE && !m_CloseBattle)
			vRot = { 0.f,0.785f,0.f };
	}
	break;

	}
	Transform()->SetLocalPosX(vPos.x);
	Transform()->SetLocalPosZ(vPos.z);
	Transform()->SetLocalRot(vRot);
}

void CPlayerScript::Attacking()
{
	if (m_IsAttacking)
	{
		float n = m_Animation->GetAnimFrmRatio();
		if (m_CanNextCombo && 0.1 < n)
		{
			m_IsComboInputOn = true;
		}
	}
	else
	{
		AttackStartComboState();
		m_eCurState = PLAYER_STATE::SWORDATTACK;
		m_IsAttacking = true;
	}
}

void CPlayerScript::AttackStartComboState()
{
	if (!m_CloseBattle)
		MouseRote();


	m_CanNextCombo = true;
	m_IsComboInputOn = false;
	if (m_CurrentCombo + 1 > m_MaxCombo)
	{
		m_CurrentCombo = m_MaxCombo;

	}
	else if (m_CurrentCombo + 1 >= 1 && m_CurrentCombo + 1 <= m_MaxCombo)
	{
		m_CurrentCombo = m_CurrentCombo + 1;
	}
	else if (m_CurrentCombo + 1 < 1)
	{
		m_CurrentCombo = 1;
	}

}

void CPlayerScript::AttackEndComboState()
{
	m_IsAttacking = false;
	m_IsComboInputOn = false;
	m_CanNextCombo = false;
	m_CurrentCombo = 0;
	m_PreCombo = 0;
	m_eCurState = PLAYER_STATE::IDLE;
}

void CPlayerScript::MeleeAttack()
{
	AttackEndComboState();
	m_MeleeAtkOn = true;
	//m_eCurState = PLAYER_STATE::MELEEATTACK;

}

void CPlayerScript::Monsterguard()
{
	AttackEndComboState();
	m_AttackMiss = true;
	//m_eCurState = PLAYER_STATE::SWORDATTACKMISS;

}

void CPlayerScript::CalRay()
{
	m_tRay.vPoint = Transform()->GetLocalPos();
	switch (m_eCurDir)
	{
	case DIR::UP:
	{
		m_tRay.vPoint.z += fDT * m_fPlayerSpeed;//* CurCamera->Transform()->GetWorldDir(DIR_TYPE::FRONT);

	}
	break;
	case DIR::DOWN:
	{
		m_tRay.vPoint.z -= fDT * m_fPlayerSpeed;

	}
	break;
	case DIR::LEFT:
	{
		m_tRay.vPoint.x -= fDT * m_fPlayerSpeed;

	}
	break;
	case DIR::RIGHT:
	{
		m_tRay.vPoint.x += fDT * m_fPlayerSpeed;

	}
	break;
	case DIR::DOWNLEFT:
	{

		m_tRay.vPoint.z -= fDT * m_fPlayerSpeed / 1.4f;
		m_tRay.vPoint.x -= fDT * m_fPlayerSpeed / 1.4f;

	}
	break;
	case DIR::DOWNRIGHT:
	{

		m_tRay.vPoint.z -= fDT * m_fPlayerSpeed / 1.4f;
		m_tRay.vPoint.x += fDT * m_fPlayerSpeed / 1.4f;

	}
	break;
	case DIR::UPLEFT:
	{

		m_tRay.vPoint.z += fDT * m_fPlayerSpeed / 1.4f;
		m_tRay.vPoint.x -= fDT * m_fPlayerSpeed / 1.4f;

	}
	break;
	case DIR::UPRIGHT:
	{

		m_tRay.vPoint.z += fDT * m_fPlayerSpeed / 1.4f;
		m_tRay.vPoint.x += fDT * m_fPlayerSpeed / 1.4f;

	}
	break;
	}


}

bool CPlayerScript::PlayerNavCheck(Vec3 _vertices1, Vec3 _vertices2, Vec3 _vertices3, Vec3 _vStart, Vec3 _vDir)
{
	//직선과 평면의 방정식 , 메쉬 삼각형 면에 맞은 정점 구하기
	Vec3 edge[2] = { (Vec3)0.f, (Vec3)0.f };
	edge[0] = _vertices2 - _vertices1;
	edge[1] = _vertices3 - _vertices1;

	Vec3 normal = edge[0].Cross(edge[1]);
	//두벡터의 외적 방향은 두직선이 이루는 평면의 수직
	normal.Normalize();
	float b = normal.Dot(_vDir);
	//두벡터의 수직과 레이의 방향과 내적
	// b _vDir* cos a 
	Vec3 w0 = _vStart - _vertices1;
	//레이정점과 삼각형 정점 벡터
	float a = -normal.Dot(w0);
	//두벡터의 수직과 w0의 내적의 반대값
	// a = - w0 *cos b
	float t = a / b;


	Vec3 p = _vStart + t * _vDir;
	float uu, uv, vv, wu, wv, inverseD;
	uu = edge[0].Dot(edge[0]);
	uv = edge[0].Dot(edge[1]);
	vv = edge[1].Dot(edge[1]);

	Vec3 w = p - _vertices1;
	wu = w.Dot(edge[0]);
	wv = w.Dot(edge[1]);
	inverseD = uv * uv - uu * vv;
	inverseD = 1.0f / inverseD;

	float u = (uv * wv - vv * wu) * inverseD;
	if (u < 0.0f || u > 1.0f)
	{
		return 0;
	}

	float v = (uv * wu - uu * wv) * inverseD;
	if (v < 0.0f || (u + v) > 1.0f)
	{
		return 0;
	}

	return 1;


}
// -> CalRay()함수에 있음 , 누른 방향의 살짝 앞의 값을 예측


void CPlayerScript::AllNavVectorCheck()
{
	//cnavmesh magnaer 에서 위치 회전 크기 바꾼 네비메쉬저장한 벡터 가져옴
	vector<vector<NavMesh>*>* Nav = m_NavMeshManager->NavMeshVectorGet();
	//캐릭터가 어느 네비매쉬위에 있는지 모르기때문에 모든 네비매쉬 불러옴
	for (UINT i = 0; i < Nav->size(); i++) 
	{
		for (UINT l = 0; l < Nav->at(i)->size(); l++) //네비메쉬의 정보
		{
			// 네비메쉬의 삼각형 정점3개 와 내 Ray 를 이용하여
			//직선과 평면의 방정식 , 메쉬 삼각형 면에 맞은 정점 구하기
			if (PlayerNavCheck(Nav->at(i)->at(l).fPointPos[0], Nav->at(i)->at(l).fPointPos[1],
				Nav->at(i)->at(l).fPointPos[2], m_tRay.vPoint, m_tRay.vDir))
			{
				//내가 쏜 ray가 삼각형 안에 있으면 
				// (* 레이 위치값은 이동한 예상값이어야 함 그래야 갈수 있는지 아닌지 미리 조사할수있음)
				m_NavVector = Nav->at(i); //네비메쉬중 맞는 네비메쉬를 찾는것, 찾아서 저장
				m_NavNum = l; //찾은 네비메쉬 안의 삼각형중 몇번째 인덱스인지 저장
				break;
			}
		}
	}
}

//원래 밑의 함수를 쓰는것이 맞는데 문제때문에 실제 네비함수로는 이것을 썼다...
bool CPlayerScript::NavVectorCheck() //찾은 메쉬에서만 삼각형 찾기, 계속 모든 여러 네비메쉬를 조사하면 오래걸리므로
{
	for (UINT l = 0; l < m_NavVector->size(); l++)
	{
		if (PlayerNavCheck(m_NavVector->at(l).fPointPos[0], m_NavVector->at(l).fPointPos[1], m_NavVector->at(l).fPointPos[2], m_tRay.vPoint, m_tRay.vDir))
		{
			m_NavNum = l;
			{
				float y = m_NavVector->at(l).fPointPos[0].y +
					m_NavVector->at(l).fPointPos[1].y +
					m_NavVector->at(l).fPointPos[2].y;

				y = (y / 3.f) + 30.f;
				Transform()->SetLocalPosY(y);
			}
			return true;
		}
	}
	return false;
}

//이 함수의 문제점은 삼각형이 너무 작거나 속도가 빠르면 한번 이동할 때 
//1번에서 2번이 아니라 3번으로 넘어가 버려서 예측한 값이 false가 되어 이동을 못함
//update에서는 이 함수를 쓴다 

bool CPlayerScript::NavCheck() 
{
	// 네비 메쉬 데이터 저장할 때 한 정점에 달라붙어있는 인접 삼각형도 함께 저장을 했음
	// 지금 위에 있는 네비 메쉬의 "인접 삼각형" 만 조사
	for (UINT i = 0; i < m_NavVector->at(m_NavNum).vecAdjIdx.size(); i++)
	{
		if (PlayerNavCheck(m_NavVector->at(m_NavVector->at(m_NavNum).vecAdjIdx[i]).fPointPos[0],
			m_NavVector->at(m_NavVector->at(m_NavNum).vecAdjIdx[i]).fPointPos[1],
			m_NavVector->at(m_NavVector->at(m_NavNum).vecAdjIdx[i]).fPointPos[2], m_tRay.vPoint, m_tRay.vDir))
		{
			// 인접 삼각형을 조사해서 이동할 삼각형이 있으면 이동시키고 true리턴
			float y = m_NavVector->at(m_NavVector->at(m_NavNum).vecAdjIdx[i]).fPointPos[0].y +
				m_NavVector->at(m_NavVector->at(m_NavNum).vecAdjIdx[i]).fPointPos[1].y +
				m_NavVector->at(m_NavVector->at(m_NavNum).vecAdjIdx[i]).fPointPos[2].y;
			m_NavNum = m_NavVector->at(m_NavNum).vecAdjIdx[i];
			y = (y / 3.f) + 30.f; // y 값은 삼각형정점의 y값의 평균을 계산해서 약간 그 위로 서있을 수 있도록 임의의 값을 주어 고정시킴
			Transform()->SetLocalPosY(y);
			return true;
		}
	}
	//그렇지 않으면 이동x
	return false;
}

//마우스 방향으로 캐릭터 이동하게 하는 함수
float CPlayerScript::MouseRote()
{

	tRay d = CurCamera->GetRay();
	Vec3 Pos = Transform()->GetLocalPos() - CurCamera->Transform()->GetWorldPos();
	Pos.Normalize();
	d.vDir.x -= Pos.x;
	d.vDir.z -= Pos.z;
	m_AttackDir.x = d.vDir.x;
	m_AttackDir.y = d.vDir.z;
	m_AttackDir.Normalize();
	float m_angle = atan2f(d.vDir.x, d.vDir.z);	//접선의 기울기
	Transform()->SetLocalRot(Vec3(0, m_angle, 0));
	return m_angle;
}

bool CPlayerScript::KnockBack(Vec3 _CollPos, bool _Parrypossible, HitScale _HitScale, float _Damage)
{

	if (m_Collignore)
	{
		return false;
	}

	Vec3 _Pos = Transform()->GetLocalPos();
	m_vhitDir.x = _CollPos.x - _Pos.x;
	m_vhitDir.y = _CollPos.z - _Pos.z;
	m_vhitDir.Normalize();
	float m_TarAngle = atan2f(m_vhitDir.x, m_vhitDir.y);
	m_TarAngle = Transform()->GetLocalRot().y - m_TarAngle;

	if (m_eCurState == PLAYER_STATE::PARRYING && _Parrypossible)
	{
		m_Animation->ChangeClip(UINT(MC_PLAYER::hitParrying1Light), false);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"parry5");
		pSound->Play(1, 0.3f);
		return true;
	}
	else
	{
		m_CameraAnim->HitOn();
		m_bKnockdown = false;
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"HurtLight610");
		pSound->Play(1, 0.3f);
		if (m_TarAngle >= 0.f && m_TarAngle < 0.785f || m_TarAngle < 0.f && m_TarAngle >= -0.785f)
			m_Animation->ChangeClip(UINT(MC_PLAYER::hitReceived1Light), false, 2.0f);
		else if (m_TarAngle < -2.335f || m_TarAngle >= 2.335f)
			m_Animation->ChangeClip(UINT(MC_PLAYER::hitReceivedBack1Light), false, 2.0f);
		else if (m_TarAngle < -0.785f && m_TarAngle >= -2.335f)
			m_Animation->ChangeClip(UINT(MC_PLAYER::hitReceivedLeft1Light), false, 2.0f);
		else if (m_TarAngle < 2.335f && m_TarAngle >= 0.785f)
			m_Animation->ChangeClip(UINT(MC_PLAYER::hitReceivedRight1Light), false, 2.0f);
		switch (_HitScale)
		{
		case HitScale::HitLight:
			m_HitSpeed = 100.f;
			break;
		case HitScale::HitMedium:
			m_HitSpeed = 200.f;
			break;
		case HitScale::HitBig:
			m_HitSpeed = 300.f;
			break;

		}

		{
			PostEffect_Vignette->Transform()->SetLocalScale(Vec3(20000.f, 20000.f, 1.f));
			Vec3 _pos = Transform()->GetLocalPos();

			// ===================
			// Hit Effect (22.1.3)
			// ===================
			CGameObject* pHit = new CGameObject;
			pHit->SetName(L"HitEffect");
			pHit->AddComponent(new CTransform);
			pHit->AddComponent(new CParticleSystem);
			pHit->AddComponent(new CEffectScript);
			pHit->Transform()->SetLocalPos(Vec3(_pos.x, 100.f, _pos.z));
			pHit->ParticleSystem()->SetPaticleName(L"Hit");
			pHit->ParticleSystem()->SetShaderIdx(1);
			pHit->ParticleSystem()->SetiMaxCount(100);
			pHit->ParticleSystem()->SetvCreateRange(Vec4(2.f, 2.f, 1.f, 0.f));
			pHit->ParticleSystem()->SetvEndColor(Vec4(1.f, 0.f, 0.0f, 1.f));
			pHit->ParticleSystem()->SetvStartColor(Vec4(0.5f, 0.5f, 0.f, 0.5f));
			pHit->ParticleSystem()->SetfMinSpeed(2);
			pHit->ParticleSystem()->SetfMaxSpeed(5);
			pHit->ParticleSystem()->SetvStartScale(Vec3(20.f, 0.f, 0.f));
			pHit->ParticleSystem()->SetvEndScale(Vec3(300.f, 0.f, 0.f));
			pHit->ParticleSystem()->SetDirection(0);
			pHit->ParticleSystem()->SetfMinLifeTime(0.3f);
			pHit->ParticleSystem()->SetfMaxLifeTime(0.35f);
			pHit->ParticleSystem()->SetfFrequency(0.05f);

			CEffectScript* BulletS = (CEffectScript*)pHit->GetScript(L"CEffectScript");
			BulletS->SetType(EFFECT::NONE);
			BulletS->SetTime(0.5f);
			BulletS->SetStopTime(0.2f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pHit;
			even.wParam = (DWORD_PTR)2;
			CEventMgr::GetInst()->AddEvent(even);
			/*
			{
				CGameObject* pPar = new CGameObject;
				pPar->SetName(L"Teleport");

				pPar->AddComponent(new CTransform);
				pPar->AddComponent(new CParticleSystem);
				pPar->AddComponent(new CEffectScript);

				pPar->Transform()->SetLocalPos(Vec3(_pos.x, 100.f, _pos.z));
				pPar->ParticleSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
				pPar->ParticleSystem()->SetfMinLifeTime(0.5f);
				pPar->ParticleSystem()->SetfMaxLifeTime(0.5f);
				pPar->ParticleSystem()->SetfFrequency(0.01f);
				pPar->ParticleSystem()->SetvCreateRange(Vec4(100.f, 100.f, 100.f, 1.f));
				pPar->ParticleSystem()->SetvStartColor(Vec4(1.f, 0.f, 0.f, 1.f));
				pPar->ParticleSystem()->SetvEndColor(Vec4(0.9f, 0.2f, 0.2f, 0.3f));
				pPar->ParticleSystem()->SetvStartScale(Vec3(10.f, 10.f, 1.f));
				pPar->ParticleSystem()->SetvEndScale(Vec3(50.f, 50.f, 1.f));

				CEffectScript* BulletS = (CEffectScript*)pPar->GetScript(L"CEffectScript");
				BulletS->SetType(EFFECT::NONE);
				BulletS->SetTime(0.6f);
				BulletS->SetStopTime(0.1f);

				tEvent even = {};
				even.eEvent = EVENT_TYPE::CREATE_OBJECT;
				even.lParam = (DWORD_PTR)pPar;
				even.wParam = (DWORD_PTR)2;
				CEventMgr::GetInst()->AddEvent(even);

			}
			*/

		}


		m_hit = true;
		int a = 1;
		UINT ret = m_Animation->MeshRender()->GetMtrlCount();
		for (int i = 0; i < ret; i++)
		{
			m_Animation->MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &a);
		}
		m_fHP -= _Damage;
		if (m_fHP <= 0)
		{
			m_iLife--;
			if (m_iLife > 0)
				m_fHP = 100.f;
		}
		m_eCurState = PLAYER_STATE::HIT;
		m_Collignore = true;
	}

	return false;
}

bool CPlayerScript::knockdown(Vec3 _CollPos, bool _Parrypossible, HitScale _HitScale, float _Damage)
{
	if (m_Collignore)
	{
		return false;
	}
	Vec3 _Pos = Transform()->GetLocalPos();
	m_vhitDir.x = _CollPos.x - _Pos.x;
	m_vhitDir.y = _CollPos.z - _Pos.z;
	m_vhitDir.Normalize();
	float m_TarAngle = atan2f(m_vhitDir.x, m_vhitDir.y);
	m_TarAngle = Transform()->GetLocalRot().y - m_TarAngle;

	if (m_eCurState == PLAYER_STATE::PARRYING && _Parrypossible)
	{
		m_Animation->ChangeClip(UINT(MC_PLAYER::hitParrying1Light), false);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"parry5");
		pSound->Play(1, 0.3f);
		return true;
	}
	else
	{
		m_CameraAnim->HitOn();
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"HurtLight610");
		pSound->Play(1, 0.3f);
		m_bKnockdown = true;
		if (m_TarAngle >= 0.f && m_TarAngle < 0.785f || m_TarAngle < 0.f && m_TarAngle >= -0.785f)
			m_Animation->ChangeClip(UINT(MC_PLAYER::hitKnockdown1Front1Light), false, 2.0f);
		else if (m_TarAngle < -2.335f || m_TarAngle >= 2.335f)
			m_Animation->ChangeClip(UINT(MC_PLAYER::hitKnockdown4Back1Light), false, 2.0f);
		else if (m_TarAngle < -0.785f && m_TarAngle >= -2.335f)
			m_Animation->ChangeClip(UINT(MC_PLAYER::hitKnockdown2Left1Light), false, 2.0f);
		else if (m_TarAngle < 2.335f && m_TarAngle >= 0.785f)
			m_Animation->ChangeClip(UINT(MC_PLAYER::hitKnockdown3Right1Light), false, 2.0f);
		switch (_HitScale)
		{
		case HitScale::HitLight:
			m_HitSpeed = 100.f;
			break;
		case HitScale::HitMedium:
			m_HitSpeed = 200.f;
			break;
		case HitScale::HitBig:
			m_HitSpeed = 300.f;
			break;

		}


		{
			Vec3 _pos = Transform()->GetLocalPos();
			PostEffect_Vignette->Transform()->SetLocalScale(Vec3(20000.f, 20000.f, 1.f));
			// ===================
			// Hit Effect (22.1.3)
			// ===================
			CGameObject* pHit = new CGameObject;
			pHit->SetName(L"HitEffect");
			pHit->AddComponent(new CTransform);
			pHit->AddComponent(new CParticleSystem);
			pHit->AddComponent(new CEffectScript);
			pHit->Transform()->SetLocalPos(Vec3(_pos.x, 100.f, _pos.z));
			pHit->ParticleSystem()->SetPaticleName(L"Hit");
			pHit->ParticleSystem()->SetShaderIdx(1);
			pHit->ParticleSystem()->SetiMaxCount(100);
			pHit->ParticleSystem()->SetvCreateRange(Vec4(2.f, 2.f, 1.f, 0.f));
			pHit->ParticleSystem()->SetvEndColor(Vec4(1.f, 0.f, 0.0f, 1.f));
			pHit->ParticleSystem()->SetvStartColor(Vec4(0.5f, 0.5f, 0.f, 0.5f));
			pHit->ParticleSystem()->SetfMinSpeed(2);
			pHit->ParticleSystem()->SetfMaxSpeed(5);
			pHit->ParticleSystem()->SetvStartScale(Vec3(20.f, 0.f, 0.f));
			pHit->ParticleSystem()->SetvEndScale(Vec3(300.f, 0.f, 0.f));
			pHit->ParticleSystem()->SetDirection(0);
			pHit->ParticleSystem()->SetfMinLifeTime(0.3f);
			pHit->ParticleSystem()->SetfMaxLifeTime(0.35f);
			pHit->ParticleSystem()->SetfFrequency(0.05f);

			CEffectScript* BulletS = (CEffectScript*)pHit->GetScript(L"CEffectScript");
			BulletS->SetType(EFFECT::NONE);
			BulletS->SetTime(0.5f);
			BulletS->SetStopTime(0.2f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pHit;
			even.wParam = (DWORD_PTR)2;
			CEventMgr::GetInst()->AddEvent(even);

			/*
			{
				CGameObject* pPar = new CGameObject;
				pPar->SetName(L"Teleport");

				pPar->AddComponent(new CTransform);
				pPar->AddComponent(new CParticleSystem);
				pPar->AddComponent(new CEffectScript);

				pPar->Transform()->SetLocalPos(Vec3(_pos.x, 100.f, _pos.z));
				pPar->ParticleSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
				pPar->ParticleSystem()->SetfMinLifeTime(0.5f);
				pPar->ParticleSystem()->SetfMaxLifeTime(0.5f);
				pPar->ParticleSystem()->SetfFrequency(0.01f);
				pPar->ParticleSystem()->SetvCreateRange(Vec4(100.f, 100.f, 100.f, 1.f));
				pPar->ParticleSystem()->SetvStartColor(Vec4(1.f, 0.f, 0.f, 1.f));
				pPar->ParticleSystem()->SetvEndColor(Vec4(0.9f, 0.2f, 0.2f, 0.3f));
				pPar->ParticleSystem()->SetvStartScale(Vec3(10.f, 10.f, 1.f));
				pPar->ParticleSystem()->SetvEndScale(Vec3(50.f, 50.f, 1.f));

				CEffectScript* BulletS = (CEffectScript*)pPar->GetScript(L"CEffectScript");
				BulletS->SetType(EFFECT::NONE);
				BulletS->SetTime(0.6f);
				BulletS->SetStopTime(0.1f);

				tEvent even = {};
				even.eEvent = EVENT_TYPE::CREATE_OBJECT;
				even.lParam = (DWORD_PTR)pPar;
				even.wParam = (DWORD_PTR)2;
				CEventMgr::GetInst()->AddEvent(even);

			}
			*/
		}

		m_hit = true;
		int a = 1;
		UINT ret = m_Animation->MeshRender()->GetMtrlCount();
		for (int i = 0; i < ret; i++)
		{
			m_Animation->MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::INT_3, &a);
		}
		m_eCurState = PLAYER_STATE::HIT;

		m_fHP -= _Damage;
		if (m_fHP <= 0)
		{
			m_iLife--;
			if (m_iLife > 0)
				m_fHP = 100.f;
		}
		m_Collignore = true;
	}

	return false;
}

bool CPlayerScript::Grap()
{
	if (PLAYER_STATE::PARRYING == m_eCurState)
	{
		return false;
	}
		else
		{
			
			Transform()->SetLocalRot(Vec3(0, m_TargetAngle, 0));
			m_Collignore = true;
			m_Grap = true;
			return true;
		}
}


void CPlayerScript::OnCollisionEnter(CGameObject* _pOther)
{


}

void CPlayerScript::OnCollisionExit(CGameObject* _pOther)
{
	if (_pOther->GetName() == L"Law" || _pOther->GetName() == L"Wise" || _pOther->GetName() == L"Pillar" || _pOther->GetName() == L"Wall")
	{
		m_AttackMoveStop = false;
	}



}

void CPlayerScript::OnCollision(CGameObject* _pOther)
{
	if (_pOther->GetName() == L"Law" || _pOther->GetName() == L"Wise" || _pOther->GetName() == L"Pillar" || _pOther->GetName() == L"Wall")
	{
		Vec2 C_Dir = _pOther->Collider2D()->GetPos() - Collider2D()->GetPos();
		C_Dir.Normalize();
		C_Dir *= _pOther->Collider2D()->GetScale();


		if (abs(C_Dir.x) > abs(_pOther->Collider2D()->GetPos().x - Collider2D()->GetPos().x))
		{
			if (_pOther->Collider2D()->GetPos().x > Collider2D()->GetPos().x)
			{
				float x = C_Dir.x - abs(_pOther->Collider2D()->GetPos().x - Collider2D()->GetPos().x);
				float asda = Transform()->GetLocalPos().x;
				x = Transform()->GetLocalPos().x - x;
				Transform()->SetLocalPosX(x);
			}
			else
			{
				float x = abs(C_Dir.x) - abs(_pOther->Collider2D()->GetPos().x - Collider2D()->GetPos().x);
				x = Transform()->GetLocalPos().x + x;
				Transform()->SetLocalPosX(x);
			}
		}
		if (abs(C_Dir.y) > abs(_pOther->Collider2D()->GetPos().y - Collider2D()->GetPos().y))
		{
			if (_pOther->Collider2D()->GetPos().y > Collider2D()->GetPos().y)
			{
				float y = C_Dir.y - abs(_pOther->Collider2D()->GetPos().y - Collider2D()->GetPos().y);
				y = Transform()->GetLocalPos().z - y;
				Transform()->SetLocalPosZ(y);
			}
			else
			{
				float y = abs(C_Dir.y) - abs(_pOther->Collider2D()->GetPos().y - Collider2D()->GetPos().y);
				y = Transform()->GetLocalPos().z + y;
				Transform()->SetLocalPosZ(y);
			}
		}
		m_AttackMoveStop = true;
	}

	if (_pOther->GetName() == L"c")
	{
		Vec2 C_Dir = Collider2D()->GetPos() - _pOther->Collider2D()->GetPos();
		float dis = Vec2::Distance(Collider2D()->GetPos(), _pOther->Collider2D()->GetPos());


		if (dis >= _pOther->Collider2D()->GetScale().x / 2.f)
		{
			C_Dir.Normalize();
			float x = _pOther->Collider2D()->GetPos().x + C_Dir.x * _pOther->Collider2D()->GetScale().x / 2.f;
			if (abs(Collider2D()->GetPos().x) >= abs(x));
			{
				Transform()->SetLocalPosX(x);
			}
		}

		if (dis >= _pOther->Collider2D()->GetScale().y / 2.f)
		{
			C_Dir.Normalize();
			float y = _pOther->Collider2D()->GetPos().y + C_Dir.y * _pOther->Collider2D()->GetScale().y / 2.f;
			if (abs(Collider2D()->GetPos().y) >= abs(y));
			{
				Transform()->SetLocalPosZ(y);
			}
		}

	}

}

void CPlayerScript::SaveToScene(FILE* _pFile)
{
	CScript::SaveToScene(_pFile);
}

void CPlayerScript::LoadFromScene(FILE* _pFile)
{
	CScript::LoadFromScene(_pFile);
}

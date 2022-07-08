#include "pch.h"
#include "CWiseScript.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CEventMgr.h>
#include <Engine/CScene.h>

#include <Engine/CTransform.h>
#include <Engine/CCollider2D.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CParticleSystem.h>

#include "CPlayerScript.h"
#include "CMissileScript.h"
#include "CMissileScript2.h"
#include "CMissileScript3.h"
#include "CPillarScript.h"
#include "CEffectScript.h"
#include "CCameraAnimaitorScript.h"

#include <random>

CWiseScript::CWiseScript() :
	CScript((int)SCRIPT_TYPE::WISESCRIPT)
	, m_pPlayer(nullptr)
	, m_Phase(WISE_PHASE::PHASE_1)
	, m_State(WISE_STATE::IDLE)
	, m_prevState(WISE_STATE::NONE)
	, m_changeState(WISE_STATE::NONE)
	, m_Move(WISE_MOVE::TELEPORT)
	, m_Attack(WISE_ATTACK::TYPE1)
	, m_Shoot(WISE_SHOOT::CIRCLE)
	, m_Special(WISE_SPECIAL::FIRST)
	, m_vPos(Vec3(0.f, 0.f, 0.f))
	, m_vDir(Vec2(0.f, 0.f))
	, m_fSpeed(700.f)
	, m_fHP(100.f)
	, m_iHpToken(2)
	, m_iHpColor(1)
	, m_iLifeCount(3)
	, m_i3PhaseDead(7)
	, m_iPatternRepeat(4)
	, m_bRed(false)
	, m_RedTime(0.f)
	, m_RedTimeMax(1.f)
	, m_bPhaseShoot(true)
	, m_bZoomIn(false)
	, m_bMoveCheck(true)
	, m_bCollision(true)
	, m_fShootTimeMax(5.f)
	, m_fShootTime(3.f)
	, m_fShootTimeMax2(5.f)
	, m_fShootTime2(0.f)
	, m_fEndTimeMax(10.f)
	, m_fEndTime(0.f)
	, m_iFloorNum(0.f)
	, m_AnimObj(nullptr)
	, m_AnimPhase(WISE_ANIMPHASE::FIRST)
	, m_fAnimTimeMax(1.f)
	, m_fAnimTime(0.f)
	, m_bAnim(false)
	, m_bCameraZoom(false)
{
	//AddDesc(tDataDesc(SCRIPT_DATA_TYPE::FLOAT, "Iaaa", &testangle));
}

CWiseScript::~CWiseScript() {}


void CWiseScript::awake()
{
	m_pPlayer = CSceneMgr::GetInst()->FindObjectByName(L"Player");

	m_Pillar = CResMgr::GetInst()->FindRes<CMeshData>(L"//meshdata\\Wise_ShieldPillar.mdat");

	// Projectiles
	m_Yellow = CResMgr::GetInst()->FindRes<CPrefab>(L"YellowBullet");
	m_Slash = CResMgr::GetInst()->FindRes<CPrefab>(L"WiseSlash");
	m_Spread = CResMgr::GetInst()->FindRes<CPrefab>(L"SpreadBullet");
	m_Fanwise = CResMgr::GetInst()->FindRes<CPrefab>(L"Fanwise");

	// Animations
	m_AnimObj = GetObj()->GetChild()[0];
	m_vecAnimClip = m_AnimObj->Animator3D()->GetAnimClip();

	//m_CameraAnim = (CCameraAnimaitorScript*)CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Camera_bn")->GetScript(L"CCameraAnimaitorScript");
	//m_CameraAnim->SetTargerMonster(GetGameObject());
}

void CWiseScript::update()
{
	m_vPos = Transform()->GetLocalPos();

	ChangeState();

	switch (m_State)
	{
	case WISE_STATE::IDLE:
		Idle();
		break;
	case WISE_STATE::MOVE:
		Move();
		break;
	case WISE_STATE::CHARGE:
		break;
	case WISE_STATE::ATTACK:
		Attack();
		break;
	case WISE_STATE::SHOOT:
		Shoot();
		break;
	case WISE_STATE::STUN:
		break;
	case WISE_STATE::HIT:
		Hit();
		break;
	case WISE_STATE::KNOCKDOWN:
		Knockdown();
		break;
	case WISE_STATE::DIE:
		Die();
		break;
	case WISE_STATE::SPECIAL:
		Special();
		break;
	default:
		break;
	}

	LookAt(m_pPlayer->Transform()->GetLocalPos());

	CheckAnimation();

	ForRed();

	// 현재상태를 이전상태로 저장해둠
	m_prevState = m_State;

	Transform()->SetLocalPos(m_vPos);
}

void CWiseScript::CheckAnimation()
{
	if (m_prevState != m_State)
	{
		switch (m_State)
		{
		case WISE_STATE::IDLE:
		{
			switch (m_Phase)
			{
			case WISE_PHASE::PHASE_1:
				m_AnimObj->Animator3D()->ChangeClip(L"NPC_Wise=stance1_idle", true);
				break;
			case WISE_PHASE::PHASE_2:
			{
				if (m_vPos.y == 0.f)
					m_vPos.y = 300.f;

				m_bCollision = false;
				m_AnimObj->Animator3D()->ChangeClip(L"NPC_Wise=stance2_IdlePillar", true);
			}
			case WISE_PHASE::PHASE_3:
			{
				m_bCollision = false;
				m_AnimObj->Animator3D()->ChangeClip(L"NPC_Wise=stance2_fury2Idle", true);
			}
			break;
			case WISE_PHASE::PHASE_4:
			{
				m_bCollision = false;
				m_AnimObj->Animator3D()->ChangeClip(L"NPC_Wise=stance2_fury4Idle", true);
			}
			break;
			}
		}
		break;
		case WISE_STATE::MOVE:
		{
			switch (m_Move)
			{
			case WISE_MOVE::LEFT:
			{
				switch (m_AnimPhase)
				{
				case WISE_ANIMPHASE::FIRST:
				{
					m_bMoveCheck = true;

					PlayAnimToEnd(L"NPC_Wise=stance2_Dash2Left1Start", false, WISE_STATE::MOVE);
					m_AnimPhase = WISE_ANIMPHASE::SECOND;
				}
				break;
				case WISE_ANIMPHASE::SECOND:
				{
					PlayAnimToEnd(L"NPC_Wise=stance2_Dash2Left2Idle", false, WISE_STATE::MOVE);
					m_AnimPhase = WISE_ANIMPHASE::THIRD;
				}
				break;
				case WISE_ANIMPHASE::THIRD:
				{
					m_iPatternRepeat--;
					if (!m_iPatternRepeat)
						m_Move = WISE_MOVE::TELEPORT_OUT;
					PlayAnimToEnd(L"NPC_Wise=stance2_Dash2Left3End", false, WISE_STATE::MOVE);
					m_AnimPhase = WISE_ANIMPHASE::FIRST;
				}
				break;
				default:
					break;
				}
			}
			break;
			case WISE_MOVE::FRONT:
			{
				switch (m_AnimPhase)
				{
				case WISE_ANIMPHASE::FIRST:
				{
					m_bMoveCheck = true;

					PlayAnimToEnd(L"NPC_Wise=stance2_Dash1Forward1Start", false, WISE_STATE::MOVE, 0.3f, false);
					m_AnimPhase = WISE_ANIMPHASE::SECOND;
				}
				break;
				case WISE_ANIMPHASE::SECOND:
				{
					PlayAnimToEnd(L"NPC_Wise=stance2_Dash1Forward2Idle", false, WISE_STATE::MOVE, 0.3f, false);
					m_AnimPhase = WISE_ANIMPHASE::THIRD;
				}
				break;
				case WISE_ANIMPHASE::THIRD:
				{
					m_iPatternRepeat--;
					if (!m_iPatternRepeat)
					{
						PlayAnimToEnd(L"NPC_Wise=stance2_Dash1Forward3End2", false, WISE_STATE::ATTACK, 0.3f, false);
						m_State = WISE_STATE::NONE;

						if (m_iHpToken == 1)
							m_Attack = WISE_ATTACK::TYPE2;
						else
							m_Attack = WISE_ATTACK::TYPE1;
					}
					else
						PlayAnimToEnd(L"NPC_Wise=stance2_Dash1Forward3End2", false, WISE_STATE::MOVE, 0.3f, false);
					m_AnimPhase = WISE_ANIMPHASE::FIRST;
				}
				break;
				default:
					break;
				}
			}
			break;
			case WISE_MOVE::BACK:
			{
				switch (m_AnimPhase)
				{
				case WISE_ANIMPHASE::FIRST:
				{
					m_bMoveCheck = true;

					PlayAnimToEnd(L"NPC_Wise=stance2_Dash4Backward1Start", false, WISE_STATE::MOVE);
					m_AnimPhase = WISE_ANIMPHASE::SECOND;
				}
				break;
				case WISE_ANIMPHASE::SECOND:
				{
					PlayAnimToEnd(L"NPC_Wise=stance2_Dash4Backward2Idle", false, WISE_STATE::MOVE);
					m_AnimPhase = WISE_ANIMPHASE::THIRD;
				}
				break;
				case WISE_ANIMPHASE::THIRD:
				{
					m_iPatternRepeat--;
					if (!m_iPatternRepeat)
					{
						PlayAnimToEnd(L"NPC_Wise=stance2_Dash4Backward3End", false, WISE_STATE::SHOOT);
						m_Shoot = WISE_SHOOT::FANWISE;
					}
					else
						PlayAnimToEnd(L"NPC_Wise=stance2_Dash4Backward3End", false, WISE_STATE::MOVE);
					m_AnimPhase = WISE_ANIMPHASE::FIRST;
				}
				break;
				default:
					break;
				}
			}
			break;
			case WISE_MOVE::TELEPORT:
			case WISE_MOVE::TELEPORT_CENTER:
			{
				//

				switch (m_AnimPhase)
				{
				case WISE_ANIMPHASE::FIRST:
				{
					PlayAnimToEnd(L"NPC_Wise=stance2_TeleportTKStart", false, WISE_STATE::MOVE, 0.3f);
					m_AnimPhase = WISE_ANIMPHASE::SECOND;

					Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"WiseTeleport");
					pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

					for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 10))
					{
						CGameObject* pPar = new CGameObject;
						pPar->SetName(L"TeleportUp");

						pPar->AddComponent(new CTransform);
						pPar->AddComponent(new CParticleSystem);
						pPar->AddComponent(new CEffectScript);

						Vec2 vDir;
						vDir.x = cos(Angle) * 100.f;
						vDir.y = sin(Angle) * 100.f;

						pPar->Transform()->SetLocalPos(Vec3(m_vPos.x + vDir.x, 0.f, m_vPos.z + vDir.y));

						pPar->ParticleSystem()->SetfMinLifeTime(0.5f);
						pPar->ParticleSystem()->SetfMaxLifeTime(0.5f);
						pPar->ParticleSystem()->SetfFrequency(0.01f);
						pPar->ParticleSystem()->SetvCreateRange(Vec4(100.f, 100.f, 100.f, 1.f));
						pPar->ParticleSystem()->SetvStartColor(Vec4(1.f, 1.f, 1.f, 1.f));
						pPar->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.f, 0.f, 1.f));
						pPar->ParticleSystem()->SetvStartScale(Vec3(10.f, 10.f, 1.f));
						pPar->ParticleSystem()->SetvEndScale(Vec3(10.f, 10.f, 1.f));

						CEffectScript* BulletS = (CEffectScript*)pPar->GetScript(L"CEffectScript");
						BulletS->SetType(EFFECT::TELPO_UP);
						BulletS->SetDir(Vec3(0.f, 1.f, 0.f));
						BulletS->SetSpeed(1000.f);
						BulletS->SetTime(2.f);
						BulletS->SetStopTime(0.5f);

						tEvent even = {};
						even.eEvent = EVENT_TYPE::CREATE_OBJECT;
						even.lParam = (DWORD_PTR)pPar;
						even.wParam = (DWORD_PTR)2;
						CEventMgr::GetInst()->AddEvent(even);
					}
				}
				break;
				case WISE_ANIMPHASE::SECOND:
				{
					if (!m_iPatternRepeat)
						PlayAnimToEnd(L"NPC_Wise=stance2_TeleportTKEnd", false, WISE_STATE::SPECIAL, 0.3f);
					else
					{
						PlayAnimToEnd(L"NPC_Wise=stance2_TeleportTKEnd", false, WISE_STATE::SHOOT, 0.3f);
						m_Shoot = WISE_SHOOT::CIRCLE;
					}
					m_AnimPhase = WISE_ANIMPHASE::FIRST;

					for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 10))
					{
						CGameObject* pPar = new CGameObject;
						pPar->SetName(L"TeleportDown");

						pPar->AddComponent(new CTransform);
						pPar->AddComponent(new CParticleSystem);
						pPar->AddComponent(new CEffectScript);

						Vec2 vDir;
						vDir.x = cos(Angle) * 100.f;
						vDir.y = sin(Angle) * 100.f;

						pPar->Transform()->SetLocalPos(Vec3(m_vPos.x + vDir.x, 500.f, m_vPos.z + vDir.y));

						pPar->ParticleSystem()->SetfMinLifeTime(0.5f);
						pPar->ParticleSystem()->SetfMaxLifeTime(0.5f);
						pPar->ParticleSystem()->SetfFrequency(0.01f);
						pPar->ParticleSystem()->SetvCreateRange(Vec4(1.f, 1.f, 1.f, 1.f));
						pPar->ParticleSystem()->SetvStartColor(Vec4(1.f, 1.f, 1.f, 1.f));
						pPar->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.f, 0.f, 1.f));
						pPar->ParticleSystem()->SetvStartScale(Vec3(10.f, 10.f, 1.f));
						pPar->ParticleSystem()->SetvEndScale(Vec3(10.f, 10.f, 1.f));

						CEffectScript* BulletS = (CEffectScript*)pPar->GetScript(L"CEffectScript");
						BulletS->SetType(EFFECT::TELPO_DOWN);
						BulletS->SetDir(Vec3(0.f, -1.f, 0.f));
						vDir.Normalize();
						BulletS->SetDir2(Vec3(vDir.x, 0.f, vDir.y));
						BulletS->SetSpeed(1000.f);
						BulletS->SetTime(2.f);
						BulletS->SetStopTime(0.7f);

						tEvent even = {};
						even.eEvent = EVENT_TYPE::CREATE_OBJECT;
						even.lParam = (DWORD_PTR)pPar;
						even.wParam = (DWORD_PTR)2;
						CEventMgr::GetInst()->AddEvent(even);
					}
				}
				default:
					break;
				}
			}
			break;
			case WISE_MOVE::TELEPORT_OUT:
			{
				PlayAnimToEnd(L"NPC_Wise=stance2_TeleportTKStart", false, WISE_STATE::SPECIAL, 0.3f);
				m_Special = WISE_SPECIAL::THIRD;

				Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"WiseTeleport");
				pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

				for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 10))
				{
					CGameObject* pPar = new CGameObject;
					pPar->SetName(L"TeleportUp");

					pPar->AddComponent(new CTransform);
					pPar->AddComponent(new CParticleSystem);
					pPar->AddComponent(new CEffectScript);

					Vec2 vDir;
					vDir.x = cos(Angle) * 100.f;
					vDir.y = sin(Angle) * 100.f;

					pPar->Transform()->SetLocalPos(Vec3(m_vPos.x + vDir.x, 0.f, m_vPos.z + vDir.y));

					pPar->ParticleSystem()->SetfMinLifeTime(0.5f);
					pPar->ParticleSystem()->SetfMaxLifeTime(0.5f);
					pPar->ParticleSystem()->SetfFrequency(0.01f);
					pPar->ParticleSystem()->SetvCreateRange(Vec4(100.f, 100.f, 100.f, 1.f));
					pPar->ParticleSystem()->SetvStartColor(Vec4(1.f, 1.f, 1.f, 1.f));
					pPar->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.f, 0.f, 1.f));
					pPar->ParticleSystem()->SetvStartScale(Vec3(10.f, 10.f, 1.f));
					pPar->ParticleSystem()->SetvEndScale(Vec3(10.f, 10.f, 1.f));

					CEffectScript* BulletS = (CEffectScript*)pPar->GetScript(L"CEffectScript");
					BulletS->SetType(EFFECT::TELPO_UP);
					BulletS->SetDir(Vec3(0.f, 1.f, 0.f));
					BulletS->SetSpeed(1000.f);
					BulletS->SetTime(2.f);
					BulletS->SetStopTime(0.5f);

					tEvent even = {};
					even.eEvent = EVENT_TYPE::CREATE_OBJECT;
					even.lParam = (DWORD_PTR)pPar;
					even.wParam = (DWORD_PTR)2;
					CEventMgr::GetInst()->AddEvent(even);
				}
			}
			break;
			case WISE_MOVE::TELEPORT_IN:
			{
				Vec3 vPlayerPos = m_pPlayer->Transform()->GetLocalPos();
				m_vPos = Vec3(vPlayerPos.x - 200.f, 0.f, vPlayerPos.z);

				PlayAnimToEnd(L"NPC_Wise=stance2_TeleportTKEnd", false, WISE_STATE::ATTACK, 0.3f);
				m_Attack = WISE_ATTACK::TYPE1;

				for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 10))
				{
					CGameObject* pPar = new CGameObject;
					pPar->SetName(L"TeleportDown");

					pPar->AddComponent(new CTransform);
					pPar->AddComponent(new CParticleSystem);
					pPar->AddComponent(new CEffectScript);

					Vec2 vDir;
					vDir.x = cos(Angle) * 100.f;
					vDir.y = sin(Angle) * 100.f;

					pPar->Transform()->SetLocalPos(Vec3(m_vPos.x + vDir.x, 500.f, m_vPos.z + vDir.y));

					pPar->ParticleSystem()->SetfMinLifeTime(0.5f);
					pPar->ParticleSystem()->SetfMaxLifeTime(0.5f);
					pPar->ParticleSystem()->SetfFrequency(0.01f);
					pPar->ParticleSystem()->SetvCreateRange(Vec4(1.f, 1.f, 1.f, 1.f));
					pPar->ParticleSystem()->SetvStartColor(Vec4(1.f, 1.f, 1.f, 1.f));
					pPar->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.f, 0.f, 1.f));
					pPar->ParticleSystem()->SetvStartScale(Vec3(10.f, 10.f, 1.f));
					pPar->ParticleSystem()->SetvEndScale(Vec3(10.f, 10.f, 1.f));

					CEffectScript* BulletS = (CEffectScript*)pPar->GetScript(L"CEffectScript");
					BulletS->SetType(EFFECT::TELPO_DOWN);
					BulletS->SetDir(Vec3(0.f, -1.f, 0.f));
					vDir.Normalize();
					BulletS->SetDir2(Vec3(vDir.x, 0.f, vDir.y));
					BulletS->SetSpeed(1000.f);
					BulletS->SetTime(2.f);
					BulletS->SetStopTime(0.7f);

					tEvent even = {};
					even.eEvent = EVENT_TYPE::CREATE_OBJECT;
					even.lParam = (DWORD_PTR)pPar;
					even.wParam = (DWORD_PTR)2;
					CEventMgr::GetInst()->AddEvent(even);
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case WISE_STATE::ATTACK:
		{
			m_fShootTime = 0;

			switch (m_Attack)
			{
			case WISE_ATTACK::TYPE1:
			{
				PlayAnimToEnd(L"NPC_Wise=stance2_swipeSlashComboA", false, WISE_STATE::MOVE, 1.5f, false);
				m_Move = WISE_MOVE::FAR_READY;
			}
			break;
			case WISE_ATTACK::TYPE2:
			{
				PlayAnimToEnd(L"NPC_Wise=stance2_swipeSlashComboA", false, WISE_STATE::MOVE, 1.5f, false);
				m_Move = WISE_MOVE::NEAR_READY;
			}
			break;
			default:
				break;
			}
		}
		break;
		case WISE_STATE::SHOOT:
		{
			switch (m_Shoot)
			{
			case WISE_SHOOT::CIRCLE:
			{
				PlayAnimToEnd(L"NPC_Wise=stance2_regen", false, WISE_STATE::MOVE);

				m_iPatternRepeat--;
				if (!m_iPatternRepeat)
					m_Move = WISE_MOVE::TELEPORT_CENTER;
			}
			break;
			case WISE_SHOOT::FANWISE:
			{
				//PlayAnimToEnd(L"NPC_Wise=stance2_waveComboA", false, WISE_STATE::MOVE, 1.5f, false);
				PlayAnimToEnd(L"NPC_Wise=stance2_swipeSlashComboA", false, WISE_STATE::MOVE, 1.5f, false);
				m_Move = WISE_MOVE::FAR_READY;
			}
			break;
			case WISE_SHOOT::FLOOR:
			{
				PlayAnimToEnd(L"NPC_Wise=stance2_swipeSlashComboB", false, WISE_STATE::MOVE, 1.5f, false);
				m_Move = WISE_MOVE::NEAR_READY;
			}
			break;
			default:
				break;
			}
		}
		break;
		case WISE_STATE::HIT:
		{
			switch (m_Phase)
			{
			case WISE_PHASE::PHASE_1:
			{
				m_fHP -= 100;
				if (m_fHP <= 0)
				{
					m_fHP = 100.f;

					m_bAnim = true;
					m_fAnimTimeMax = 0.f;

					m_State = WISE_STATE::NONE;
					m_changeState = WISE_STATE::DIE;

					m_iHpColor = 0;
				}
			}
			break;
			case WISE_PHASE::PHASE_2:
			{
				m_fHP -= 10;
				if (m_fHP <= 0)
				{
					m_iHpToken--;
					if (m_iHpToken <= 0)
					{
						m_fHP = 100.f;

						m_bAnim = true;
						m_fAnimTimeMax = 0.f;

						m_State = WISE_STATE::NONE;
						m_changeState = WISE_STATE::DIE;

						m_CameraAnim->ChangeBattle();

						m_bCollision = false;
					}
					else
					{
						m_fHP = 100.f;
						PlayAnimToEnd(L"NPC_Wise=stance2_hitReceived1Light", false, WISE_STATE::MOVE);
						m_Move = WISE_MOVE::NEAR_READY;

						m_CameraAnim->ChangeBattle();

						m_iHpColor = 1;
					}
				}
				else
				{
					if (m_iHpToken == 1)
					{
						PlayAnimToEnd(L"NPC_Wise=stance2_hitReceived1Light", false, WISE_STATE::MOVE);
						m_Move = WISE_MOVE::NEAR_READY;
					}
					else
					{
						PlayAnimToEnd(L"NPC_Wise=stance2_hitReceived1Light", false, WISE_STATE::MOVE);
						m_Move = WISE_MOVE::FAR_READY;
					}
				}
			}
			break;
			case WISE_PHASE::PHASE_3:
			{
				m_bAnim = true;
				m_fAnimTimeMax = 0.f;

				m_State = WISE_STATE::NONE;
				m_changeState = WISE_STATE::IDLE;

				m_iPatternRepeat = 1;
			}
			break;
			case WISE_PHASE::NONE:
				break;
			default:
				break;
			}

			CGameObject* pPar = new CGameObject;
			pPar->SetName(L"Blood");

			pPar->AddComponent(new CTransform);
			pPar->AddComponent(new CParticleSystem);
			pPar->AddComponent(new CEffectScript);

			pPar->Transform()->SetLocalPos(Vec3(m_vPos.x, 100.f, m_vPos.z));

			pPar->ParticleSystem()->SetfMinLifeTime(0.5f);
			pPar->ParticleSystem()->SetfMaxLifeTime(0.5f);
			pPar->ParticleSystem()->SetfFrequency(0.01f);
			pPar->ParticleSystem()->SetvCreateRange(Vec4(100.f, 100.f, 100.f, 1.f));
			pPar->ParticleSystem()->SetvStartColor(Vec4(0.59f, 0.3f, 0.f, 1.f));
			pPar->ParticleSystem()->SetvEndColor(Vec4(0.59f, 0.3f, 0.f, 0.3f));
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
		break;
		case WISE_STATE::KNOCKDOWN:
		{
			switch (m_Phase)
			{
			case WISE_PHASE::PHASE_1:
				break;
			case WISE_PHASE::PHASE_2:
			{
				m_fHP--;
				if (m_fHP <= 0)
				{
					m_iHpToken--;
					if (m_iHpToken <= 0)
					{
						m_fHP = 2.f;

						m_bAnim = true;
						m_fAnimTimeMax = 0.f;

						m_State = WISE_STATE::NONE;
						m_changeState = WISE_STATE::DIE;
					}
					else
					{
						m_fHP = 2.f;
						PlayAnimToEnd(L"NPC_Wise=stance2_hitReceived1Light", false, WISE_STATE::MOVE);
						m_Move = WISE_MOVE::NEAR_READY;
					}
				}
				else
				{
					if (m_iHpToken == 1)
					{
						PlayAnimToEnd(L"NPC_Wise=stance2_hitKnockdown2Medium", false, WISE_STATE::MOVE);
						m_Move = WISE_MOVE::NEAR_READY;
					}
					else
					{
						PlayAnimToEnd(L"NPC_Wise=stance2_hitKnockdown2Medium", false, WISE_STATE::MOVE);
						m_Move = WISE_MOVE::FAR_READY;
					}
				}
			}
			break;
			case WISE_PHASE::PHASE_3:
				break;
			default:
				break;
			}

			CGameObject* pPar = new CGameObject;
			pPar->SetName(L"Blood");

			pPar->AddComponent(new CTransform);
			pPar->AddComponent(new CParticleSystem);
			pPar->AddComponent(new CEffectScript);

			pPar->Transform()->SetLocalPos(Vec3(m_vPos.x, 100.f, m_vPos.z));

			pPar->ParticleSystem()->SetfMinLifeTime(0.5f);
			pPar->ParticleSystem()->SetfMaxLifeTime(0.5f);
			pPar->ParticleSystem()->SetfFrequency(0.01f);
			pPar->ParticleSystem()->SetvCreateRange(Vec4(100.f, 100.f, 100.f, 1.f));
			pPar->ParticleSystem()->SetvStartColor(Vec4(0.59f, 0.3f, 0.f, 1.f));
			pPar->ParticleSystem()->SetvEndColor(Vec4(0.59f, 0.3f, 0.f, 0.3f));
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
		break;
		case WISE_STATE::DIE:
		{
			m_bCollision = false;

			m_iLifeCount--;

			switch (m_Phase)
			{
			case WISE_PHASE::PHASE_1:
			{
				m_CameraAnim->MonsterZoomin(800.f);

				if (GetObj()->GetChild()[1])
				{
					CGameObject* pObj = GetObj()->GetChild()[1];
					for (int i = 0; i < pObj->GetChild().size(); ++i)
					{
						DeleteObject(pObj->GetChild()[i]);
					}

					pObj = GetObj()->GetChild()[2];
					for (int i = 0; i < pObj->GetChild().size(); ++i)
					{
						DeleteObject(pObj->GetChild()[i]);
					}
				}

				PlayAnimToEnd(L"NPC_Wise=stance1_changePhaseTo2", false, WISE_STATE::MOVE, 2.f, true, true);
				m_Move = WISE_MOVE::TELEPORT;
				m_Phase = WISE_PHASE::PHASE_2;
			}
			break;
			case WISE_PHASE::PHASE_2:
			{
				//m_CameraAnim->MonsterZoomin(800.f);

				PlayAnimToEnd(L"NPC_Wise=stance2_changePhase", false, WISE_STATE::MOVE, 0.f, true, true);
				m_Move = WISE_MOVE::TELEPORT_CENTER;
				m_Phase = WISE_PHASE::PHASE_3;
			}
			break;
			case WISE_PHASE::PHASE_4:
			case WISE_PHASE::PHASE_3:
			{
				m_AnimObj->Animator3D()->ChangeClip(L"NPC_Wise=stance2_hitKnockdown2Medium", false);

				m_State = WISE_STATE::NONE;
			}
			break;
			case WISE_PHASE::NONE:
				break;
			default:
				break;
			}
		}
		break;
		case WISE_STATE::SPECIAL:
		{
			switch (m_Special)
			{
			case WISE_SPECIAL::FIRST:
			{
				m_bCollision = false;
				PlayAnimToEnd(L"NPC_Wise=stance2_JumpOnPilar", false, WISE_STATE::IDLE);

				CGameObject* pPillar = m_Pillar->Instantiate();
				pPillar->SetName(L"Pillar");
				pPillar->SetDynamicShadow(true);

				pPillar->AddComponent(new CCollider2D);
				pPillar->AddComponent(new CPillarScript);

				pPillar->Transform()->SetLocalPos(Vec3(0.f, -330.f, 0.f));
				pPillar->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pPillar->Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, 0.f, 0.f));

				pPillar->Collider2D()->SetvOffsetScale(Vec2(200.f, 200.f));
				pPillar->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

				tEvent even = {};
				even.eEvent = EVENT_TYPE::CREATE_OBJECT;
				even.lParam = (DWORD_PTR)pPillar;
				even.wParam = (DWORD_PTR)2;
				CEventMgr::GetInst()->AddEvent(even);

				m_Special = WISE_SPECIAL::SECOND;
			}
			break;
			case WISE_SPECIAL::SECOND:
			{
				m_bCollision = false;
				m_vPos.y = 0.f;
				PlayAnimToEnd(L"NPC_Wise=stance2_JumpOffPilar", false, WISE_STATE::MOVE);
				m_Move = WISE_MOVE::LEFT;
				m_iPatternRepeat = 4;

				m_Special = WISE_SPECIAL::THIRD;
			}
			break;
			case WISE_SPECIAL::THIRD:
				break;
			case WISE_SPECIAL::FOURTH:
			{
				m_bCollision = false;
				PlayAnimToEnd(L"NPC_Wise=stance2_fury1Start", false, WISE_STATE::IDLE);

				m_Special = WISE_SPECIAL::FIFTH;
			}
			break;
			case WISE_SPECIAL::FIFTH:
			{
				m_bCollision = false;
				PlayAnimToEnd(L"NPC_Wise=stance2_fury3Start", false, WISE_STATE::IDLE);

				for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 6))
				{
					CGameObject* pPillar = m_Pillar->Instantiate();
					pPillar->SetName(L"Pillar");
					pPillar->SetDynamicShadow(true);

					pPillar->AddComponent(new CCollider2D);
					pPillar->AddComponent(new CPillarScript);

					Vec2 vDir;
					vDir.x = cos(Angle) * 1000.f;
					vDir.y = sin(Angle) * 1000.f;

					pPillar->Transform()->SetLocalPos(Vec3(vDir.x, -330.f, vDir.y));
					pPillar->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
					pPillar->Transform()->SetLocalRot(Vec3(-XM_PI / 2.f, 0.f, 0.f));

					pPillar->Collider2D()->SetvOffsetScale(Vec2(200.f, 200.f));
					pPillar->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);


					CPillarScript* pScript = (CPillarScript*)pPillar->GetScript(L"CPillarScript");
					pScript->SetType(false);

					tEvent even = {};
					even.eEvent = EVENT_TYPE::CREATE_OBJECT;
					even.lParam = (DWORD_PTR)pPillar;
					even.wParam = (DWORD_PTR)2;
					CEventMgr::GetInst()->AddEvent(even);
				}

				m_Phase = WISE_PHASE::PHASE_4;
				m_iPatternRepeat = 0;

				m_Special = WISE_SPECIAL::NONE;
			}
			break;
			case WISE_SPECIAL::NONE:
			{

			}
			break;
			default:
				break;
			}
		}
		default:
			break;
		}
	}
}

void CWiseScript::LookAt(Vec3 _TargetPos)
{
	Vec2 vChangeVec;
	vChangeVec.x = _TargetPos.x - m_vPos.x;
	vChangeVec.y = _TargetPos.z - m_vPos.z;
	vChangeVec.Normalize();

	float m_angle = atan2f(vChangeVec.x, vChangeVec.y);

	Transform()->SetLocalRot(Vec3(0, m_angle, 0));
}

void CWiseScript::Idle()
{
	switch (m_Phase)
	{
	case WISE_PHASE::PHASE_1:
	{
		m_fShootTimeMax = 10.f;
		m_fShootTime += fDT;

		if (m_fShootTime >= m_fShootTimeMax)
		{
			m_fShootTime = 0.f;

			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"WiseWave");
			pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

			CGameObject* pBullet = m_Fanwise->Instantiate();
			pBullet->SetName(L"Sector");
			pBullet->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pBullet->Transform()->SetLocalRot(Vec3(0.f, Transform()->GetLocalRot().y, 0.f));

			CMissileScript3* pScript = (CMissileScript3*)pBullet->GetScript(L"CMissileScript3");
			pScript->SetSpeed(3000.f);
			pScript->SetSectorInfo(XM_PI / 6.f, 0.f, 0.03f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)4;
			CEventMgr::GetInst()->AddEvent(even);
		}
	}
	break;
	case WISE_PHASE::PHASE_2:
	{
		if (m_iPatternRepeat)
		{
			m_State = WISE_STATE::SPECIAL;
			return;
		}

		m_fShootTimeMax = 10.f;
		m_fShootTime += fDT;

		if (m_bPhaseShoot)
			m_bPhaseShoot = false;
		else
			m_bPhaseShoot = true;

		if (m_fShootTime >= m_fShootTimeMax)
		{
			m_fShootTime = 0.f;

			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"WiseWave");
			pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

			int idx = 0;
			for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 4))
			{
				CGameObject* pBullet = m_Fanwise->Instantiate();
				pBullet->SetName(L"Sector");
				pBullet->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

				if (m_bPhaseShoot)
					pBullet->Transform()->SetLocalRot(Vec3(0.f, Angle + XM_PI / 4, 0.f));
				else
					pBullet->Transform()->SetLocalRot(Vec3(0.f, Angle, 0.f));

				CMissileScript3* pScript = (CMissileScript3*)pBullet->GetScript(L"CMissileScript3");
				pScript->SetSpeed(3000.f);
				pScript->SetSectorInfo(XM_PI / 6.f, 0.f, 0.03f);
				idx++;

				tEvent even = {};
				even.eEvent = EVENT_TYPE::CREATE_OBJECT;
				even.lParam = (DWORD_PTR)pBullet;
				even.wParam = (DWORD_PTR)4;
				CEventMgr::GetInst()->AddEvent(even);
			}
		}
	}
	break;
	case WISE_PHASE::PHASE_3:
	{
		m_bCollision = false;

		if (m_iPatternRepeat)
		{
			m_State = WISE_STATE::SPECIAL;
			return;
		}

		m_fShootTimeMax = 0.1f;
		m_fShootTime += fDT;

		m_fShootTimeMax2 = 5.f;
		m_fShootTime2 += fDT;

		m_fEndTimeMax = 10.f;
		m_fEndTime += fDT;

		if (m_fShootTime >= m_fShootTimeMax)
		{
			m_fShootTime = 0.f;

			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"WiseBullet1");
			pSound->Play(1, 0.5f, true); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

			CGameObject* pBullet = m_Yellow->Instantiate();
			pBullet->SetName(L"Bullet");

			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int> MoveX(-1000, 1000);
			std::uniform_int_distribution<int> MoveZ(-1000, 1000);

			pBullet->Transform()->SetLocalPos(Vec3(MoveX(gen), 0.f, MoveZ(gen)));

			CMissileScript* BulletS = (CMissileScript*)pBullet->GetScript(L"CMissileScript");

			Vec3 vPlayerPos = m_pPlayer->Transform()->GetLocalPos();
			Vec3 vPos = pBullet->Transform()->GetLocalPos();

			Vec2 vTarget = Vec2(vPlayerPos.x - vPos.x, vPlayerPos.z - vPos.z);
			vTarget.Normalize();
			BulletS->SetDir(Vec3(vTarget.x, 0.f, vTarget.y));
			BulletS->SetSpeed(600.f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)4;
			CEventMgr::GetInst()->AddEvent(even);
		}

		if (m_fShootTime2 >= m_fShootTimeMax2)
		{
			m_fShootTime2 = 0.f;

			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"WiseWave");
			pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

			CGameObject* pBullet = m_Fanwise->Instantiate();
			pBullet->SetName(L"Sector");
			pBullet->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pBullet->Transform()->SetLocalRot(Vec3(0.f, Transform()->GetLocalRot().y, 0.f));

			CMissileScript3* pScript = (CMissileScript3*)pBullet->GetScript(L"CMissileScript3");
			pScript->SetSpeed(3000.f);
			pScript->SetSectorInfo(XM_2PI, 0.2f, 0.f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)4;
			CEventMgr::GetInst()->AddEvent(even);
		}

		if (m_fEndTime >= m_fEndTimeMax)
		{
			m_fEndTime = 0;

			m_bAnim = true;
			m_fAnimTimeMax = 0.f;

			m_State = WISE_STATE::NONE;
			m_changeState = WISE_STATE::SPECIAL;
		}
	}
	break;
	case WISE_PHASE::PHASE_4:
	{

	}
	break;
	default:
		break;
	}
}

void CWiseScript::Move()
{
	switch (m_Move)
	{
	case WISE_MOVE::LEFT:
	{
		if (m_bMoveCheck)
		{
			Vec3 vPlayerPos = m_pPlayer->Transform()->GetLocalPos();
			Vec3 vPlayerRot = m_pPlayer->Transform()->GetLocalRot();
			m_vDir = Vec2(vPlayerPos.x - m_vPos.x, vPlayerPos.z - m_vPos.z);
			m_vDir.Normalize();

			Vec2 vDir;
			vDir.x = cos(vPlayerRot.y);
			vDir.y = sin(vPlayerRot.y);
			vDir.Normalize();

			m_vDir += vDir;
			m_vDir.Normalize();

			m_bMoveCheck = false;

			for (int i = 0; i < 3; i++)
			{
				CGameObject* pBullet = m_Spread->Instantiate();
				pBullet->SetName(L"Bullet2");

				std::random_device rd;		// 시드값을 얻기 위한 random_device 생성.
				std::mt19937 gen(rd());		// random_device 를 통해 난수 생성 엔진을 초기화 한다.
				std::uniform_int_distribution<int> MoveX(-1000, 1000);		// 1 부터 6 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
				std::uniform_int_distribution<int> MoveZ(-1000, 1000);

				pBullet->Transform()->SetLocalPos(Vec3(MoveX(gen), 0.f, MoveZ(gen)));

				tEvent even = {};
				even.eEvent = EVENT_TYPE::CREATE_OBJECT;
				even.lParam = (DWORD_PTR)pBullet;
				even.wParam = (DWORD_PTR)4;
				CEventMgr::GetInst()->AddEvent(even);
			}
		}

		m_vPos.x += m_vDir.x * fDT * 20000.f;
		m_vPos.z += m_vDir.y * fDT * 20000.f;
	}
	break;
	case WISE_MOVE::FRONT:
	{
		Vec3 vPlayerPos = m_pPlayer->Transform()->GetLocalPos();
		m_vDir = Vec2(vPlayerPos.x - m_vPos.x, vPlayerPos.z - m_vPos.z);

		if ((m_vDir.x * m_vDir.x + m_vDir.y * m_vDir.y) > 30000.f)
		{
			m_iPatternRepeat = 2;
			m_vDir.Normalize();
			m_vPos.x += m_vDir.x * fDT * 1000.f;
			m_vPos.z += m_vDir.y * fDT * 1000.f;	// 80000
		}
		else
		{
			m_iPatternRepeat = 1;

			m_bAnim = true;
			m_prevState = WISE_STATE::NONE;
			m_State = WISE_STATE::MOVE;
		}
	}
	break;
	case WISE_MOVE::BACK:
	{
		if (m_bMoveCheck)
		{
			Vec3 vPlayerPos = m_pPlayer->Transform()->GetLocalPos();
			m_vDir = Vec2(m_vPos.x - vPlayerPos.x, m_vPos.z - vPlayerPos.z);
			m_vDir.Normalize();

			m_bMoveCheck = false;
		}

		m_vPos.x += m_vDir.x * fDT * 1200.f;
		m_vPos.z += m_vDir.y * fDT * 1200.f;
	}
	break;
	case WISE_MOVE::TELEPORT:
	{
		if (m_AnimPhase == WISE_ANIMPHASE::SECOND)
		{
			std::random_device rd;		// 시드값을 얻기 위한 random_device 생성.
			std::mt19937 gen(rd());		// random_device 를 통해 난수 생성 엔진을 초기화 한다.
			std::uniform_int_distribution<int> MoveX(-1000, 1000);		// 1 부터 6 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
			std::uniform_int_distribution<int> MoveZ(-1000, 1000);

			m_vPos = Vec3((float)MoveX(gen), 0.f, (float)MoveZ(gen));
		}
	}
	break;
	case WISE_MOVE::TELEPORT_CENTER:
	{
		if (m_AnimPhase == WISE_ANIMPHASE::SECOND)
			m_vPos = Vec3(0.f, 0.f, 0.f);
	}
	break;
	case WISE_MOVE::TELEPORT_OUT:
	{
		m_vPos = Vec3(10000.f, 10000.f, 10000.f);
	}
	break;
	case WISE_MOVE::FAR_READY:
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> Pattern(0, 2);

		switch (Pattern(gen))
		{
		case 0:	// 재공격
		{
			m_bAnim = true;
			m_fAnimTimeMax = 0.f;

			m_State = WISE_STATE::NONE;
			m_changeState = WISE_STATE::MOVE;

			m_Move = WISE_MOVE::FRONT;
		}
		break;
		case 1:	// 이동 후 부채꼴
		{
			Vec3 vPlayerPos = m_pPlayer->Transform()->GetLocalPos();
			m_vDir = Vec2(vPlayerPos.x - m_vPos.x, vPlayerPos.z - m_vPos.z);

			if ((m_vDir.x * m_vDir.x + m_vDir.y * m_vDir.y) < 80000.f)
			{
				m_bAnim = true;
				m_fAnimTimeMax = 0.f;

				m_State = WISE_STATE::NONE;
				m_changeState = WISE_STATE::MOVE;

				m_Move = WISE_MOVE::BACK;
				m_iPatternRepeat = 1;
			}
			else
			{
				m_bAnim = true;
				m_fAnimTimeMax = 0.f;

				m_State = WISE_STATE::NONE;
				m_changeState = WISE_STATE::SHOOT;

				m_Shoot = WISE_SHOOT::FANWISE;
			}
		}
		break;
		case 2:	// 텔레포트 후 공격
		{
			m_bAnim = true;
			m_fAnimTimeMax = 0.f;

			m_State = WISE_STATE::NONE;
			m_changeState = WISE_STATE::MOVE;

			m_Move = WISE_MOVE::TELEPORT_OUT;
		}
		break;
		default:
			break;
		}
	}
	break;
	case WISE_MOVE::NEAR_READY:
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> Pattern(0, 1);

		switch (Pattern(gen))
		{
		case 0:	// 근접공격
		{
			m_bAnim = true;
			m_fAnimTimeMax = 0.f;

			m_State = WISE_STATE::NONE;
			m_changeState = WISE_STATE::MOVE;

			m_Move = WISE_MOVE::FRONT;
		}
		break;
		case 1:	// 장판
		{
			m_bAnim = true;
			m_fAnimTimeMax = 0.f;

			m_State = WISE_STATE::NONE;
			m_changeState = WISE_STATE::SHOOT;

			m_Shoot = WISE_SHOOT::FLOOR;

			m_iFloorNum = 0;
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
}

void CWiseScript::Attack()
{
	switch (m_Attack)
	{
	case WISE_ATTACK::TYPE1:
	case WISE_ATTACK::TYPE2:
	{
		m_fShootTimeMax = 1.5f;

		m_fShootTime += fDT;
		if (m_fShootTime >= m_fShootTimeMax)
		{
			m_fShootTime = 0;

			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"WiseAttack");
			pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

			CGameObject* pBullet = m_Slash->Instantiate();
			pBullet->SetName(L"Slash");

			CMissileScript* BulletS = (CMissileScript*)pBullet->GetScript(L"CMissileScript");
			Vec3 vPlayerPos = m_pPlayer->Transform()->GetLocalPos();

			Vec2 vDir = Vec2(vPlayerPos.x - m_vPos.x, vPlayerPos.z - m_vPos.z);
			vDir.Normalize();
			pBullet->Transform()->SetLocalPos(Vec3(m_vPos.x + vDir.x * 200.f, 0.f, m_vPos.z + vDir.y * 200.f));

			BulletS->SetScale(Vec3(350.f, 0.f, 350.f));
			BulletS->SetTime(0.1f);
			BulletS->SetBullet(false);
			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)2;
			CEventMgr::GetInst()->AddEvent(even);
		}
	}
	break;
	default:
		break;
	}
}

void CWiseScript::Shoot()
{
	switch (m_Shoot)
	{
	case WISE_SHOOT::CIRCLE:
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"WiseBullet");
		pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

		for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 12))
		{
			CGameObject* pBullet = m_Yellow->Instantiate();
			pBullet->SetName(L"Bullet");

			Vec2 vDir;
			vDir.x = cos(Angle) * 400.f;
			vDir.y = sin(Angle) * 400.f;

			CMissileScript* BulletS = (CMissileScript*)pBullet->GetScript(L"CMissileScript");
			Vec3 vPlayerPos = m_pPlayer->Transform()->GetLocalPos();

			pBullet->Transform()->SetLocalPos(Vec3(vPlayerPos.x + vDir.x, 0.f, vPlayerPos.z + vDir.y));

			Vec3 vPos = pBullet->Transform()->GetLocalPos();
			Vec2 vTarget = Vec2(vPlayerPos.x - vPos.x, vPlayerPos.z - vPos.z);
			vTarget.Normalize();
			BulletS->SetDir(Vec3(vTarget.x, 0.f, vTarget.y));
			BulletS->SetSpeed(600.f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)4;
			CEventMgr::GetInst()->AddEvent(even);
		}
	}
	break;
	case WISE_SHOOT::FANWISE:
	{
		m_fShootTimeMax = 1.5f;
		m_fShootTime += fDT;

		if (m_fShootTime >= m_fShootTimeMax)
		{
			m_fShootTime = 0.f;

			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"WiseWave");
			pSound->Play(1); //0을 넣으면 무한반복 ,중복재생 하고싶으면 true (기본값은 false, 중복재생 막아놓음)

			CGameObject* pBullet = m_Fanwise->Instantiate();
			pBullet->SetName(L"Sector");
			pBullet->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pBullet->Transform()->SetLocalRot(Vec3(0.f, Transform()->GetLocalRot().y, 0.f));

			CMissileScript3* pScript = (CMissileScript3*)pBullet->GetScript(L"CMissileScript3");
			pScript->SetPos(Vec3(m_vPos.x, 0.f, m_vPos.z));
			pScript->SetSpeed(1000.f);
			pScript->SetSectorInfo(XM_PI / 12.f, 0.05f, 0.03f);

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)4;
			CEventMgr::GetInst()->AddEvent(even);
		}
	}
	break;
	case WISE_SHOOT::FLOOR:
	{
		m_fShootTimeMax = 2.0f;
		m_fShootTime += fDT;

		if (m_iFloorNum == 3)
			m_iFloorNum = 0;

		if (m_fShootTime >= m_fShootTimeMax)
		{
			m_fShootTime = 0.f;

			CGameObject* pBullet = m_Fanwise->Instantiate();
			pBullet->SetName(L"Sector");
			pBullet->Transform()->SetLocalScale(Vec3(2000.f, 2000.f, 2000.f));

			CMissileScript3* pScript = (CMissileScript3*)pBullet->GetScript(L"CMissileScript3");
			pScript->SetPos(Vec3(m_vPos.x, 0.f, m_vPos.z));
			pScript->SetSpeed(0.f);
			pScript->SetTime(1.f);

			switch (m_iFloorNum)
			{
			case 0:
			{
				pBullet->Transform()->SetLocalRot(Vec3(0.f, Transform()->GetLocalRot().y + XM_2PI / 3.f, 0.f));
				pScript->SetSectorInfo(XM_PI / 2.f, 0.f, 0.f);
			}
			break;
			case 1:
			{
				pBullet->Transform()->SetLocalRot(Vec3(0.f, Transform()->GetLocalRot().y, 0.f));
				pScript->SetSectorInfo(XM_PI / 8.f, 0.f, 0.f);
			}
			break;
			case 2:
			{
				pBullet->Transform()->SetLocalRot(Vec3(0.f, Transform()->GetLocalRot().y + XM_PI / 4.f, 0.f));
				pScript->SetSectorInfo(XM_PI, 0.f, 0.f);
			}
			break;
			default:
				break;
			}

			m_iFloorNum++;

			tEvent even = {};
			even.eEvent = EVENT_TYPE::CREATE_OBJECT;
			even.lParam = (DWORD_PTR)pBullet;
			even.wParam = (DWORD_PTR)4;
			CEventMgr::GetInst()->AddEvent(even);
		}
	}
	break;
	default:
		break;
	}
}

void CWiseScript::Stun()
{
}

void CWiseScript::Hit()
{

}

void CWiseScript::Knockdown()
{
}

void CWiseScript::Die()
{
	switch (m_Phase)
	{
	case WISE_PHASE::PHASE_1:
	{
		// 처음에 HIT에서 바로 DIE로 바꿔줘서 여길 못들어와서 HIT에서 해줬음
		//m_fHP = 10.f;
		//m_Phase = WISE_PHASE::PHASE_2;
	}
	break;
	default:
		break;
	}
}

void CWiseScript::Special()
{
	if (m_Special == WISE_SPECIAL::THIRD)
	{
		m_fShootTimeMax = 3.5f;
		m_fShootTime += fDT;

		if (m_fShootTime >= m_fShootTimeMax)
		{
			m_fShootTime = 0.f;

			m_State = WISE_STATE::MOVE;
			m_Move = WISE_MOVE::TELEPORT_IN;

			m_Special = WISE_SPECIAL::FOURTH;
		}
	}
}

void CWiseScript::ForRed()
{
	if (m_bRed)
	{
		m_RedTime += fDT;
		if (m_RedTime >= m_RedTimeMax)
		{
			m_RedTime = 0;
			m_bRed = false;

			int a = 0;
			m_AnimObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::INT_3, &a);
		}
	}

	if (m_i3PhaseDead == -1)
	{
		m_bAnim = true;
		m_fAnimTimeMax = 0.f;

		m_State = WISE_STATE::NONE;
		m_changeState = WISE_STATE::DIE;

		m_i3PhaseDead--;
	}
}

void CWiseScript::ChangeState()
{
	if (m_bAnim)
	{
		m_fAnimTime += fDT;
		if (m_fAnimTime > m_fAnimTimeMax)
		{
			m_fAnimTime = 0.f;
			m_bAnim = false;

			m_State = m_changeState;
			m_bCollision = true;

			if (m_bCameraZoom)
			{
				m_CameraAnim->ZoomOut();
				m_bCameraZoom = false;
			}
		}
	}
}

void CWiseScript::PlayAnimToEnd(const wstring& _strName, bool _Repeat, WISE_STATE _State, float _AddTime, bool _bNone, bool _bCamera)
{
	int iAnimIdx = m_AnimObj->Animator3D()->ChangeClip(_strName, _Repeat);
	m_fAnimTimeMax = m_vecAnimClip->at(iAnimIdx).dEndTime;
	m_fAnimTimeMax += _AddTime;
	m_bAnim = true;

	if (_bNone)
		m_State = WISE_STATE::NONE;

	if (_bCamera)
		m_bCameraZoom = true;

	m_changeState = _State;
}

void CWiseScript::HitMoment()
{
	if (m_bCollision)
	{
		if (!m_bRed)
		{
			m_bRed = true;

			int a = 1;
			m_AnimObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::INT_3, &a);
		}

		m_bAnim = true;
		m_fAnimTimeMax = 0.f;

		m_State = WISE_STATE::NONE;
		m_changeState = WISE_STATE::HIT;
	}
}

void CWiseScript::KnockdownMoment()
{
	if (m_bCollision)
	{
		m_bAnim = true;
		m_fAnimTimeMax = 0.f;

		m_State = WISE_STATE::NONE;
		m_changeState = WISE_STATE::KNOCKDOWN;
	}
}

void CWiseScript::OnCollisionEnter(CGameObject* _pOther)
{
}
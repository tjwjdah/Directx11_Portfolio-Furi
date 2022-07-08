#include "pch.h"
#include "CSecneChangeScript.h"

#include <Engine\CGameObject.h>
#include <Engine\CSceneMgr.h>
#include <Engine\CScene.h>
#include "CPlayerScript.h"
#include "CHpbarScript.h"
#include "CCameraAnimaitorScript.h"
CSecneChangeScript::CSecneChangeScript():CScript((int)SCRIPT_TYPE::SECNECHANGESCRIPT)
,m_iTest(0)
,m_bTest(false)
{
}

CSecneChangeScript::~CSecneChangeScript()
{
}



void CSecneChangeScript::update()
{
	if (!m_bTest)
	{
		VObjUpRenOnOff(ObjList::Change, false);
		VObjUpRenOnOff(ObjList::Hpbar, false);
		VObjUpRenOnOff(ObjList::Law, false);
		VObjUpRenOnOff(ObjList::Wise, false);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"main");
		pSound->Play(0, 0.1f);
		m_bTest = true;

	}
	/*if (KEY_TAP(KEY_TYPE::NUM_3))
	{
		ChangeObjList((ObjList)m_iTest);
		m_iTest++;
	}*/
}

void CSecneChangeScript::ChangeObjPushObj(CGameObject* _Obj, ObjList _List)
{
	switch (_List)
	{
	case ObjList::Start:
	{
	
		m_vStartObj.push_back(_Obj);
	}
		break;
	case ObjList::Law:
	{
		
		m_vLawObj.push_back(_Obj);
	}
		break;
	case ObjList::Change:
	{
		
		m_vChangeObj.push_back(_Obj);
	}
		break;
	case ObjList::Wise:
	{
		
		m_vWiseObj.push_back(_Obj);
	}
		break;
	case ObjList::Hpbar:
	{
		
		m_vHpbarObj.push_back(_Obj);
	}
		break;
	}
}

void CSecneChangeScript::ChangeObjList(ObjList _List)
{
	switch (_List)
	{
	case ObjList::Start:
	{
		
		VObjUpRenOnOff(ObjList::Start, true);
	}
		break;
	case ObjList::Law:
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"main");
		pSound->Stop();
		pSound = CResMgr::GetInst()->FindRes<CSound>(L"Law");
		pSound->Play(0, 0.1f);
		m_Player->SetTarGetObj(m_Law);
		m_Player->SetAttackMoveStop(false);
		m_HpBar->SetBossName(BOSS::LAW);
		VObjUpRenOnOff(ObjList::Start, false);
		VObjUpRenOnOff(ObjList::Law, true);
		VObjUpRenOnOff(ObjList::Hpbar, true);
		
	}
		break;
	case ObjList::Change:
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"Law");
		pSound->Stop();
		pSound = CResMgr::GetInst()->FindRes<CSound>(L"father");
		pSound->Play(0, 0.1f);
		VObjUpRenOnOff(ObjList::Law, false);
		VObjUpRenOnOff(ObjList::Hpbar, false);
		VObjUpRenOnOff(ObjList::Change, true);
		
	}
		break;
	case ObjList::Wise:
	{	
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"father");
		pSound->Stop();
		pSound = CResMgr::GetInst()->FindRes<CSound>(L"wise");
		pSound->Play(0, 0.1f);
		m_HpBar->SetBossName(BOSS::WISE);
		m_Player->SetTarGetObj(m_Wise);
		m_Player->GetObj()->Transform()->SetLocalPos(Vec3(0.f, 0.f, -1400.f));
		CGameObject* CObj=CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Camera_Root");
		CObj->Transform()->SetLocalPos(Vec3(0.f, 2200.f, -1200.f));
		CObj = CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Directional Light");
		CObj->Transform()->SetLocalRot(Vec3(0.f, 1.f, 0.f));
		CCameraAnimaitorScript* CameraAnim = (CCameraAnimaitorScript*)CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Camera_bn")->GetScript(L"CCameraAnimaitorScript");
		CameraAnim->SetTargerMonster(m_Wise);
		VObjUpRenOnOff(ObjList::Change, false);
		VObjUpRenOnOff(ObjList::Wise, true);
		VObjUpRenOnOff(ObjList::Hpbar, true);
	}
		break;

	}
}

void CSecneChangeScript::VObjUpRenOnOff(ObjList _List,bool _OnOff)
{
	switch (_List)
	{
	case ObjList::Start:
	{
		for (UINT i = 0; i < m_vStartObj.size(); i++)
			m_vStartObj[i]->SetbUpdateRenderOn(_OnOff);
	}
	break;
	case ObjList::Law:
	{
		for (UINT i = 0; i < m_vLawObj.size(); i++)
			m_vLawObj[i]->SetbUpdateRenderOn(_OnOff);
	}
	break;
	case ObjList::Change:
	{
		for (UINT i = 0; i < m_vChangeObj.size(); i++)
			m_vChangeObj[i]->SetbUpdateRenderOn(_OnOff);
	}
	break;
	case ObjList::Wise:
	{
		for (UINT i = 0; i < m_vWiseObj.size(); i++)
			m_vWiseObj[i]->SetbUpdateRenderOn(_OnOff);
	}
	break;
	case ObjList::Hpbar:
	{
		for (UINT i = 0; i < m_vHpbarObj.size(); i++)
			m_vHpbarObj[i]->SetbUpdateRenderOn(_OnOff);
	}
	break;
	}
}

void CSecneChangeScript::SetPlayer(CGameObject* Obj)
{
	m_Player = (CPlayerScript*)Obj->GetScript(L"CPlayerScript");
}

void CSecneChangeScript::SetHpbar(CGameObject* Obj)
{
	m_HpBar = (CHpbarScript*)Obj->GetScript(L"CHpbarScript");
}


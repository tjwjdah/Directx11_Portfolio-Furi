#include "pch.h"
#include "CCameraAnimaitorScript.h"

#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>
#include <Engine/CSceneMgr.h>
#include <Engine\CScene.h>
#include "CPlayerScript.h"
#include "CLawScript.h"
CCameraAnimaitorScript::CCameraAnimaitorScript() :CScript((int)SCRIPT_TYPE::CAMERAANIMAITORSCRIPT)
, m_bnCamera(false)
, m_CupClipData(nullptr)
, m_bnChild(nullptr)
, m_bFinish(true)
, m_dCurTime(0.f)
, m_iFrameCount(30)
, m_iFrameIdx(0)
, m_iFramespeed(1)
, m_RecodeNum(0)
, m_RecodeFrm(60)
, m_TargetPlayerObj(nullptr)
, m_ZoomSpeed(0.f)
, m_ChargeAttackAnimTime(0.f)
, m_PlayingType(CameraPlayType::NormalBattle)
, m_ZoomDir(0.f, 0.f, 0.f)
, m_TargetMonsterObj(nullptr)
, m_iFrameEndIdx(0)
, m_ZoomXCheck(false)
, m_ZoomYCheck(false)
, m_ZoomZCheck(false)
, m_ZoomYDistance(0.f)
, m_Distance(0.f)
, m_PlayerPos(Vec2(0.f,0.f))
, m_TargetPos(Vec2(0.f,0.f))
, m_PrePlayeringType(CameraPlayType::NormalBattle)
, m_HitTime(0.f)
, m_HitOn(false)
{

}

CCameraAnimaitorScript::~CCameraAnimaitorScript()
{
}

void CCameraAnimaitorScript::awake()
{
	
	m_TargetMonsterObj = CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Law");
	if (m_TargetMonsterObj == nullptr)
		m_TargetMonsterObj = CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Wise");
}

void CCameraAnimaitorScript::update()
{
	/*
	if (KEY_TAP(KEY_TYPE::NUM_0))
	{
		m_bFinish = true;
	}
	if (KEY_TAP(KEY_TYPE::NUM_1))
	{
		m_bFinish = false;
	}
	if (KEY_TAP(KEY_TYPE::NUM_2))
	{
		ChangeBattle();
	}
	*/
	if (m_PlayingType == CameraPlayType::Recoding)
		int a = 0;
	switch (m_PlayingType)
	{
	case CameraPlayType::NormalBattle:
		NormalBattle();
		break;
	case CameraPlayType::CloseBattle:
		CloseBattle();
		break;
	case CameraPlayType::AnimPlay:
		ClipShow();
		break;
	case CameraPlayType::Recoding:
		Recoding();
		break;
	case CameraPlayType::ChargeAttackAnin:
	{
		ClipShow();
		m_ChargeAttackAnimTime += fDT;
		if (m_ChargeAttackAnimTime > 2.f)
		{
			m_PlayingType = CameraPlayType::NormalBattle;
			m_PrePlayeringType = CameraPlayType::NormalBattle;
			m_ChargeAttackAnimTime = 0.f;
			Transform()->SetLocalRot(m_PreTransform.bnC_Rotation);
			Transform()->SetLocalPos(m_PreTransform.bnC_Translation);
			Transform()->SetLocalScale(m_PreTransform.bnC_Scali);
			
			if (m_bnCamera)
			{

				GetGameObject()->GetParent()->Transform()->SetLocalRot(m_PreRootTransform.bnC_Rotation);
				GetGameObject()->GetParent()->Transform()->SetLocalPos(m_PreRootTransform.bnC_Translation);
			}
		}
	}
		break;
	case CameraPlayType::Zoomin:
		Zoomin();
		break;
	case CameraPlayType::ZoomOut:
		ZoomOut();
		break;
	case CameraPlayType::None:
		break;
	}
	if (m_HitOn)
	{
		HiT();
	}
	else
	{
		m_HitPrePos = GetGameObject()->GetParent()->Transform()->GetLocalPos();
	}
}

void CCameraAnimaitorScript::LoadClipData(const wstring& _strFilePath)
{
	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += _strFilePath;
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFullPath.c_str(), L"rb");
	UINT DataSIze=0;
	fread(&DataSIze, sizeof(UINT), 1, pFile);
	//m_ClipData.resize(DataSIze);
	for (UINT i = 0; i < DataSIze; i++)
	{
		CameraData CData;
		bool bnCamera;
		fread(&bnCamera, sizeof(bool), 1, pFile);
		LoadWString(CData.CameraClipName, pFile);
		UINT TranslationSize;
		fread(&TranslationSize, sizeof(int), 1, pFile);
			CData.CameraTransform.resize(TranslationSize);
			for (UINT l = 0; l < TranslationSize; l++)
			{
				fread(&CData.CameraTransform[l].bnC_Translation, sizeof(Vec3), 1, pFile);
			}
			for (UINT l = 0; l < TranslationSize; l++)
			{
				fread(&CData.CameraTransform[l].bnC_Scali, sizeof(Vec3), 1, pFile);
			}
			for (UINT l = 0; l < TranslationSize; l++)
			{
				fread(&CData.CameraTransform[l].bnC_Rotation, sizeof(Vec3), 1, pFile);
			}
			if (bnCamera)
				m_ClipData.push_back(CData);
			else
				m_bnChild->m_ClipData.push_back(CData);
	}	
	UINT TranslationSize;
	fread(&TranslationSize, sizeof(int), 1, pFile);
	fclose(pFile);
}
	

void CCameraAnimaitorScript::SetbnChild(CGameObject* _bnChild)
{
	if(_bnChild->GetScript(L"CCameraAnimaitorScript") !=nullptr)
		m_bnChild =(CCameraAnimaitorScript*)_bnChild->GetScript(L"CCameraAnimaitorScript");
	m_bnCamera = true;
	m_bnChild->m_bnCamera = false;
	
}

void CCameraAnimaitorScript::SetCameraAnim(UINT _i, int _FrameSpeed)
{
	m_CupClipData = &m_ClipData[_i];
	m_iFramespeed = _FrameSpeed;
	m_bFinish = false;
	m_dCurTime = 0.f;
	m_PlayingType = CameraPlayType::AnimPlay;

	if(_i==34)
		m_iFrameEndIdx = m_CupClipData->CameraTransform.size()-60;
	else
		m_iFrameEndIdx = m_CupClipData->CameraTransform.size();
	if (_i !=37 && _i != 29 && _i != 30)
	{
		m_PreTransform.bnC_Rotation = Transform()->GetLocalRot();
		m_PreTransform.bnC_Translation = Transform()->GetLocalPos();
		m_PreTransform.bnC_Scali = Transform()->GetLocalScale();
		m_PreRootTransform.bnC_Rotation = GetGameObject()->GetParent()->Transform()->GetLocalRot();
		m_PreRootTransform.bnC_Translation = GetGameObject()->GetParent()->Transform()->GetLocalPos();
		m_PreRootTransform.bnC_Scali = GetGameObject()->GetParent()->Transform()->GetLocalScale();
	}
	else
	{
		int a = 0;
	}
	if (m_bnCamera)
	{
		if (m_TargetPlayerObj ==nullptr)
		{
			m_TargetPlayerObj = CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Player");
		}
		CPlayerScript* Player = (CPlayerScript*)m_TargetPlayerObj->GetScript(L"CPlayerScript");
		float y = Player->GetTargetAngle();
		switch (_i)
		{
		case 28:
		{
			float x = sin(1.57f + y) * 300.f;
			float z = cos(1.57f + y) * 300.f;
			//float x = sin(1.57f + y) * 250.f;
			//float z = cos(1.57f + y) * 250.f;
			GetGameObject()->GetParent()->Transform()->SetLocalPosX(m_TargetPlayerObj->Transform()->GetLocalPos().x+x);
			GetGameObject()->GetParent()->Transform()->SetLocalPosY(0.f);
			GetGameObject()->GetParent()->Transform()->SetLocalPosZ(m_TargetPlayerObj->Transform()->GetLocalPos().z+z);
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(0.f);
			//

			GetGameObject()->GetParent()->Transform()->SetLocalRotY(0.785f+y);
			GetGameObject()->GetParent()->Transform()->SetLocalRotZ(0.f);
			m_bnChild->SetCameraAnim(_i, _FrameSpeed);

			m_ChargeAttackAnimTime = 0.f;
		}

		break;
		case 29:
		{
			float x = sin(1.57f + y) * 300.f;
			float z = cos(1.57f + y) * 300.f;
			GetGameObject()->GetParent()->Transform()->SetLocalPosX(m_TargetPlayerObj->Transform()->GetLocalPos().x+x);
			GetGameObject()->GetParent()->Transform()->SetLocalPosY(50.f);
			GetGameObject()->GetParent()->Transform()->SetLocalPosZ(m_TargetPlayerObj->Transform()->GetLocalPos().z+z);
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(0.f);
			//

			GetGameObject()->GetParent()->Transform()->SetLocalRotY(1.57f + y);
			GetGameObject()->GetParent()->Transform()->SetLocalRotZ(0.f);
			m_bnChild->SetCameraAnim(_i, _FrameSpeed);

			m_ChargeAttackAnimTime = 0.f;
		}
		break;
		case 30:
		{
			float x = sin(1.57f + y) * 300.f;
			float z = cos(1.57f + y) * 300.f;
		
			GetGameObject()->GetParent()->Transform()->SetLocalPosX(m_TargetPlayerObj->Transform()->GetLocalPos().x+x);
			GetGameObject()->GetParent()->Transform()->SetLocalPosY(50.f);
			GetGameObject()->GetParent()->Transform()->SetLocalPosZ(m_TargetPlayerObj->Transform()->GetLocalPos().z+z);
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(0.f);
			//

			GetGameObject()->GetParent()->Transform()->SetLocalRotY(1.57f + y);
			GetGameObject()->GetParent()->Transform()->SetLocalRotZ(0.f);
			m_bnChild->SetCameraAnim(_i, _FrameSpeed);

			m_ChargeAttackAnimTime = 0.f;
		}
		break;
		case 34:
		{
			float x = -cos(-1.57f + y) * 200.f;
			float z = sin(-1.57f + y) *200.f;
			x += sin(1.57f + y) * 200.f;
			z += cos(1.57f + y) * 200.f;
			GetGameObject()->GetParent()->Transform()->SetLocalPosX(m_TargetPlayerObj->Transform()->GetLocalPos().x + x);
			GetGameObject()->GetParent()->Transform()->SetLocalPosY(0.f);
			GetGameObject()->GetParent()->Transform()->SetLocalPosZ(m_TargetPlayerObj->Transform()->GetLocalPos().z + z);
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(0.f);
			//

			GetGameObject()->GetParent()->Transform()->SetLocalRotY(-0.785f + y);
			GetGameObject()->GetParent()->Transform()->SetLocalRotZ(0.f);
			m_bnChild->SetCameraAnim(_i, _FrameSpeed);
			m_PlayingType = CameraPlayType::ChargeAttackAnin;
		}
		break;
		case 37:
		{
			float x = cos(-1.57f + y) * 200.f;
			float z = -sin(-1.57f + y) * 200.f;
			x += sin(1.57f + y) * 200.f;
			z += cos(1.57f + y) * 200.f;
			GetGameObject()->GetParent()->Transform()->SetLocalPosX(m_TargetPlayerObj->Transform()->GetLocalPos().x + x);
			GetGameObject()->GetParent()->Transform()->SetLocalPosY(0.f);
			GetGameObject()->GetParent()->Transform()->SetLocalPosZ(m_TargetPlayerObj->Transform()->GetLocalPos().z + z);
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(0.f);
			//

			GetGameObject()->GetParent()->Transform()->SetLocalRotY(-0.785f + y);
			GetGameObject()->GetParent()->Transform()->SetLocalRotZ(0.f);
			m_bnChild->SetCameraAnim(_i, _FrameSpeed);
			
			m_ChargeAttackAnimTime = 0.f;
		}
			break;
		}
		

	}
}

void CCameraAnimaitorScript::ClipRecoding()
{
	if (m_PlayingType ==CameraPlayType::Recoding)
	{
		m_PlayingType = m_PrePlayeringType;
		m_ClipData.push_back(m_RecodeClip);
		CameraData TData;
		TData.CameraTransform.resize(m_RecodeClip.CameraTransform.size());
		TData.CameraClipName = m_RecodeClip.CameraClipName;
		m_RecodeClip.CameraTransform.clear();
		m_RecodeNum++;
		m_bnChild->m_ClipData.push_back(TData);
		Transform()->SetLocalRot(m_PreTransform.bnC_Rotation);
		Transform()->SetLocalPos(m_PreTransform.bnC_Translation);
		Transform()->SetLocalScale(m_PreTransform.bnC_Scali);

	}
	else
	{

		m_PlayingType = CameraPlayType::Recoding;
		m_PreTransform.bnC_Rotation = Transform()->GetLocalRot();
		m_PreTransform.bnC_Translation = Transform()->GetLocalPos();
		m_PreTransform.bnC_Scali = Transform()->GetLocalScale();
		m_RecodeClip.CameraClipName = L"RecodeingClip" + std::to_wstring(m_RecodeNum);


	}
	m_dCurTime = 0.f;
}

void CCameraAnimaitorScript::Recoding()
{
	m_dCurTime += fDT;
	// 현재 재생중인 Clip 의 시간을 진행한다.
	//m_vecClipUpdateTime[m_iCurClip] += fDT;

	/*if (m_vecClipUpdateTime[m_iCurClip] >= m_pVecClip->at(m_iCurClip).dTimeLength)
	{
		if (m_bRepeat)
			m_vecClipUpdateTime[m_iCurClip] = 0.f;
		else
			m_vecClipUpdateTime[m_iCurClip] = m_pVecClip->at(m_iCurClip).dTimeLength - 0.04f;
		m_bFinish = true;

	}*/
	
	if (1.f / m_RecodeFrm < m_dCurTime)
	{
		// 현재 프레임 인덱스 구하기
		
		Transformstruct TData;
		TData.bnC_Rotation= Transform()->GetLocalRot();
		TData.bnC_Scali= Transform()->GetLocalScale();
		TData.bnC_Translation = Transform()->GetLocalPos();

		m_RecodeClip.CameraTransform.push_back(TData);
		m_dCurTime = 0.f;
	}


}

void CCameraAnimaitorScript::ClipShow()
{
	if (m_bFinish)
		return;
	m_dCurTime += fDT * m_iFramespeed;
	// 현재 재생중인 Clip 의 시간을 진행한다.
	//m_vecClipUpdateTime[m_iCurClip] += fDT;

	/*if (m_vecClipUpdateTime[m_iCurClip] >= m_pVecClip->at(m_iCurClip).dTimeLength)
	{
		if (m_bRepeat)
			m_vecClipUpdateTime[m_iCurClip] = 0.f;
		else
			m_vecClipUpdateTime[m_iCurClip] = m_pVecClip->at(m_iCurClip).dTimeLength - 0.04f;
		m_bFinish = true;

	}*/

	// 현재 프레임 인덱스 구하기
	double dFrameIdx = m_dCurTime * (double)m_iFrameCount;
	m_iFrameIdx = (int)(dFrameIdx);


	// 다음 프레임 인덱스
	if (m_iFrameIdx >= m_iFrameEndIdx - 1)
	{
		m_dCurTime = 0.f;
		m_bFinish = true;
		if (m_PlayingType == CameraPlayType::ChargeAttackAnin)
		{
			return;
		}
			// 끝이면 현재 인덱스를 유지
		
		
		Transform()->SetLocalRot(m_PreTransform.bnC_Rotation);
		Transform()->SetLocalPos(m_PreTransform.bnC_Translation);
		Transform()->SetLocalScale(m_PreTransform.bnC_Scali);
		m_PlayingType = m_PrePlayeringType;

		if (m_bnCamera)
		{

			GetGameObject()->GetParent()->Transform()->SetLocalRot(m_PreRootTransform.bnC_Rotation);
			GetGameObject()->GetParent()->Transform()->SetLocalPos(m_PreRootTransform.bnC_Translation);
		}
		return;
	}

	Transform()->SetLocalPos(m_CupClipData->CameraTransform[m_iFrameIdx].bnC_Translation);
	//Transform()->SetLocalScale(m_CupClipData->CameraTransform[m_iFrameIdx].bnC_Scali);
	Transform()->SetLocalRot(m_CupClipData->CameraTransform[m_iFrameIdx].bnC_Rotation);
}

void CCameraAnimaitorScript::Zoomin()
{
	if (m_ZoomXCheck == true && m_ZoomYCheck == true && m_ZoomZCheck == true)
		return;


	Vec3 CPos = GetGameObject()->GetParent()->Transform()->GetLocalPos();
	if (CPos.x< (m_TargetMonsterObj->Transform()->GetLocalPos().x + 3.f) && CPos.x>(m_TargetMonsterObj->Transform()->GetLocalPos().x - 3.f))
	{
		m_ZoomXCheck = true;
	}
	else
	CPos.x += m_ZoomDir.x	*fDT* m_ZoomSpeed;
	if (CPos.y > m_ZoomYDistance)
		CPos.y += m_ZoomDir.y * fDT * m_ZoomSpeed;
	else
	{
		m_ZoomYCheck = true;
	}
	if (CPos.z< (m_TargetMonsterObj->Transform()->GetLocalPos().z -400.f+ 3.f) && CPos.z>(m_TargetMonsterObj->Transform()->GetLocalPos().z -400.f- 3.f))
	{
		m_ZoomZCheck = true;
	}
	else
	CPos.z += m_ZoomDir.z * fDT * m_ZoomSpeed;
	
	GetGameObject()->GetParent()->Transform()->SetLocalPos(CPos);
			
}

void CCameraAnimaitorScript::ZoomOut()
{
	if (m_ZoomXCheck == true && m_ZoomYCheck == true && m_ZoomZCheck == true)
	{
		m_ZoomXCheck = false;
		m_ZoomYCheck = false;
		m_ZoomZCheck = false;
		GetGameObject()->GetParent()->Transform()->SetLocalPos(m_PreRootTransform.bnC_Translation);
		m_PlayingType = m_PrePlayeringType;
		if (m_PlayingType == CameraPlayType::NormalBattle)
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(1.f);
		else if(m_PlayingType == CameraPlayType::CloseBattle)
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(0.15f);
	}
	Vec3 CPos = GetGameObject()->GetParent()->Transform()->GetLocalPos();
	if (CPos.x< (m_PreRootTransform.bnC_Translation.x + 3.f) && CPos.x>(m_PreRootTransform.bnC_Translation.x - 3.f))
		m_ZoomXCheck = true;
	else
		CPos.x += -m_ZoomDir.x * fDT * m_ZoomSpeed;

	if (CPos.y < m_PreRootTransform.bnC_Translation.y)
		CPos.y += -m_ZoomDir.y * fDT * m_ZoomSpeed;
	else
		m_ZoomYCheck = true;
	if (CPos.z< (m_PreRootTransform.bnC_Translation.z + 3.f) && CPos.z>m_PreRootTransform.bnC_Translation.z - 3.f)
		m_ZoomZCheck = true;
	else
		CPos.z += -m_ZoomDir.z * fDT * m_ZoomSpeed;

	
	GetGameObject()->GetParent()->Transform()->SetLocalPos(CPos);

}

void CCameraAnimaitorScript::ChangeBattle()
{
	if (CameraPlayType::NormalBattle == m_PlayingType)
	{
		m_PlayingType = CameraPlayType::CloseBattle;
		m_PrePlayeringType = CameraPlayType::CloseBattle;
		CPlayerScript* Player = (CPlayerScript*)m_TargetPlayerObj->GetScript(L"CPlayerScript");
		Player->SetCloseBattle(true);
		
	}
	else if (CameraPlayType::CloseBattle == m_PlayingType)
	{
		m_PlayingType = CameraPlayType::NormalBattle;
		m_PrePlayeringType = CameraPlayType::NormalBattle;
		CPlayerScript* Player = (CPlayerScript*)m_TargetPlayerObj->GetScript(L"CPlayerScript");
		Player->SetCloseBattle(false);
	}
}

void CCameraAnimaitorScript::NormalBattle()
{
	if (m_bnCamera)
	{
		if (GetGameObject()->GetParent()->Transform()->GetLocalRot().x < 1.f)
		{
			float  xx = GetGameObject()->GetParent()->Transform()->GetLocalRot().x + fDT / 1.5f;
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(xx);
		}
		else
		{
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(1.f);
		}
		m_PlayerPos.x = m_TargetPlayerObj->Transform()->GetLocalPos().x;
		m_PlayerPos.y = m_TargetPlayerObj->Transform()->GetLocalPos().z;
		m_TargetPos.x = m_TargetMonsterObj->Transform()->GetLocalPos().x;
		m_TargetPos.y = m_TargetMonsterObj->Transform()->GetLocalPos().z;
		Vec2 vPos = (m_TargetPos - m_PlayerPos)/2.f;
		m_Distance = Vec2::Distance(m_PlayerPos, m_TargetPos)*2.f;
		if (m_Distance > 2500.f)
			return;
		if (m_Distance < 800.f)
			m_Distance = 800.f;
		float z = m_Distance * tanf(0.57);
		Vec3 CalPos = Vec3(m_PlayerPos.x+vPos.x, m_Distance, m_PlayerPos.y+vPos.y-z);
		Vec3 CaPos= GetGameObject()->GetParent()->Transform()->GetLocalPos();
		if (KEY_TAP(KEY_TYPE::ENTER))
		{
			int a = 0;
		}
		CalPos -= CaPos;
		if ((CalPos.x <10.f && CalPos.x > -10.f) || (CalPos.z <5.f && CalPos.z > -10.f))
			return;
		CalPos.Normalize();
		CaPos += CalPos * 1000.f * fDT;
		GetGameObject()->GetParent()->Transform()->SetLocalPos(CaPos);
	}
}

void CCameraAnimaitorScript::CloseBattle()
{
	//
	if (m_bnCamera)
	{
		if (GetGameObject()->GetParent()->Transform()->GetLocalRot().x > 0.15f)
		{
			float  xx = GetGameObject()->GetParent()->Transform()->GetLocalRot().x - fDT/1.5f;
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(xx);
		}
		else
		{
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(0.15f);
		}

		m_PlayerPos.x = m_TargetPlayerObj->Transform()->GetLocalPos().x;
		m_PlayerPos.y = m_TargetPlayerObj->Transform()->GetLocalPos().z;
		m_TargetPos.x = m_TargetMonsterObj->Transform()->GetLocalPos().x;
		m_TargetPos.y = m_TargetMonsterObj->Transform()->GetLocalPos().z;
		Vec2 vPos = (m_TargetPos - m_PlayerPos) / 2.f;
		//m_Distance = Vec2::Distance(m_PlayerPos, m_TargetPos) * 2.f;
		//if (m_Distance > 2000.f)
			//return;
		//if (m_Distance <	100)
			//m_Distance =	400;
		//float z = 200.f * tanf(0.15);
		Vec3 CalPos;
		if(vPos.y<0)
			CalPos = Vec3(m_PlayerPos.x + vPos.x, 200.f, m_TargetPos.y - 500.f);
		else
			CalPos = Vec3(m_PlayerPos.x + vPos.x, 200.f, m_PlayerPos.y - 500.f);
		Vec3 CaPos = GetGameObject()->GetParent()->Transform()->GetLocalPos();
		CalPos -= CaPos;
		
		Vec3 CDir = CalPos;
		CDir.Normalize();
		if (CalPos.x <10.f && CalPos.x > -10.f)
		{

		}
		else
		{
			CaPos.x += CDir.x *1000.f * fDT;
		}
		if (CalPos.y <3.f && CalPos.y > -3.f)
		{

		}
		else
		{
			CaPos.y += CDir.y * 1000.f * fDT;
		}
		if(CalPos.z <10.f && CalPos.z > -10.f)
		{

		}
		else
		{
			CaPos.z += CDir.z * 1000.f * fDT;
		}
		
		GetGameObject()->GetParent()->Transform()->SetLocalPos(CaPos);
	}

}

const float HitScale = 300.f;
void CCameraAnimaitorScript::HiT()
{
	Vec3 vPos = GetGameObject()->GetParent()->Transform()->GetLocalPos();
		m_HitTime += fDT;

		if (m_HitTime < 0.025)
		{
			vPos.y += fDT * HitScale;
		}
		else if (m_HitTime >= 0.025f && m_HitTime < 0.05f)
		{
			vPos.y -= fDT * HitScale;
		}
		else if (m_HitTime >= 0.05f && m_HitTime < 0.075f)
		{
			vPos.x += fDT * HitScale;
		}
		else if (m_HitTime >= 0.075f && m_HitTime < 0.1f)
		{
			vPos.x -= fDT * HitScale;
		}
		else if (m_HitTime > 0.1f && m_HitTime < 0.125f)
		{
			vPos.y += fDT * HitScale;
		}
		else if (m_HitTime >= 0.125f && m_HitTime < 0.15f)
		{
			vPos.y -= fDT * HitScale;
		}
		else if (m_HitTime >= 0.15f && m_HitTime < 0.175f)
		{
			vPos.x += fDT * HitScale;
		}
		else if (m_HitTime >= 0.175f && m_HitTime < 0.2f)
		{
			vPos.x -= fDT * HitScale;
		}
		else if (m_HitTime > 0.2f)
		{
			m_HitTime = 0.f;
			m_HitOn = false;
			vPos = m_HitPrePos;
		}
		GetGameObject()->GetParent()->Transform()->SetLocalPos(vPos);
}

void CCameraAnimaitorScript::MonsterZoomin(float _ZoominSpeed, float _ZoominDistance)
{
	
	if (m_bnCamera)
	{
		if (m_PlayingType != CameraPlayType::Zoomin) {
			m_PreRootTransform.bnC_Rotation = GetGameObject()->GetParent()->Transform()->GetLocalRot();
			m_PreRootTransform.bnC_Translation = GetGameObject()->GetParent()->Transform()->GetLocalPos();
			m_PreRootTransform.bnC_Scali = GetGameObject()->GetParent()->Transform()->GetLocalScale();
			m_ZoomSpeed = _ZoominSpeed*1.5f;
			m_PlayingType = CameraPlayType::Zoomin;
			m_ZoomXCheck = false;
			m_ZoomYCheck = false;
			m_ZoomZCheck = false;
			Vec3 Mpos = m_TargetMonsterObj->Transform()->GetLocalPos();
			Mpos.z -= 400.f;
			m_ZoomDir = Mpos;
			m_ZoomDir.y = _ZoominDistance/2.f;
			m_ZoomYDistance = _ZoominDistance/2.f;
			m_ZoomDir -= m_PreRootTransform.bnC_Translation;
			m_ZoomDir.Normalize();
			GetGameObject()->GetParent()->Transform()->SetLocalRotX(0.15f);


		}
		else {
			m_PlayingType = CameraPlayType::ZoomOut;
		
			m_ZoomXCheck = false;
			m_ZoomYCheck = false;
			m_ZoomZCheck = false;
		}
	}
	
}

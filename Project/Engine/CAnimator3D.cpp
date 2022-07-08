#include "pch.h"
#include "CAnimator3D.h"

#include "CTimeMgr.h"
#include "CMeshRender.h"
#include "CStructuredBuffer.h"
#include "CResMgr.h"

#include "CAnimation3DShader.h"

#include "CKeyMgr.h"

CAnimator3D::CAnimator3D()
	: m_iCurClip(0)//(0)
	, m_dCurTime(0.)
	, m_iFrameCount(30)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_pChangeBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_iFrameIdx(0)
	, m_iNextFrameIdx(0)
	, m_fRatio(0.f)
	, m_bRepeat(true)
	, m_bFinish(false)
	, m_iChangeClip(0)
	, m_bBlend(false)
	, m_fBlendTime(0.f)
	, m_iChangeFrameIdx(-1)
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
	, m_fAniSpeed(1.f)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;
	m_pChangeBoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::~CAnimator3D()
{
	SAFE_DELETE(m_pBoneFinalMatBuffer);
	SAFE_DELETE(m_pChangeBoneFinalMatBuffer);
}

void CAnimator3D::update()
{
}

void CAnimator3D::lateupdate()
{

}

void CAnimator3D::finalupdate()
{
	if (m_bBlend) //블렌드
	{
		BlendAnimation();
	}
	else //일반 업데이트
	{
		NormalUpdate();
	}
	/*
	if (!m_bRepeat && m_bFinish)
		return;

	if (m_iCurClip >= m_pVecClip->size())
	{
		m_iCurClip = 0;
	}
	m_dCurTime = 0.f;

	if (m_bBlend)  //블렌딩
	{
		BlendAnimation();
	}
	else //그냥 업데이트
	{
		// 현재 재생중인 Clip 의 시간을 진행한다.
		m_vecClipUpdateTime[m_iCurClip] += fDT;

		if (m_vecClipUpdateTime[m_iCurClip] >= m_pVecClip->at(m_iCurClip).dTimeLength)
		{
			m_vecClipUpdateTime[m_iCurClip] = 0.f;
			if (!m_bRepeat) m_bFinish = true;
		}

		m_dCurTime = m_pVecClip->at(m_iCurClip).dStartTime + m_vecClipUpdateTime[m_iCurClip];

		// 현재 프레임 인덱스 구하기
		double dFrameIdx = m_dCurTime * (double)m_iFrameCount;
		m_iFrameIdx = (int)(dFrameIdx);

		// 다음 프레임 인덱스
		if (m_iFrameIdx >= m_pVecClip->at(m_iCurClip).iEndFrame - 1)//.iFrameLength - 1)
		{
			m_iNextFrameIdx = m_iFrameIdx;	// 끝이면 현재 인덱스를 유지

		}
		else
			m_iNextFrameIdx = m_iFrameIdx + 1;

		// 프레임간의 시간에 따른 비율을 구해준다.
		m_fRatio = (float)(dFrameIdx - (double)m_iFrameIdx);

		// 컴퓨트 쉐이더 연산여부
		m_bFinalMatUpdate = false;

	}
	*/

}

void CAnimator3D::NormalUpdate()
{
	if (!m_bRepeat && m_bFinish)
		return;
	m_dCurTime = 0.f;
	m_bFinish = false;
	// 현재 재생중인 Clip 의 시간을 진행한다.
	m_vecClipUpdateTime[m_iCurClip] += fDT/ m_fAniSpeed;
	float Check = m_vecClipUpdateTime[m_iCurClip] + 0.03;
	
	if (Check >= m_pVecClip->at(m_iCurClip).dTimeLength)
	{
		if (m_bRepeat)
			m_vecClipUpdateTime[m_iCurClip] = 0.f;
		else
			m_vecClipUpdateTime[m_iCurClip] = m_pVecClip->at(m_iCurClip).dTimeLength-0.04f;
		m_bFinish = true;

	}

	m_dCurTime = m_pVecClip->at(m_iCurClip).dStartTime + m_vecClipUpdateTime[m_iCurClip];

	// 현재 프레임 인덱스 구하기
	double dFrameIdx = m_dCurTime * (double)m_iFrameCount;
	m_iFrameIdx = (int)(dFrameIdx);

	// 다음 프레임 인덱스
	if (m_iFrameIdx >= m_pVecClip->at(m_iCurClip).iFrameLength-1)
		m_iNextFrameIdx = m_iFrameIdx;	// 끝이면 현재 인덱스를 유지
	else
		m_iNextFrameIdx = m_iFrameIdx + 1;

	// 프레임간의 시간에 따른 비율을 구해준다.
	m_fRatio = (float)(dFrameIdx - (double)m_iFrameIdx);

	// 컴퓨트 쉐이더 연산여부
	m_bFinalMatUpdate = false;

}

void CAnimator3D::BlendAnimation()
{
	// 현재 재생중인 Clip 의 시간을 진행한다.
	m_vecClipUpdateTime[m_iChangeClip] += fDT;

	if (m_vecClipUpdateTime[m_iChangeClip] >= m_pVecClip->at(m_iChangeClip).dTimeLength)
	{
		m_vecClipUpdateTime[m_iChangeClip] = 0.f;
		//if (!m_bRepeat) m_bFinish = true;
	}

	m_dCurTime = m_pVecClip->at(m_iChangeClip).dStartTime + m_vecClipUpdateTime[m_iChangeClip];

	// 현재 프레임 인덱스 구하기
	double dFrameIdx = m_dCurTime * (double)m_iFrameCount;
	m_iChangeFrameIdx = (int)(dFrameIdx);

	// 다음 프레임 인덱스
	if (m_iChangeFrameIdx >= m_pVecClip->at(m_iChangeClip).iEndFrame - 1)//.iFrameLength - 1)
	{
		m_iNextFrameIdx = m_iChangeFrameIdx;	// 끝이면 현재 인덱스를 유지
	}
	else
	{
		m_iNextFrameIdx = m_iChangeFrameIdx + 1;
	}

	float bt = 0.1f;  //블렌딩 시간

	m_fBlendTime += fDT;
	if (m_fBlendTime >= bt) //0.7초
	{
		//blend 끝
		m_bBlend = false;
		m_iCurClip = m_iChangeClip;
		m_vecClipUpdateTime[m_iCurClip] = m_fBlendTime;
		m_fBlendTime = 0.f;
		//m_iChangeClip = 0;
		m_iChangeFrameIdx = -1;
		m_iChangeClip = -1;
	}

	// 프레임간의 시간에 따른 비율을 구해준다.
	//m_fRatio = (float)(dFrameIdx - (double)m_iChangeFrameIdx);
	m_fRatio = 1.f - (bt - m_fBlendTime) / bt;

	// 컴퓨트 쉐이더 연산여부 
	m_bFinalMatUpdate = false;
}


void CAnimator3D::ChangeClip(UINT _i, bool _bRepeat, float _fAniSpeed)
{
	//int idx = FindAnimation(_strAnimName); //못찾으면 -1
	//if (idx == -1)
	//{
	//	assert(idx != -1); //false 일때 assert
	//	return;
	//}

	if (_i >= m_pVecClip->size())
		return;
	m_bRepeat = _bRepeat;
	m_bBlend = false;

	m_fAniSpeed = _fAniSpeed;

	m_fBlendTime = 0.f;
	m_iChangeClip = _i;
	m_bFinish = false;
	m_vecClipUpdateTime[m_iChangeClip] = 0.f;
	//m_pCurAnim = iter->second;
	//m_pCurAnim->SetFrameIndex(0, true);
	//m_iCurClip = _i;
	//m_vecClipUpdateTime[m_iCurClip] = 0.f;

	{
		//blend 끝
		//m_bBlend = false;
		m_iCurClip = m_iChangeClip;
		//m_vecClipUpdateTime[m_iCurClip] = m_fBlendTime;
		m_fBlendTime = 0.f;
		//m_iChangeClip = 0;
		m_iChangeFrameIdx = -1;
		m_iChangeClip = -1;
	}
}

int CAnimator3D::ChangeClip(const wstring& _strName, bool _bRepeat, float _fAniSpeed)
{
	int idx = FindAnimation(_strName); //못찾으면 -1
	//assert(idx != -1); //false 일때 assert

	if (idx < 0 || idx >= m_pVecClip->size())
	{
		assert(idx != -1); //false 일때 assert
		return -1;
	}

	m_fAniSpeed = _fAniSpeed;
	m_bRepeat = _bRepeat;
	m_bBlend = false;

	m_fBlendTime = 0.f;
	m_iChangeClip = idx;
	m_bFinish = false;
	m_vecClipUpdateTime[m_iChangeClip] = 0.f;
	//m_pCurAnim = iter->second;
	//m_pCurAnim->SetFrameIndex(0, true);
	//m_iCurClip = _i;
	//m_vecClipUpdateTime[m_iCurClip] = 0.f;

	{
		//blend 끝
		//m_bBlend = false;
		m_iCurClip = m_iChangeClip;
		//m_vecClipUpdateTime[m_iCurClip] = m_fBlendTime;
		m_fBlendTime = 0.f;
		//m_iChangeClip = 0;
		m_iChangeFrameIdx = -1;
		m_iChangeClip = -1;
	}

	return idx;
}


float CAnimator3D::GetAnimFrmRatio()
{
	float n = (float)m_iFrameIdx / (float)m_pVecClip->at(m_iCurClip).iFrameLength;
	return n;
}

void CAnimator3D::PlayAnimation(const wstring& _strName, bool _bRepeat)//, bool _bInit)
{
	int idx = FindAnimation(_strName); //못찾으면 -1
	assert(idx != -1); //false 일때 assert

	m_iCurClip = idx;
	m_bRepeat = _bRepeat;
	//MeshRender()->GetMesh()->PushAniClip(_bRepeat);

	m_iFrameIdx = 0;
	m_iNextFrameIdx = 0;
	//m_vecClipUpdateTime[m_iCurClip] = 0.f;
	m_bFinish = false;
	//bool m_bRepeat;
}



int CAnimator3D::FindAnimation(const wstring& _strAnimName)
{
	for (int i = 0; i < m_pVecClip->size(); ++i)
	{
		if (_strAnimName == m_pVecClip->at(i).strAnimName)
		{
			return i;
		}
	}
	return -1;
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	m_pVecClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());

	// 선생님 test 코드
	//static float fTime = 0.f;
	//fTime += 1.f;
	//m_vecClipUpdateTime[0] = fTime;
}

void CAnimator3D::UpdateData()
{

	if (!m_bFinalMatUpdate)
	{
		// Animation3D Update Compute Shader
		CAnimation3DShader* pUpdateShader = (CAnimation3DShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"CAnimation3DUpdateShader").Get();

		// Bone Data
		Ptr<CMesh> pMesh = MeshRender()->GetMesh();
		check_mesh(pMesh);

		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer(m_iCurClip));
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer(m_iCurClip));
		if (m_iChangeClip >= 0)
		{
			pUpdateShader->SetChangeFrameDataBuffer(pMesh->GetBoneFrameDataBuffer(m_iChangeClip));
			pUpdateShader->SetChangeOffsetMatBuffer(pMesh->GetBoneOffsetBuffer(m_iChangeClip));
		}
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);

		/*pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer(m_iChangeClip));
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer(m_iChangeClip));
		pUpdateShader->SetOutputBuffer(m_pChangeBoneFinalMatBuffer);*/

		UINT iBoneCount = (UINT)m_pVecBones[m_iCurClip]->size();
		if (m_iChangeClip >= 0)
		{
			UINT iChangeBoneCount = (UINT)m_pVecBones[m_iChangeClip]->size();
			pUpdateShader->SetChangeBoneCount(iChangeBoneCount);
		}

		pUpdateShader->SetBoneCount(iBoneCount);
		pUpdateShader->SetFrameIndex(m_iFrameIdx);
		pUpdateShader->SetNextFrameIdx(m_iNextFrameIdx);
		pUpdateShader->SetFrameRatio(m_fRatio);

		pUpdateShader->SetChangeFrameIndex(m_iChangeFrameIdx);


		// 업데이트 쉐이더 실행
		pUpdateShader->Excute();

		m_bFinalMatUpdate = true;
	}

	// t12 레지스터에 최종행렬 데이터(구조버퍼) 바인딩	
	m_pBoneFinalMatBuffer->UpdateData(12);

}

void CAnimator3D::ClearData()
{
	m_pBoneFinalMatBuffer->Clear();

	UINT iMtrlCount = MeshRender()->GetMtrlCount();
	Ptr<CMaterial> pMtrl = nullptr;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		pMtrl = MeshRender()->GetSharedMaterial(i);
		if (nullptr == pMtrl)
			continue;
		pMtrl->SetAnim3D(false); // Animation Mesh 알리기
		pMtrl->SetBoneCount(0);
	}
}

void CAnimator3D::ClipNameSave()
{
	FILE* pClipNameListFile = nullptr;
	wstring strFileName = CPathMgr::GetResPath();
	
	wstring name = strFileName + L"AnimationNameList\\" +GetObj()->GetName()+L".txt";
	_wfopen_s(&pClipNameListFile, name.c_str(), L"wb");
	if (nullptr != pClipNameListFile)
	{
		wchar_t szScriptName[50] = L"";
		vector<wstring> strCurScriptList;
		for (UINT i = 0; i < m_pVecClip->size(); ++i)
		{
			fwprintf_s(pClipNameListFile, m_pVecClip->at(i).strAnimName.c_str());
			fwprintf_s(pClipNameListFile, L",");
			fwprintf_s(pClipNameListFile, L"\n");
		}
		fclose(pClipNameListFile);
	}
}

void CAnimator3D::check_mesh(Ptr<CMesh> _pMesh)
{
	UINT iBoneCount = _pMesh->GetBoneCount(m_iCurClip);
	if (m_pBoneFinalMatBuffer->GetElementCount() != iBoneCount)
	{
		m_pBoneFinalMatBuffer->CreateRW(sizeof(Matrix), iBoneCount, nullptr);
	}

	if (m_bBlend)
	{
		iBoneCount = _pMesh->GetBoneCount(m_iChangeClip);
		if (m_pBoneFinalMatBuffer->GetElementCount() != iBoneCount)
		{
			m_pBoneFinalMatBuffer->CreateRW(sizeof(Matrix), iBoneCount, nullptr);
		}
	}

}

void CAnimator3D::SaveToScene(FILE* _pFile)
{
}

void CAnimator3D::LoadFromScene(FILE* _pFile)
{
}
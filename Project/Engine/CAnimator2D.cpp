#include "pch.h"
#include "CAnimator2D.h"

#include "CAnimation2D.h"

#include "CMeshRender.h"
#include "CMaterial.h"

#include "CRenderMgr.h"
#include "CStructuredBuffer.h"

CAnimator2D::CAnimator2D()
	: CComponent(COMPONENT_TYPE::ANIMATOR2D)
	, m_pCurAnim(nullptr)
	, m_bRepeat(false)
{
}

CAnimator2D::CAnimator2D(const CAnimator2D& _origin) :
	CComponent(COMPONENT_TYPE::ANIMATOR2D),
	m_bRepeat(_origin.m_bRepeat)
{
	map<wstring, CAnimation2D*>::const_iterator iter = _origin.m_mapAnim.begin();
	
	for (; iter != _origin.m_mapAnim.end(); ++iter)
	{
		CAnimation2D* Ani = iter->second->Clone();
		Ani->m_pAnimator = this;

		m_mapAnim.insert(make_pair(iter->first, Ani));

	}
	if (!_origin.m_pCurAnim)
	{
		PlayAnimation(m_mapAnim.begin()->second->GetName(),false);
	}
	else
		PlayAnimation(_origin.m_pCurAnim->GetName(),false);
}

CAnimator2D::~CAnimator2D()
{
	Safe_Delete_Map(m_mapAnim);
}

void CAnimator2D::lateupdate()
{
	m_pCurAnim->lateupdate();

	if (m_bRepeat && m_pCurAnim->IsFinish())
		m_pCurAnim->SetFrameIndex(0, true);
}

void CAnimator2D::ChangeAnimation(const wstring& _strAnimName)
{
	map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(_strAnimName);
	if (iter == m_mapAnim.end())
		return;

	m_pCurAnim = iter->second;
	m_pCurAnim->SetFrameIndex(0, true);
}

void CAnimator2D::CreateAnimation(const wstring& _strAnimName, Ptr<CTexture> _pAnimTex, Vec2 _vLeftTop, Vec2 _vStep, int _iFrameCount, float _fDuration)
{
	CAnimation2D* pAnim = FindAnimation(_strAnimName);
	assert(nullptr == pAnim);

	pAnim = new CAnimation2D;

	pAnim->Create(_strAnimName, _pAnimTex, _vLeftTop, _vStep, _iFrameCount, _fDuration);
	
	m_mapAnim.insert(make_pair(_strAnimName, pAnim));
	pAnim->m_pAnimator = this;
}

CAnimation2D* CAnimator2D::FindAnimation(const wstring& _strName)
{
	map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(_strName);

	if (iter == m_mapAnim.end())
		return nullptr;

	return iter->second;
}

void CAnimator2D::PlayAnimation(const wstring& _strName, bool _bRepeat, bool _bInit)
{
	CAnimation2D* pAnim = FindAnimation(_strName);
	assert(pAnim);

	m_pCurAnim = pAnim;
	m_bRepeat = _bRepeat;

	if (_bInit)
	{
		m_pCurAnim->SetFrameIndex(0, true);
	}
}

void CAnimator2D::UpdateData()
{
	const tAnimFrm& tFrm = m_pCurAnim->GetCurFrame();
	Ptr<CTexture> pAnimTex = m_pCurAnim->GetTex();

	tAnim2D info = {};
	info.iAnimUse[0] = 1;
	info.vLeftTop = tFrm.vLT;
	info.vStep = tFrm.vStep;

	CRenderMgr::GetInst()->GetAnim2DBuffer()->SetData(&info, sizeof(tAnim2D), 1);
	CRenderMgr::GetInst()->GetAnim2DBuffer()->UpdateData(72);

	pAnimTex->UpdateData(73, (UINT)PIPELINE_STAGE::PS_PIXEL);
}

void CAnimator2D::Clear()
{
	tAnim2D info = {};
	info.iAnimUse[0] = 0;

	CRenderMgr::GetInst()->GetAnim2DBuffer()->SetData(&info, sizeof(tAnim2D), 1);
	CTexture::Clear(73, (UINT)PIPELINE_STAGE::PS_PIXEL);
}

void CAnimator2D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	UINT iAnimCount = (UINT)m_mapAnim.size();
	fwrite(&iAnimCount, sizeof(UINT), 1, _pFile);

	map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.begin();
	for (; iter != m_mapAnim.end(); ++iter)
	{
		iter->second->SaveToScene(_pFile);
	}

	bool bCurAnim = false;
	if (m_pCurAnim)
	{
		bCurAnim = true;
		fwrite(&bCurAnim, 1, 1, _pFile);
		fwrite(&m_bRepeat, 1, 1, _pFile);
		SaveWString(m_pCurAnim->GetName(), _pFile);
	}
	else
	{
		fwrite(&bCurAnim, 1, 1, _pFile);
	}
}

void CAnimator2D::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	UINT iAnimCount = 0;
	fread(&iAnimCount, sizeof(UINT), 1, _pFile);
		
	for (UINT i = 0; i < iAnimCount; ++i)
	{
		CAnimation2D* pAnim = new CAnimation2D;
		pAnim->LoadFromScene(_pFile);

		m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));
		pAnim->m_pAnimator = this;
	}

	bool bCurAnim = false;
	fread(&bCurAnim, 1, 1, _pFile);

	if (bCurAnim)
	{			
		fread(&m_bRepeat, 1, 1, _pFile);

		wstring strCurAnimKey;
		LoadWString(strCurAnimKey, _pFile);
		ChangeAnimation(strCurAnimKey);
	}
}

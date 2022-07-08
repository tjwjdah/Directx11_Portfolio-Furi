#include "pch.h"
#include "CAnimation2D.h"

#include "CTimeMgr.h"
#include "CResMgr.h"

CAnimation2D::CAnimation2D()
	: m_pAnimator(nullptr)
	, m_iCurFrm(0)
	, m_fAccTime(0.f)
	, m_bFinish(false)
	, m_iFrameCount(0)
{
}

CAnimation2D::~CAnimation2D()
{
}

void CAnimation2D::lateupdate()
{
	assert(!m_vecFrm.empty());

	if (m_bFinish)
		return;

	m_fAccTime += fDT;
	if (m_fAccTime >= m_vecFrm[m_iCurFrm].fDuration)
	{
		while (true)
		{
			m_fAccTime = m_fAccTime - m_vecFrm[m_iCurFrm].fDuration;
			++m_iCurFrm;

			if (m_iCurFrm >= m_vecFrm.size())
			{
				m_iCurFrm -= 1;
				m_bFinish = true;
			}

			if(m_fAccTime < m_vecFrm[m_iCurFrm].fDuration)
			{
				break;
			}
		}		
	}
}

void CAnimation2D::Create(const wstring& _strAnimName, Ptr<CTexture> _pAnimTex
	, Vec2 _vLeftTop, Vec2 _vStep, int _iFrameCount, float _fDuration)
{
	assert(_pAnimTex.Get());
	m_iFrameCount = _iFrameCount;
	SetName(_strAnimName);
	m_pTex = _pAnimTex;

	tAnimFrm frm = {};
	frm.fDuration = _fDuration;	

	float fWidth = (float)_pAnimTex->Width();
	float fHeight = (float)_pAnimTex->Height();

	frm.vStep = Vec2(_vStep.x / fWidth, _vStep.y / fHeight);

	for (int i = 0; i < _iFrameCount; ++i)
	{
		frm.vLT = Vec2(_vLeftTop.x / fWidth + frm.vStep.x * (float)i, _vLeftTop.y / fHeight);		
		m_vecFrm.push_back(frm);
	}
}

void CAnimation2D::SaveToScene(FILE* _pFile)
{
	CEntity::SaveToScene(_pFile);

	UINT iFrmCount = (UINT)m_vecFrm.size();
	fwrite(&iFrmCount, sizeof(UINT), 1, _pFile);

	for (size_t i = 0; i < m_vecFrm.size(); ++i)
	{
		fwrite(&m_vecFrm[i], sizeof(tAnimFrm), 1, _pFile);
	}
	
	SaveResRefInfo(m_pTex, _pFile);

	fwrite(&m_iCurFrm, sizeof(int), 1, _pFile);
}

void CAnimation2D::LoadFromScene(FILE* _pFile)
{
	CEntity::LoadFromScene(_pFile);

	UINT iFrmCount = 0;
	fread(&iFrmCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iFrmCount; ++i)
	{
		tAnimFrm frm = {};
		fread(&frm, sizeof(tAnimFrm), 1, _pFile);
		m_vecFrm.push_back(frm);
	}

	LoadResRefInfo(m_pTex, _pFile);

	fread(&m_iCurFrm, sizeof(int), 1, _pFile);
}
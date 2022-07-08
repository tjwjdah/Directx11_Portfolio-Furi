#include "pch.h"
#include "CWeightShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CWeightShader::CWeightShader()
	: m_pWeightTex(nullptr)
	, m_pPickInfo(nullptr)
{
}

CWeightShader::~CWeightShader()
{
}

void CWeightShader::UpdateData()
{
	// 필요한 텍스쳐 전달
	m_pWeightTex->UpdateDataRW(0);

	if (nullptr != m_pBrushTexArr)
	{
		m_pBrushTexArr->UpdateData(8, (UINT)PIPELINE_STAGE::PS_COMPUTE);
		m_tInfo.arrTexCheck[8] = 1; // t8 레지스터에 텍스쳐 전달된거 알려줌
	}

	// 필요한 상수값 전달
	m_tInfo.arrInt[0] = m_iTileIdx;
	m_tInfo.arrInt[1] = m_iBrushIdx;
	m_tInfo.arrFloat[0] = m_fBrushScale;
	m_tInfo.arrVec2[0] = m_vWeightTexResolution;

	static const CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_tInfo);
	pMtrlBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_COMPUTE);

	// 구조화버퍼 전달
	m_pPickInfo->UpdateData(14);
}

void CWeightShader::Clear()
{
	// 전달한 텍스쳐 클리어
	CTexture::ClearRW(0);
	CTexture::Clear(8, (UINT)PIPELINE_STAGE::PS_COMPUTE);

	// 전달한 구조화버퍼 클리어	
	m_pPickInfo->Clear();
}

void CWeightShader::Excute()
{
	if (nullptr == m_pWeightTex || nullptr == m_pBrushTexArr)
		return;

	UINT iWidth = m_pWeightTex->Width();
	UINT iHeight = m_pWeightTex->Height();

	Dispatch(iWidth / 32 + 1, iHeight / 32 + 1, 1);
}
#include "pch.h"
#include "CHeightMapShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CHeightMapShader::CHeightMapShader()
	: m_tInfo{}
	, m_pPickingBuffer(nullptr)
{
}

CHeightMapShader::~CHeightMapShader()
{
}

void CHeightMapShader::UpdateData()
{
	m_pHeightMap->UpdateDataRW(0);	
	m_pBrushTex->UpdateData(13, (UINT)PIPELINE_STAGE::PS_COMPUTE);
	m_pPickingBuffer->UpdateData(14);
}

void CHeightMapShader::Clear()
{
	CTexture::ClearRW(0);
	CTexture::Clear(13, (UINT)PIPELINE_STAGE::PS_COMPUTE);
	m_pPickingBuffer->Clear();
}

void CHeightMapShader::Excute()
{
	if (nullptr == m_pHeightMap || nullptr == m_pBrushTex)
		return;

	UINT iWidth = m_pHeightMap->Width();
	UINT iHeight = m_pHeightMap->Height();
	
	m_tInfo.arrInt[0] = iWidth;
	m_tInfo.arrInt[1] = iHeight;

	static const CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_tInfo);
	pMtrlBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_COMPUTE);

	Dispatch(iWidth / 32 + 1, iHeight / 32 + 1, 1);	
}
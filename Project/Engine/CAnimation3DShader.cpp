#include "pch.h"
#include "CAnimation3DShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CAnimation3DShader::CAnimation3DShader()
	: m_tInfo{}
	, m_pFrameDataBuffer(nullptr)
	, m_pOffsetMatBuffer(nullptr)
	, m_pOutputBuffer(nullptr)
	//, m_pChangeOutputBuffer(nullptr)
{
}

CAnimation3DShader::~CAnimation3DShader()
{
}

void CAnimation3DShader::UpdateData()
{
	// 필요한 상수값 전달	
	static const CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_tInfo);
	pMtrlBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_COMPUTE);

	// 구조화버퍼 전달
	m_pFrameDataBuffer->UpdateData(13);
	m_pOffsetMatBuffer->UpdateData(14);
	if (m_tInfo.arrInt[3] >= 0)
	{
		m_pChangeOffsetMatBuffer->UpdateData(15);
		m_pChangeFrameDataBuffer->UpdateData(16);
	}

	m_pOutputBuffer->UpdateDataRW(0);

}


void CAnimation3DShader::Clear()
{
	// 전달한 구조화버퍼 클리어	
	m_pFrameDataBuffer->Clear();
	m_pOffsetMatBuffer->Clear();
	m_pChangeOffsetMatBuffer->Clear();
	m_pOutputBuffer->ClearRW();
	//m_pChangeOutputBuffer->ClearRW();
}

void CAnimation3DShader::Excute()
{
	UINT iGrounX = (m_tInfo.arrInt[0] / 256) + 1;
	Dispatch(iGrounX, 1, 1);
}
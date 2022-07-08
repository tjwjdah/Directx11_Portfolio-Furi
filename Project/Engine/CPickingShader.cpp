#include "pch.h"
#include "CPickingShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CPickingShader::CPickingShader()
	: m_info{}
{
}

CPickingShader::~CPickingShader()
{
}

void CPickingShader::UpdateData()
{
	m_pHeightMap->UpdateData(0, (UINT)PIPELINE_STAGE::PS_COMPUTE);
	m_pOutputBuffer->UpdateDataRW(0);
}

void CPickingShader::Clear()
{
	m_pHeightMap->Clear(0, (UINT)PIPELINE_STAGE::PS_COMPUTE);
	m_pOutputBuffer->ClearRW();
}

void CPickingShader::Excute()
{
	if (nullptr == m_pHeightMap || nullptr == m_pOutputBuffer)
		return;

	// 상수 데이터 전달
	static const CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_info);
	pMtrlBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_COMPUTE);

	int iThreadNum = m_info.arrInt[0] * 2 * m_info.arrInt[1];
	Dispatch(iThreadNum / 1024 + 1, 1, 1);
}
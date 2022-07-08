#include "pch.h"
#include "CCopyBoneMatrixShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CCopyBoneMatrixShader::CCopyBoneMatrixShader()
	: m_tInfo{}
	, m_pSrcBuffer(nullptr)
	, m_pDestBuffer(nullptr)
{
}

CCopyBoneMatrixShader::~CCopyBoneMatrixShader()
{
}

void CCopyBoneMatrixShader::UpdateData()
{
	// 필요한 상수값 전달	
	static const CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_tInfo);
	pMtrlBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_COMPUTE);

	// 구조화버퍼 전달
	m_pSrcBuffer->UpdateData(14);
	m_pDestBuffer->UpdateDataRW(0);
}

void CCopyBoneMatrixShader::Clear()
{
	// 전달한 구조화버퍼 클리어	
	m_pSrcBuffer->Clear();
	m_pDestBuffer->ClearRW();
}

void CCopyBoneMatrixShader::Excute()
{
	int iBoneCount = m_tInfo.arrInt[0];
	int iThreadX = iBoneCount / 1024 + 1;
	Dispatch(iThreadX, 1, 1);
}
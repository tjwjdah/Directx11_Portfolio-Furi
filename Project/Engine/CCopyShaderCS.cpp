#include "pch.h"
#include "CCopyShaderCS.h"

#include "CDevice.h"
#include "CConstBuffer.h"


void CCopyShaderCS::UpdateData()
{
	if (nullptr != m_pDestTex)
	{
		m_pDestTex->UpdateDataRW(0);

		static const CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
		tMtrlInfo info = {};
		info.arrVec2[0] = Vec2(m_pDestTex->Width(), m_pDestTex->Height());
		pMtrlBuffer->SetData(&info);
		pMtrlBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_COMPUTE);
	}	

	if(nullptr != m_pSrcTex)
		m_pSrcTex->UpdateData(13, (UINT)PIPELINE_STAGE::PS_COMPUTE);
}

void CCopyShaderCS::Clear()
{
	CTexture::ClearRW(0);
	CTexture::Clear(13, (UINT)PIPELINE_STAGE::PS_COMPUTE);	
}

void CCopyShaderCS::Excute()
{
	// 32, 32, 1 그룹 스레드
	UINT iWidth = m_pDestTex->Width();
	UINT iHeight = m_pDestTex->Height();

	Dispatch(iWidth / 32 + 1, iHeight / 32 + 1, 1);
}

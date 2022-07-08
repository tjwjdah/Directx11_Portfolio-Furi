#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CCopyBoneMatrixShader :
    public CComputeShader
{
private:
	// g_int_0 : BonCount
	// g_int_1 : RowIndex
	tMtrlInfo			m_tInfo;
	CStructuredBuffer*	m_pSrcBuffer;
	CStructuredBuffer*	m_pDestBuffer;

public:
	void SetBoneCount(UINT _iBoneCount) { m_tInfo.arrInt[0] = _iBoneCount; }
	void SetRowIndex(UINT _iRowIdx) { m_tInfo.arrInt[1] = _iRowIdx; }
	void SetSourceBuffer(CStructuredBuffer* _pBuffer) { m_pSrcBuffer = _pBuffer; }
	void SetDestBuffer(CStructuredBuffer* _pBuffer) { m_pDestBuffer = _pBuffer; }

public:
	virtual void UpdateData();
	virtual void Clear();
	virtual void Excute();

public:
	CCopyBoneMatrixShader();
	virtual ~CCopyBoneMatrixShader();
};


#pragma once
#include "CComputeShader.h"

#include "Ptr.h"
#include "CTexture.h"

class CStructuredBuffer;

class CWeightShader :
    public CComputeShader
{
private:
	tMtrlInfo			m_tInfo;
	Ptr<CTexture>		m_pWeightTex;	// OutputTex : u1
	Ptr<CTexture>		m_pBrushTexArr; // t8
	CStructuredBuffer*	m_pPickInfo;	// t19

	float				m_fBrushScale;	// g_float_0 
	Vec2				m_vWeightTexResolution; // g_vec2_0

	UINT				m_iTileIdx;		// g_int_0
	UINT				m_iBrushIdx;	// g_int_1

public:
	void SetWeightTex(Ptr<CTexture> _pTex) 
	{
		m_pWeightTex = _pTex; 
		if (nullptr != m_pWeightTex)
		{
			m_vWeightTexResolution = Vec2(m_pWeightTex->Width(), m_pWeightTex->Height());
		}
	}
	void SetBrushTex(Ptr<CTexture> _pBrushTexArr) { m_pBrushTexArr = _pBrushTexArr; }
	void SetBrushPos(CStructuredBuffer* _pPickInfo) { m_pPickInfo = _pPickInfo; }
	void SetBrushScale(float _fScale) { m_fBrushScale = _fScale; }
	void SetTileIdx(UINT _iIdx) { m_iTileIdx = _iIdx; }
	void SetBrushIdx(UINT _iIdx) { m_iBrushIdx = _iIdx; }	

public:
	virtual void UpdateData();
	virtual void Clear();
	virtual void Excute();

	CLONE_DISABLE(CWeightShader);

public:
	CWeightShader();
	virtual ~CWeightShader();
};


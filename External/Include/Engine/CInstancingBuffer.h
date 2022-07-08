#pragma once

#include "CSingleton.h"
#include "CStructuredBuffer.h"
#include "CTexture.h"

struct tInstancingData
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
	int    iRowIdx;
};

class CTexture;
class CStructuredBuffer;
class CCopyBoneMatrixShader;


class CInstancingBuffer
	: public CSingleton<CInstancingBuffer>
{
	SINGLE(CInstancingBuffer);

private:
	ComPtr<ID3D11Buffer>		m_pInstancingBuffer;
	UINT						m_iMaxCount;
	vector<tInstancingData>		m_vecData;

	vector<CStructuredBuffer*>	m_vecBoneMat;
	int							m_iAnimInstCount;
	CStructuredBuffer*			m_pBoneBuffer;

	Ptr<CCopyBoneMatrixShader>	m_pCopyShader;

public:
	void init();
	void Clear() { m_vecData.clear(); m_vecBoneMat.clear(); m_iAnimInstCount = 0; }
	void AddInstancingData(tInstancingData& _tData) { m_vecData.push_back(_tData); }

	UINT GetInstanceCount() { return (UINT)m_vecData.size(); }
	ComPtr<ID3D11Buffer> GetBuffer() { return m_pInstancingBuffer; }

	void AddInstancingBoneMat(CStructuredBuffer* _pBuffer);
	void SetData();

	int GetAnimInstancingCount() { return m_iAnimInstCount; };
	CStructuredBuffer* GetBoneBuffer() { return m_pBoneBuffer; }

private:
	void Resize(UINT _iCount);


};


#pragma once

#include "CShader.h"

#include "CTexture.h"

class CComputeShader
	: public CShader
{
private:
	ComPtr<ID3DBlob>			m_pCSBlob;
	ComPtr<ID3D11ComputeShader>	m_pCS;

public:
	void CreateComputeShader(const wstring& _strRelativePath, const char* _strFuncName);	
	virtual void Excute() { assert(nullptr); };

private:
	// 필요 자원 바인딩(업데이트)
	virtual void UpdateData() = 0;
	virtual void Clear() = 0;

public:
	void Dispatch(UINT _x, UINT _y, UINT  _z);

public:
	CComputeShader();
	~CComputeShader();
};


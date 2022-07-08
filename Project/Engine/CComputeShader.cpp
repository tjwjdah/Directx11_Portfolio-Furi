#include "pch.h"
#include "CComputeShader.h"

#include "CDevice.h"
#include "CPathMgr.h"

CComputeShader::CComputeShader()
	: m_pCS(nullptr)
	, m_pCSBlob(nullptr)
{
}

CComputeShader::~CComputeShader()
{
}

void CComputeShader::CreateComputeShader(const wstring& _strRelativePath, const char* _strFuncName)
{
	wstring strPath = CPathMgr::GetResPath();
	strPath += _strRelativePath;

	ComPtr<ID3DBlob> errBlob;
	HRESULT hr = D3DCompileFromFile(strPath.c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName, "cs_5_0"
		, D3DCOMPILE_DEBUG, 0
		, &m_pCSBlob, errBlob.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBoxA(nullptr, (char*)errBlob->GetBufferPointer() , "Shader Compile Error !!", MB_OK);
	}

	DEVICE->CreateComputeShader(m_pCSBlob->GetBufferPointer(), m_pCSBlob->GetBufferSize(), nullptr, m_pCS.GetAddressOf());
}

void CComputeShader::Dispatch(UINT _x, UINT _y, UINT _z)
{
	UpdateData();
	
	CONTEXT->CSSetShader(m_pCS.Get(), nullptr, 0);
	CONTEXT->Dispatch(_x, _y, _z);

	Clear();
}


#include "pch.h"
#include "CGraphicsShader.h"

#include "CDevice.h"
#include "CPathMgr.h"

vector<D3D11_INPUT_ELEMENT_DESC> CGraphicsShader::g_vecLayoutDesc;
int CGraphicsShader::g_iOffset = 0;
int CGraphicsShader::g_iOffsetInst = 0;

CGraphicsShader::CGraphicsShader()
	: m_RSType(RS_TYPE::CULL_BACK)
	, m_BlendType(BLEND_TYPE::DEFAULT)
	, m_DSType(DS_TYPE::LESS)
	, m_ePOV(SHADER_POV::FORWARD)
	, m_eTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	assert(nullptr);
}

CGraphicsShader::CGraphicsShader(SHADER_POV _ePOV)
	: m_RSType(RS_TYPE::CULL_BACK)
	, m_BlendType(BLEND_TYPE::DEFAULT)
	, m_DSType(DS_TYPE::LESS)
	, m_ePOV(_ePOV)
	, m_eTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

CGraphicsShader::~CGraphicsShader()
{
}

void CGraphicsShader::CreateVertexShader(const wstring& _strRelativePath, const char* _strFuncName)
{
	// Vertex Shader 만들기	
	wstring strPath = CPathMgr::GetResPath();
	strPath += _strRelativePath;

	// Vertex Shader 컴파일	
	ComPtr<ID3DBlob> errBlob;

	HRESULT hr = D3DCompileFromFile(strPath.c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName, "vs_5_0"
		, D3DCOMPILE_DEBUG, 0
		, m_vsBlob.GetAddressOf(), errBlob.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBoxA(nullptr, (char*)errBlob->GetBufferPointer(), "Shader Compile Error !!", MB_OK);
	}

	DEVICE->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), nullptr, m_pVS.GetAddressOf());


	// Instancing 용 VertexShader Create
	{
		string strVSInst = _strFuncName;
		strVSInst += "_Inst";

		HRESULT hr = D3DCompileFromFile(strPath.c_str(), nullptr
			, D3D_COMPILE_STANDARD_FILE_INCLUDE
			, strVSInst.c_str(), "vs_5_0"
			, D3DCOMPILE_DEBUG, 0
			, m_vsInstBlob.GetAddressOf(), errBlob.GetAddressOf());

		if (FAILED(hr))
		{
			// MessageBoxA(nullptr, (char*)errBlob->GetBufferPointer(), "Shader Compile Error !!", MB_OK);
		}
		else
		{
			DEVICE->CreateVertexShader(m_vsInstBlob->GetBufferPointer(), m_vsInstBlob->GetBufferSize(), nullptr, m_pVSInst.GetAddressOf());
		}
	}

	
	// CreateInputLayout
	if (m_vsInstBlob.Get())
	{
		hr = DEVICE->CreateInputLayout(g_vecLayoutDesc.data(), (UINT)g_vecLayoutDesc.size()
			, m_vsInstBlob->GetBufferPointer(), m_vsInstBlob->GetBufferSize()
			, m_pLayout.GetAddressOf());
	}
	else
	{
		hr = DEVICE->CreateInputLayout(g_vecLayoutDesc.data(), (UINT)g_vecLayoutDesc.size()
			, m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize()
			, m_pLayout.GetAddressOf());
	}


	if (FAILED(hr))
		assert(nullptr);
}

void CGraphicsShader::CreateHullShader(const wstring& _strRelativePath, const char* _strFuncName)
{
	// Hull Shader 만들기	
	wstring strPath = CPathMgr::GetResPath();
	strPath += _strRelativePath;

	// Hull Shader 컴파일	
	ComPtr<ID3DBlob> errBlob;

	HRESULT hr = D3DCompileFromFile(strPath.c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName, "hs_5_0"
		, D3DCOMPILE_DEBUG, 0
		, &m_hsBlob, errBlob.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBoxA(nullptr, (char*)errBlob->GetBufferPointer(), "Shader Compile Error !!", MB_OK);
	}

	DEVICE->CreateHullShader(m_hsBlob->GetBufferPointer(), m_hsBlob->GetBufferSize(), nullptr, m_pHS.GetAddressOf());

}

void CGraphicsShader::CreateDomainShader(const wstring& _strRelativePath, const char* _strFuncName)
{
	// Domain Shader 만들기	
	wstring strPath = CPathMgr::GetResPath();
	strPath += _strRelativePath;

	// Domain Shader 컴파일	
	ComPtr<ID3DBlob> errBlob;

	HRESULT hr = D3DCompileFromFile(strPath.c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName, "ds_5_0"
		, D3DCOMPILE_DEBUG, 0
		, &m_dsBlob, errBlob.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBoxA(nullptr, (char*)errBlob->GetBufferPointer(), "Shader Compile Error !!", MB_OK);
	}

	DEVICE->CreateDomainShader(m_dsBlob->GetBufferPointer(), m_dsBlob->GetBufferSize(), nullptr, m_pDS.GetAddressOf());
}

void CGraphicsShader::CreateGeometryShader(const wstring& _strRelativePath, const char* _strFuncName)
{
	wstring strPath = CPathMgr::GetResPath();
	strPath += _strRelativePath;

	ComPtr<ID3DBlob> errBlob;
	HRESULT hr = D3DCompileFromFile(strPath.c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName, "gs_5_0"
		, D3DCOMPILE_DEBUG, 0
		, &m_gsBlob, errBlob.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBoxA(nullptr, (char*)errBlob->GetBufferPointer(), "Shader Compile Error !!", MB_OK);
	}

	DEVICE->CreateGeometryShader(m_gsBlob->GetBufferPointer(), m_gsBlob->GetBufferSize(), nullptr, m_pGS.GetAddressOf());
}

void CGraphicsShader::CreatePixelShader(const wstring& _strRelativePath, const char* _strFuncName)
{
	wstring strPath = CPathMgr::GetResPath();
	strPath += _strRelativePath;

	ComPtr<ID3DBlob> errBlob;
	HRESULT hr = D3DCompileFromFile(strPath.c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName, "ps_5_0"
		, D3DCOMPILE_DEBUG, 0
		, &m_psBlob, errBlob.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBoxA(nullptr, (char*)errBlob->GetBufferPointer(), "Shader Compile Error !!", MB_OK);
	}

	DEVICE->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), nullptr, m_pPS.GetAddressOf());
}

void CGraphicsShader::UpdateData()
{
}

void CGraphicsShader::UpdateData(int _iInstancing)
{
	// Topology(위상구조) 설정
	CONTEXT->IASetPrimitiveTopology(m_eTopology);

	// Shader 전달
	if (_iInstancing)
		CONTEXT->VSSetShader(m_pVSInst.Get(), nullptr, 0);
	else
		CONTEXT->VSSetShader(m_pVS.Get(), nullptr, 0);
		
	CONTEXT->HSSetShader(m_pHS.Get(), nullptr, 0);
	CONTEXT->DSSetShader(m_pDS.Get(), nullptr, 0);
	CONTEXT->GSSetShader(m_pGS.Get(), nullptr, 0);
	CONTEXT->PSSetShader(m_pPS.Get(), nullptr, 0);

	// input layout 정보 전달
	CONTEXT->IASetInputLayout(m_pLayout.Get());

	// Rasterizer State 전달	
	CONTEXT->RSSetState(CDevice::GetInst()->GetRS(m_RSType).Get());

	// BlendState 전달
	ComPtr<ID3D11BlendState> pBS = CDevice::GetInst()->GetBlendState(m_BlendType);
	CONTEXT->OMSetBlendState(pBS.Get(), nullptr, 0xffffffff);

	// Depth Stencil State 전달
	ComPtr<ID3D11DepthStencilState> pDSS = CDevice::GetInst()->GetDepthStencilState(m_DSType);
	CONTEXT->OMSetDepthStencilState(pDSS.Get(), 0);
}

void CGraphicsShader::AddInputLayout(const char* _pSemanticName, int _iSemanticIdx, DXGI_FORMAT _eFormat, bool _bInstancing)
{
	D3D11_INPUT_ELEMENT_DESC LayoutDesc = {};
	LayoutDesc.SemanticName = _pSemanticName;
	LayoutDesc.SemanticIndex = _iSemanticIdx;	
	LayoutDesc.Format = _eFormat;
	LayoutDesc.InputSlot = 0;

	if (false == _bInstancing)
	{
		LayoutDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		LayoutDesc.InstanceDataStepRate = 0;

		LayoutDesc.AlignedByteOffset = g_iOffset;
		g_iOffset += GetSizeofFormat(_eFormat);
	}
	else
	{
		LayoutDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc.InstanceDataStepRate = 1;
		LayoutDesc.InputSlot = 1;

		LayoutDesc.AlignedByteOffset = g_iOffsetInst;
		g_iOffsetInst += GetSizeofFormat(_eFormat);
	}	

	g_vecLayoutDesc.push_back(LayoutDesc);
}
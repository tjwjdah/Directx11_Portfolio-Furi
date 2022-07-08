#include "pch.h"
#include "CMRT.h"

#include "CDevice.h"

CMRT::CMRT()
	: m_arrRTTex{}
	, m_arrClearColor{}
	, m_tViewPort{}
	, m_iRTCount(0)
{
}

CMRT::~CMRT()
{
}

void CMRT::Create(Ptr<CTexture> _arrTex[8], Vec4 _arrClearColor[8], Ptr<CTexture> _pDSTex)
{
	m_iRTCount = 0;
	for (int i = 0; i < 8; ++i)
	{
		if (nullptr == _arrTex[i] && 0 == m_iRTCount)
		{
			m_iRTCount = i;
		}

		m_arrRTTex[i] = _arrTex[i];
		m_arrClearColor[i] = _arrClearColor[i];
	}	

	m_pDSTex = _pDSTex;


	m_tViewPort.Width = (float)_arrTex[0]->Width();
	m_tViewPort.Height = (float)_arrTex[0]->Height();
	m_tViewPort.MinDepth = 0.f;
	m_tViewPort.MaxDepth = 1.f;
	m_tViewPort.TopLeftX = 0.f;
	m_tViewPort.TopLeftY = 0.f;
}

void CMRT::OMSet(bool _bUsePrevDepth)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	UINT iRTCount = 0;
	for (; iRTCount < 8; ++iRTCount)
	{
		if (nullptr == m_arrRTTex[iRTCount])
			break;

		arrRTV[iRTCount] = m_arrRTTex[iRTCount]->GetRTV().Get();
	}

	ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
	if (nullptr != m_pDSTex)
	{
		pDSV = m_pDSTex->GetDSV().Get();
	}
	else if(_bUsePrevDepth)
	{
		CONTEXT->OMGetRenderTargets( 0, nullptr, pDSV.GetAddressOf());		
	}
	
	CONTEXT->OMSetRenderTargets(iRTCount, arrRTV, pDSV.Get());
	CONTEXT->RSSetViewports(1, &m_tViewPort);;
}

void CMRT::Clear()
{
	for (int i = 0; i < 8; ++i)
	{
		if (nullptr == m_arrRTTex[i])
			break;

		CONTEXT->ClearRenderTargetView(m_arrRTTex[i]->GetRTV().Get(), m_arrClearColor[i]);
	}	

	if (nullptr != m_pDSTex)
		CONTEXT->ClearDepthStencilView(m_pDSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

#pragma once

#include "CSingleton.h"

#include "CTexture.h"

class CConstBuffer;

class CDevice
	: public CSingleton<CDevice>
{
	SINGLE(CDevice);
private:
	ComPtr<ID3D11Device>			m_pDevice;		// GUP memory 관리 및 객체 생성 용도
	ComPtr<ID3D11DeviceContext>		m_pContext;		// GPU 를 이용한 렌더링 관련 명령 수행
	ComPtr<IDXGISwapChain>			m_pSwapChain;	// 렌더링용 타겟 텍스쳐 관리, back, front 버퍼 지정

	Ptr<CTexture>					m_pRTTex;
	Ptr<CTexture>					m_pDSTex;
	
	D3D11_VIEWPORT					m_tViewPort;
	ComPtr<ID3D11SamplerState>		m_pSamplerState;

	HWND							m_hOutputWindowHwnd;
	POINT							m_ptBufferResolution;
	bool							m_bWindow;

	ComPtr<ID3D11SamplerState>		m_arrSam[2];

	CConstBuffer*					m_arrCB[(UINT)CB_TYPE::END];

	ComPtr<ID3D11RasterizerState>   m_arrRS[(UINT)RS_TYPE::END];
	ComPtr<ID3D11BlendState>		m_arrBS[(UINT)BLEND_TYPE::END];
	ComPtr<ID3D11DepthStencilState> m_arrDSS[(UINT)DS_TYPE::END];


public:
	int init(HWND _hWnd, POINT _ptResolution, bool _bWindow);

public:
	ComPtr<ID3D11Device> GetDevice() { return m_pDevice; }
	ComPtr<ID3D11DeviceContext> GetContext() { return m_pContext; }

	const CConstBuffer* GetCB(CB_TYPE _eType) { return m_arrCB[(UINT)_eType]; }
	POINT GetBufferResolution() { return m_ptBufferResolution; }

	ComPtr<ID3D11BlendState> GetBlendState(BLEND_TYPE _eType) { return m_arrBS[(UINT)_eType]; }
	ComPtr<ID3D11DepthStencilState> GetDepthStencilState(DS_TYPE _eType) { return m_arrDSS[(UINT)_eType]; }
	ComPtr<ID3D11RasterizerState> GetRS(RS_TYPE _eType) { return m_arrRS[(UINT)_eType]; }

	ComPtr<IDXGISwapChain> GetSwapChain() { return m_pSwapChain; }

	void Present() { m_pSwapChain->Present(0, 0); }

	Ptr<CTexture> GetRenderTargetTex() { return m_pRTTex; }

	void SetRTTex(CTexture* _Tex) { m_pRTTex = _Tex; }
	void SetDSTex(CTexture* _Tex) { m_pDSTex = _Tex; }
	
private:
	int  CreateSwapChain();	
	void CreateViewPort();	
	void CreateConstBuffer();
	void CreateSamplerState();
	void CreateRSState();
	void CreateBlendState();
	void CreateDepthStencilState();
};


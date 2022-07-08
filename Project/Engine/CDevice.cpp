#include "pch.h"
#include "CDevice.h"

#include "CCore.h"
#include "CConstBuffer.h"

#include "CResMgr.h"

CDevice::CDevice()
	: m_tViewPort{}
	, m_hOutputWindowHwnd(nullptr)
	, m_ptBufferResolution{}
	, m_bWindow(true)
	, m_arrCB{}
{
}

CDevice::~CDevice()
{		
	// 상수버퍼 해제
	for (UINT i = 0; i < (UINT)CB_TYPE::END; ++i)
	{
		if (nullptr != m_arrCB[i])
			delete m_arrCB[i];
	}
}

int CDevice::init(HWND _hWnd, POINT _ptResolution, bool _bWindow)
{
	m_hOutputWindowHwnd = _hWnd;
	m_ptBufferResolution = _ptResolution;
	m_bWindow = _bWindow;

	g_global.vResolution = Vec2((float)m_ptBufferResolution.x, (float)m_ptBufferResolution.y);


	int iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
		
	D3D_FEATURE_LEVEL eFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE
		, nullptr, iFlag, 0, 0
		, D3D11_SDK_VERSION
		, &m_pDevice, &eFeatureLevel
		, &m_pContext)))
	{
		return E_FAIL;
	}

	//m_pDevice->AddRef();

	// 멀티샘플 레벨체크
	UINT iMultiSampleLevel = 0; // out
	m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &iMultiSampleLevel);

	if (iMultiSampleLevel <= 0)	
		return E_FAIL;
	
	// SwapChain 생성
	if (FAILED(CreateSwapChain()))
		return E_FAIL;
	
	// ViewPort
	CreateViewPort();

	// Create Sampler
	CreateSamplerState();

	// ConstBuffer
	CreateConstBuffer();

	// Create Rasterizer, Blend, DS State
	CreateRSState();
	CreateBlendState();
	CreateDepthStencilState();

	return S_OK;
}

int CDevice::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC tDesc = {};

	// 출력 윈도우, 출력 모드(전체화면 모드 or 윈도우 모드)
	tDesc.OutputWindow = m_hOutputWindowHwnd;
	tDesc.Windowed = m_bWindow;

	// 버퍼 해상도 및 포맷
	tDesc.BufferDesc.Width = m_ptBufferResolution.x;
	tDesc.BufferDesc.Height = m_ptBufferResolution.y;
	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	tDesc.BufferCount = 1; // 백버퍼 수,

	// 화면 갱신비율
	tDesc.BufferDesc.RefreshRate.Denominator = 1;
	tDesc.BufferDesc.RefreshRate.Numerator = 60;

	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	
	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	// front, back 버퍼 전환 시 장면 유지 필요 없음
	tDesc.Flags = 0;

	// Anti-Aliasing
	// Deferred Rendering 으로 인한 자체구현 필요
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;

	ComPtr<IDXGIDevice> pDXGIDevice = nullptr;
	ComPtr<IDXGIAdapter> pAdapter = nullptr;
	ComPtr<IDXGIFactory> pFactory = nullptr;

	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	HRESULT hr = pFactory->CreateSwapChain(m_pDevice.Get(), &tDesc, &m_pSwapChain);
		
	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}



void CDevice::CreateViewPort()
{
	POINT ptWinRes = CCore::GetInst()->GetWndResolution();

	m_tViewPort.Width = (float)ptWinRes.x;
	m_tViewPort.Height = (float)ptWinRes.y;
	m_tViewPort.MinDepth = 0.f;
	m_tViewPort.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &m_tViewPort);
}

void CDevice::CreateConstBuffer()
{
	m_arrCB[(UINT)CB_TYPE::TRANSFORM] = new CConstBuffer;
	m_arrCB[(UINT)CB_TYPE::TRANSFORM]->Create(CB_TYPE::TRANSFORM, sizeof(tTransform));

	m_arrCB[(UINT)CB_TYPE::MATERIAL] = new CConstBuffer;
	m_arrCB[(UINT)CB_TYPE::MATERIAL]->Create(CB_TYPE::MATERIAL, sizeof(tMtrlInfo));

	m_arrCB[(UINT)CB_TYPE::GLOBAL_VALUE] = new CConstBuffer;
	m_arrCB[(UINT)CB_TYPE::GLOBAL_VALUE]->Create(CB_TYPE::GLOBAL_VALUE, sizeof(tGlobalValue));
}

void CDevice::CreateSamplerState()
{
	D3D11_SAMPLER_DESC tDesc = {};

	tDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	DEVICE->CreateSamplerState(&tDesc, m_arrSam[0].GetAddressOf());

	tDesc = {};
	tDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	DEVICE->CreateSamplerState(&tDesc, m_arrSam[1].GetAddressOf());

	ID3D11SamplerState* arrSam[2] = { m_arrSam[0].Get(), m_arrSam[1].Get() };

	CONTEXT->VSSetSamplers(0, 2, arrSam);
	CONTEXT->HSSetSamplers(0, 2, arrSam);
	CONTEXT->DSSetSamplers(0, 2, arrSam);
	CONTEXT->GSSetSamplers(0, 2, arrSam);
	CONTEXT->PSSetSamplers(0, 2, arrSam);
	CONTEXT->CSSetSamplers(0, 2, arrSam);
}

void CDevice::CreateRSState()
{
	m_arrRS[(UINT)RS_TYPE::CULL_BACK] = nullptr;

	D3D11_RASTERIZER_DESC tDesc = {};
	tDesc.CullMode = D3D11_CULL_FRONT;
	tDesc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&tDesc, m_arrRS[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

	tDesc = {};
	tDesc.CullMode = D3D11_CULL_NONE;
	tDesc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&tDesc, m_arrRS[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

	tDesc = {};
	tDesc.CullMode = D3D11_CULL_NONE;
	tDesc.FillMode = D3D11_FILL_WIREFRAME;
	DEVICE->CreateRasterizerState(&tDesc, m_arrRS[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());
}

void CDevice::CreateBlendState()
{
	m_arrBS[(UINT)BLEND_TYPE::DEFAULT] = nullptr;

	D3D11_BLEND_DESC tDesc = {};
	tDesc.AlphaToCoverageEnable = true;	
	tDesc.IndependentBlendEnable = false;	
	tDesc.RenderTarget[0].BlendEnable = true;
	tDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlend =  D3D11_BLEND_SRC_ALPHA;
	tDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	tDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	
	tDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DEVICE->CreateBlendState(&tDesc, m_arrBS[(UINT)BLEND_TYPE::ALPHABLEND].GetAddressOf());	
		
	tDesc.AlphaToCoverageEnable = false;
	tDesc.IndependentBlendEnable = false;
	tDesc.RenderTarget[0].BlendEnable = true;
	tDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	tDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	tDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	tDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DEVICE->CreateBlendState(&tDesc, m_arrBS[(UINT)BLEND_TYPE::ALPHA_ONE].GetAddressOf());

	tDesc.AlphaToCoverageEnable = false;
	tDesc.IndependentBlendEnable = false;
	tDesc.RenderTarget[0].BlendEnable = true;
	tDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	tDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	tDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	tDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	tDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DEVICE->CreateBlendState(&tDesc, m_arrBS[(UINT)BLEND_TYPE::ONE_ONE].GetAddressOf());
}

void CDevice::CreateDepthStencilState()
{
	m_arrDSS[(UINT)DS_TYPE::LESS] = nullptr;

	D3D11_DEPTH_STENCIL_DESC tDesc = {};

	// Less Equal
	tDesc.StencilEnable = false;
	tDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	tDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	tDesc.DepthEnable = true;
	DEVICE->CreateDepthStencilState(&tDesc, m_arrDSS[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());

	// Greater
	tDesc.StencilEnable = false;
	tDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	tDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	tDesc.DepthEnable = true;
	DEVICE->CreateDepthStencilState(&tDesc, m_arrDSS[(UINT)DS_TYPE::GREATER].GetAddressOf());

	// Greater Equal
	tDesc.StencilEnable = false;
	tDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	tDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	tDesc.DepthEnable = true;
	DEVICE->CreateDepthStencilState(&tDesc, m_arrDSS[(UINT)DS_TYPE::GREATER_EQUAL].GetAddressOf());

	// No Test
	tDesc.StencilEnable = false;
	tDesc.DepthEnable = true;					// 깊이 옵션 사용
	tDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;	// 항상 통과(테스트 x)
	tDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 깊이는 기록
	DEVICE->CreateDepthStencilState(&tDesc, m_arrDSS[(UINT)DS_TYPE::NO_TEST].GetAddressOf());

	// Less, No Write	
	tDesc.StencilEnable = false;
	tDesc.DepthEnable = true;
	tDesc.DepthFunc = D3D11_COMPARISON_LESS;
	tDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DEVICE->CreateDepthStencilState(&tDesc, m_arrDSS[(UINT)DS_TYPE::LESS_NO_WRITE].GetAddressOf());
	
	// No Test, No Write	
	tDesc.StencilEnable = false;
	tDesc.DepthEnable = false;	
	DEVICE->CreateDepthStencilState(&tDesc, m_arrDSS[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());
}

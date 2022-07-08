#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CResMgr.h"

#include "CMRT.h"

void CRenderMgr::CreateMRT()
{
	// ======================
	// SwapChain RenderTarget
	// ======================
	// SwapChain 의 백버퍼를 포인터로 참조한다.	
	ComPtr<ID3D11Texture2D> pTex2D = nullptr;
	ComPtr<IDXGISwapChain> pSwapChain = CDevice::GetInst()->GetSwapChain();

	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pTex2D.GetAddressOf());
	Ptr<CTexture> pRTTex = CResMgr::GetInst()->CreateTexture(L"SwapChainRenderTargetTex", pTex2D, true);
	assert(pRTTex.Get());
	CDevice::GetInst()->SetRTTex(pRTTex.Get());
	// DepthStencl Texture 생성
	Vec2 vResolution = Vec2((float)CDevice::GetInst()->GetBufferResolution().x, (float)CDevice::GetInst()->GetBufferResolution().y);
	Ptr<CTexture> pDepthTex = CResMgr::GetInst()->CreateTexture(L"DepthStencilTex", (UINT)vResolution.x, (UINT)vResolution.y
		, D3D11_BIND_DEPTH_STENCIL, DXGI_FORMAT_D24_UNORM_S8_UINT, true);
	CDevice::GetInst()->SetDSTex(pRTTex.Get());
	Ptr<CTexture> arrTex[8] = { pRTTex, };
	Vec4 arrClearColor[8] = { Vec4(0.f, 0.f, 0.f, 0.f), };

	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT;
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->SetName(L"SwapChain");
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(arrTex, arrClearColor, pDepthTex);

	// =========
	// Light MRT
	// =========
	{
		Ptr<CTexture> pDiffuseLight = CResMgr::GetInst()->CreateTexture(L"DiffuseLightTargetTex", (UINT)vResolution.x, (UINT)vResolution.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM, true);

		Ptr<CTexture> pSpecularLight = CResMgr::GetInst()->CreateTexture(L"SpecularLightTargetTex", (UINT)vResolution.x, (UINT)vResolution.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM, true);

		Ptr<CTexture> arrTex[8] = { pDiffuseLight, pSpecularLight };
		Vec4 arrClearColor[8] = { Vec4(0.f, 0.f, 0.f, 0.f),  Vec4(0.f, 0.f, 0.f, 0.f), };

		m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->SetName(L"Lights");
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(arrTex, arrClearColor, nullptr);
	}


	// ============
	// Deferred MRT
	// ============
	{
		Ptr<CTexture> pDiffuseTex = CResMgr::GetInst()->CreateTexture(L"DiffuseTargetTex", (UINT)vResolution.x, (UINT)vResolution.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM, true);

		Ptr<CTexture> pNormalTex = CResMgr::GetInst()->CreateTexture(L"NormalTargetTex", (UINT)vResolution.x, (UINT)vResolution.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32G32B32A32_FLOAT, true);

		Ptr<CTexture> pPositionTex = CResMgr::GetInst()->CreateTexture(L"PositionTargetTex", (UINT)vResolution.x, (UINT)vResolution.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32G32B32A32_FLOAT, true);

		Ptr<CTexture> pDiffuseLight = CResMgr::GetInst()->FindDataTexture(L"DiffuseLightTargetTex");

		Ptr<CTexture> arrTex[8] = { pDiffuseTex, pNormalTex, pPositionTex, pDiffuseLight };
		Vec4 arrClearColor[8] = { Vec4(0.f, 0.f, 0.f, 0.f),  Vec4(0.f, 0.f, 0.f, 0.f),  Vec4(0.f, 0.f, 0.f, 0.f),  Vec4(0.f, 0.f, 0.f, 0.f) };

		m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->SetName(L"Deferred");
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Create(arrTex, arrClearColor, nullptr);
	}



	// LightMergeMtrl 파라미터 셋팅
	Ptr<CMaterial> pLightMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"LightMergeMtrl");
	pLightMergeMtrl->SetData(SHADER_PARAM::TEX_0, CResMgr::GetInst()->FindDataTexture(L"DiffuseTargetTex").Get());
	pLightMergeMtrl->SetData(SHADER_PARAM::TEX_1, CResMgr::GetInst()->FindDataTexture(L"DiffuseLightTargetTex").Get());
	pLightMergeMtrl->SetData(SHADER_PARAM::TEX_2, CResMgr::GetInst()->FindDataTexture(L"SpecularLightTargetTex").Get());


	// ==============
	// ShadowMap MRT
	// ==============
	{
		//POINT ptShadowMapResolution = { 16384, 16384 };
		POINT ptShadowMapResolution = { 4096, 4096 };
		Ptr<CTexture> pShadowMapTex = CResMgr::GetInst()->CreateTexture(L"ShadowMapTargetTex"
			, ptShadowMapResolution.x, ptShadowMapResolution.y
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R32_FLOAT, true);

		Ptr<CTexture> pShadowMapDepthTex = CResMgr::GetInst()->CreateTexture(L"ShadowmapDepthTex"
			, ptShadowMapResolution.x, ptShadowMapResolution.y
			, D3D11_BIND_DEPTH_STENCIL
			, DXGI_FORMAT_D32_FLOAT, true);

		Ptr<CTexture> arrTex[8] = { pShadowMapTex , };
		Vec4 arrClearColor[8] = { Vec4(0.f, 0.f, 0.f, 0.f), };

		m_arrMRT[(UINT)MRT_TYPE::DYNAMIC_SHADDOWMAP] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::DYNAMIC_SHADDOWMAP]->Create(arrTex, arrClearColor, pShadowMapDepthTex);
	}
}

void CRenderMgr::ClearMRT()
{
	for (UINT i = 0; i < (UINT)MRT_TYPE::END; ++i)
	{
		if (nullptr != m_arrMRT[i])
		{
			m_arrMRT[i]->Clear();
		}
	}
}

#include "pch.h"
#include "MRTGUI.h"

#include <Engine\CResMgr.h>
#include <Engine\CRenderMgr.h>


MRTGUI::MRTGUI()
	: m_pCopyMtrl(nullptr)	
	, m_iRenderCount(0)
	, m_pShader(nullptr)
{
}

MRTGUI::~MRTGUI()
{
	SAFE_DELETE(m_pCopyMtrl);
	SAFE_DELETE(m_pShader);
}


void MRTGUI::init()
{
	m_pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		
	for (UINT i = (UINT)MRT_TYPE::DEFERRED; i < (UINT)MRT_TYPE::END; ++i)
	{
		if (nullptr == CRenderMgr::GetInst()->GetMRT((MRT_TYPE)i))
			continue;

		UINT targetCount = CRenderMgr::GetInst()->GetMRT((MRT_TYPE)i)->GetRTCount();

		for (UINT j = 0; j < targetCount; ++j)
		{
			wchar_t szName[20] = L"";
			wsprintf(szName, L"MRT_CopyTex_%d", m_vecTex.size());

			m_vecTex.push_back(CResMgr::GetInst()->CreateTexture(szName, 200, 200
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
				, DXGI_FORMAT_R8G8B8A8_UNORM, true));
		}
	}	

	m_pDepthTex = CResMgr::GetInst()->CreateTexture(L"MRTGUIDepthTex", 200, 200
		, D3D11_BIND_DEPTH_STENCIL, DXGI_FORMAT_D24_UNORM_S8_UINT, true);

	// =============================
	// Textrue DownScale Copy Shader
	// =============================	
	m_pShader = new CGraphicsShader(SHADER_POV::FORWARD);
	m_pShader->CreateVertexShader(L"shader\\tool.fx", "VS_CopyTex");
	m_pShader->CreatePixelShader(L"shader\\tool.fx", "PS_CopyTex");
	m_pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);


	m_pCopyMtrl = new CMaterial;
	m_pCopyMtrl->SetDefault();
	m_pCopyMtrl->SetShader(m_pShader);
}

void MRTGUI::update()
{
	
}

void MRTGUI::render()
{
	bool bOpen = true;	
	ImGui::Begin("MRTGUI", &bOpen);

	m_iRenderCount = 0;

	// SwapChain MRT

	// Deferred MRT
	render_MRT(MRT_TYPE::DEFERRED);

	// Light MRT
	render_MRT(MRT_TYPE::LIGHT);

	//
	render_MRT(MRT_TYPE::DYNAMIC_SHADDOWMAP);

	if (false == bOpen)
	{
		DeActivate();
	}

	ImGui::End();
}

void MRTGUI::render_MRT(MRT_TYPE _eType)
{
	CMRT* pMRT = CRenderMgr::GetInst()->GetMRT(_eType);

	Ptr<CTexture> arrTex[8] = {};
	for (UINT i = 0; i < pMRT->GetRTCount(); ++i)
	{
		arrTex[i] = pMRT->GetRTTex(i);
	}
	
	CopyTex(arrTex, pMRT->GetRTCount());

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	ImGui::Text(GetString(pMRT->GetName()));

	for (UINT i = 0; i < pMRT->GetRTCount(); ++i)
	{
		render_tex(m_vecTex[i + m_iRenderCount]);

		if (i != pMRT->GetRTCount() - 1)
			ImGui::SameLine();
	}
	ImGui::Text("");

	m_iRenderCount += pMRT->GetRTCount();
}

void MRTGUI::CopyTex(Ptr<CTexture> _arrTex[8], int _iCopyCount)
{
	for (int i = 0; i < _iCopyCount; ++i)
	{
		Ptr<CTexture> arrTargetTex[8] = { m_vecTex[i + m_iRenderCount]};
		Vec4 v[8] = {};

		m_mrt.Create(arrTargetTex, v, m_pDepthTex);
		m_mrt.OMSet();

		m_pCopyMtrl->SetData(SHADER_PARAM::TEX_0, _arrTex[i].Get());

		m_pCopyMtrl->UpdateData();

		m_pRectMesh->UpdateData(0);
		m_pRectMesh->render(0);

		m_pCopyMtrl->Clear();
	}
}

void MRTGUI::render_tex(Ptr<CTexture> _pTex)
{	
	ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
	ImVec2 teszie = ImVec2(200.f, 200.f);				// Texture render size
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
	ImGui::Image(_pTex->GetSRV().Get(), teszie, uv_min, uv_max, tint_col, border_col);
}

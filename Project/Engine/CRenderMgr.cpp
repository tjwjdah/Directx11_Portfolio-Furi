#include "pch.h"
#include "CRenderMgr.h"

#include "CResMgr.h"
#include "CTimeMgr.h"
#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"
#include "CCamera.h"
#include "CLight3D.h"

#include "CScene.h"
#include "CSceneMgr.h"

#include "CMRT.h"

CRenderMgr::CRenderMgr()
	: m_iCurNoise(0)
	, m_fAccTime(0.f)
	, m_pLight2DBuffer(nullptr)
	, m_pLight3DBuffer(nullptr)
	, m_pToolCam(nullptr)
	, m_pAnim2DBuffer(nullptr)
	, m_arrMRT{}
{
}

CRenderMgr::~CRenderMgr()
{
	SAFE_DELETE(m_pLight2DBuffer);
	SAFE_DELETE(m_pLight3DBuffer);
	SAFE_DELETE(m_pAnim2DBuffer);
	Safe_Delete_Array(m_arrMRT);
}

void CRenderMgr::init()
{
	Ptr<CTexture> pTex = nullptr;
	pTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\noise\\noise_01.png", L"texture\\noise\\noise_01.png");
	m_vecNoiseTex.push_back(pTex);
	/*pTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\noise\\noise_02.png", L"texture\\noise\\noise_02.png");
	m_vecNoiseTex.push_back(pTex);*/

	g_global.vNoiseResolution = Vec2(pTex->Width(), pTex->Height());

	// Light StructuredBuffer Create
	m_pLight2DBuffer = new CStructuredBuffer;
	m_pLight2DBuffer->Create(sizeof(tLightInfo), 1, nullptr);

	m_pLight3DBuffer = new CStructuredBuffer;
	m_pLight3DBuffer->Create(sizeof(tLightInfo), 1, nullptr);

	// Animation 2D Buffer
	m_pAnim2DBuffer = new CStructuredBuffer;
	m_pAnim2DBuffer->Create(sizeof(tAnim2D), 1, nullptr);

	// CopyTarget Texture Create
	Vec2 vResolution = Vec2((float)CDevice::GetInst()->GetBufferResolution().x, (float)CDevice::GetInst()->GetBufferResolution().y);
	m_pCopyTarget = CResMgr::GetInst()->CreateTexture(L"PostEffectTargetTex"
		, (UINT)vResolution.x, (UINT)vResolution.y
		, D3D11_BIND_SHADER_RESOURCE
		, DXGI_FORMAT_R8G8B8A8_UNORM, true);

	// PostEffectMtrl 에 CopyTexture 전달
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_ripple");
	pMtrl->SetData(SHADER_PARAM::TEX_0, m_pCopyTarget.Get());

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_GlichLine");
	pMtrl->SetData(SHADER_PARAM::TEX_0, m_pCopyTarget.Get());

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_filter");
	pMtrl->SetData(SHADER_PARAM::TEX_0, m_pCopyTarget.Get());

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_Colorfilter");
	pMtrl->SetData(SHADER_PARAM::TEX_0, m_pCopyTarget.Get());

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_blur");
	pMtrl->SetData(SHADER_PARAM::TEX_0, m_pCopyTarget.Get());

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_Vignette");
	pMtrl->SetData(SHADER_PARAM::TEX_0, m_pCopyTarget.Get());


	// MRT 생성
	CreateMRT();
}

void CRenderMgr::update()
{
	// =================
	// Noise Texture 교체
	// =================
	m_fAccTime += fDT;
	if (m_fAccTime > 0.1f)
	{
		m_fAccTime = 0.f;
		m_iCurNoise += 1;
		if (m_vecNoiseTex.size() == m_iCurNoise)
			m_iCurNoise = 0;
	}

	m_vecNoiseTex[m_iCurNoise]->UpdateData(70, (UINT)PIPELINE_STAGE::PS_ALL);

	// ===================
	// 광원 데이터 업데이트
	// ===================
	g_global.iLight2DCount = (UINT)m_vecLight2D.size();
	g_global.iLight3DCount = (UINT)m_vecLight3D.size();

	vector<tLightInfo> vecLight[2];
	for (size_t i = 0; i < m_vecLight2D.size(); ++i)
	{
		vecLight[0].push_back(m_vecLight2D[i]->GetInfo());
	}
	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		vecLight[1].push_back(m_vecLight3D[i]->GetInfo());
	}

	UpdateLights(vecLight[0], m_pLight2DBuffer, 61);
	UpdateLights(vecLight[1], m_pLight3DBuffer, 62);

	m_vecLight2D.clear();
	m_vecLight3D.clear();

	// ==================
	// 전역 데이터 업데이트
	// ==================
	static const CConstBuffer* pGlobalBuffer = CDevice::GetInst()->GetCB(CB_TYPE::GLOBAL_VALUE);
	pGlobalBuffer->SetData(&g_global);
	pGlobalBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_ALL);
}

void CRenderMgr::render()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	if (nullptr == pCurScene)
		return;

	// MRT 클리어
	ClearMRT();

	if (SCENE_STATE::PLAY == pCurScene->GetState())
	{
		render_play();
		render_ui();  //******************************************** 추가
	}
	else
	{
		render_tool();
	}
	if (m_vecCam.size() > 0)
		CurCamera = m_vecCam[0];
	m_vecCam.clear();
}

void CRenderMgr::render_play()
{
	GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		m_vecCam[i]->SortObject();

		GetMRT(MRT_TYPE::DEFERRED)->OMSet();
		m_vecCam[i]->render_deferred();

		CRenderMgr::GetInst()->render_shadowmap();

		// Lighting
		GetMRT(MRT_TYPE::LIGHT)->OMSet();

		// main camera 기준 view, proj 전달
		g_transform.matView = m_vecCam[i]->GetViewMat();
		g_transform.matViewInv = m_vecCam[i]->GetViewInvMat();
		g_transform.matProj = m_vecCam[i]->GetProjMat();

		for (size_t i = 0; i < m_vecLight3D.size(); ++i)
		{
			m_vecLight3D[i]->render();
		}

		GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

		LightMerge();
		m_vecCam[i]->render_forward();
	}

	GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		m_vecCam[i]->render_posteffect();
	}
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		m_vecCam[i]->render_collider();
	}
}

void CRenderMgr::render_ui()   //******************************************** 추가
{
	if (nullptr == m_uicam)
		return;

	GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	// 물체 분류
	m_uicam->SortUIObject();

	//// Deferred 물체 그리기
	//GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	//m_uicam->render_deferred();

	//// ShadowMap
	//CRenderMgr::GetInst()->render_shadowmap();

	//// Lighting
	//GetMRT(MRT_TYPE::LIGHT)->OMSet();

	// main camera 기준 view, proj 전달
	g_transform.matView = m_uicam->GetViewMat();
	g_transform.matViewInv = m_uicam->GetViewInvMat();
	g_transform.matProj = m_uicam->GetProjMat();

	//for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	//{
	//	m_vecLight3D[i]->render();
	//}

	// 물체 색상, Lights Merge
	//GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	//LightMerge();

	// forward 물체 그리기
	GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	//m_uicam->render_forward();
	m_uicam->render_ui();
	//m_pToolCam->render_posteffect();
}


void CRenderMgr::render_tool()
{
	if (nullptr == m_pToolCam)
		return;

	GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	// 물체 분류
	m_pToolCam->SortObject();

	// Deferred 물체 그리기
	GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	m_pToolCam->render_deferred();

	// ShadowMap
	CRenderMgr::GetInst()->render_shadowmap();

	// Lighting
	GetMRT(MRT_TYPE::LIGHT)->OMSet();

	// main camera 기준 view, proj 전달
	g_transform.matView = m_pToolCam->GetViewMat();
	g_transform.matViewInv = m_pToolCam->GetViewInvMat();
	g_transform.matProj = m_pToolCam->GetProjMat();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		m_vecLight3D[i]->render();
	}

	// 물체 색상, Lights Merge
	GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	LightMerge();

	// forward 물체 그리기
	GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	m_pToolCam->render_forward();
	m_pToolCam->render_posteffect();
}


void CRenderMgr::render_shadowmap()
{
	m_arrMRT[(UINT)MRT_TYPE::DYNAMIC_SHADDOWMAP]->OMSet();

	// 광원 시점으로 깊이를 그림
	for (UINT i = 0; i < m_vecLight3D.size(); ++i)
	{
		if (m_vecLight3D[i]->GetInfo().eType != LIGHT_TYPE::DIR)
			continue;

		m_vecLight3D[i]->render_shadowmap();
	}

	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet(); // 메인카메라 깊이로 되돌리기 위해서
}




void CRenderMgr::CopyTarget()
{
	CONTEXT->CopyResource(m_pCopyTarget->GetTex2D().Get(), CDevice::GetInst()->GetRenderTargetTex()->GetTex2D().Get());
}


CCamera* CRenderMgr::GetCurCam()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	if (pCurScene->GetState() == SCENE_STATE::PLAY)
	{
		if (m_vecCam.empty())
			return nullptr;

		return m_vecCam[0];
	}
	else
	{
		return m_pToolCam;
	}
}

void CRenderMgr::LightMerge()
{
	static Ptr<CMesh> pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	static Ptr<CMaterial> pMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"LightMergeMtrl");

	pMergeMtrl->UpdateData();

	pRectMesh->UpdateData(0);
	pRectMesh->render(0);

	pMergeMtrl->Clear();
}

void CRenderMgr::UpdateLights(vector<tLightInfo>& _vec, CStructuredBuffer* _buffer, UINT _iRegisterNum)
{
	if (_buffer->GetElementCount() < _vec.size())
		_buffer->Create(sizeof(tLightInfo), (UINT)_vec.size(), nullptr);

	_buffer->SetData(_vec.data(), sizeof(tLightInfo), (UINT)_vec.size());
	_buffer->UpdateData(_iRegisterNum);
}

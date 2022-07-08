#include "pch.h"
#include "CToolObjMgr.h"

#include <Engine\CGraphicsShader.h>
#include <Engine\CMaterial.h>

#include <Engine\CRenderMgr.h>
#include <Engine\CTransform.h>

#include "CGameObjectEx.h"
#include "CCameraEx.h"
#include "CToolCamScript.h"
#include "CGridScript.h"



CToolObjMgr::CToolObjMgr()
{

}

CToolObjMgr::~CToolObjMgr()
{
	Safe_Delete_Vector(m_vecToolObj);

	SAFE_DELETE(m_pGridShader);
	SAFE_DELETE(m_pGridMtrl);
}

void CToolObjMgr::init()
{
	// ToolCamera Object
	CGameObjectEx* pToolCam = new CGameObjectEx;
	pToolCam->AddComponent(new CTransform);
	pToolCam->AddComponent(new CCameraEx);
	pToolCam->AddComponent(new CToolCamScript);

	pToolCam->Transform()->SetLocalPos(Vec3(0.f, 1000.f, 0.f));
	pToolCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pToolCam->Camera()->SetFar(100000.f);
	pToolCam->Camera()->SetLayerAllCheck();

	m_vecToolObj.push_back(pToolCam);
	CRenderMgr::GetInst()->RegisterToolCamera(pToolCam->Camera());

	// Tool Grid Object

	m_pGridShader = new CGraphicsShader(SHADER_POV::TOOL);	
	m_pGridShader->CreateVertexShader(L"shader\\tool.fx", "VS_Grid");
	m_pGridShader->CreatePixelShader(L"shader\\tool.fx", "PS_Grid");
	m_pGridShader->SetBlendType(BLEND_TYPE::ALPHABLEND);
	m_pGridShader->SetRSType(RS_TYPE::CULL_NONE);
	
	m_pGridMtrl = new CMaterial();
	m_pGridMtrl->SetDefault();
	m_pGridMtrl->SetShader(m_pGridShader);

	CGameObjectEx* pGridObj = new CGameObjectEx;
	pGridObj->AddComponent(new CTransform);
	pGridObj->AddComponent(new CMeshRender);

	CGridScript* pGridScript = new CGridScript;
	pGridScript->SetToolCamera(pToolCam);	
	pGridScript->SetGridColor(Vec3(1.f, 0.2f, 0.2f));
	pGridObj->AddComponent(pGridScript);

	pGridObj->Transform()->SetLocalScale(Vec3(100000.f, 100000.f, 1.f));
	pGridObj->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));
	pGridObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pGridObj->MeshRender()->SetMaterial(m_pGridMtrl, 0);

	m_vecToolObj.push_back(pGridObj);
}

void CToolObjMgr::progress()
{
	for (size_t i = 0; i < m_vecToolObj.size(); ++i)
	{
		m_vecToolObj[i]->update();
	}

	for (size_t i = 0; i < m_vecToolObj.size(); ++i)
	{
		m_vecToolObj[i]->lateupdate();
	}

	for (size_t i = 0; i < m_vecToolObj.size(); ++i)
	{
		m_vecToolObj[i]->finalupdate();		
	}
}

void CToolObjMgr::render()
{	
	for (size_t i = 0; i < m_vecToolObj.size(); ++i)
	{
		if(nullptr != m_vecToolObj[i]->MeshRender())
			m_vecToolObj[i]->MeshRender()->render();
	}
}

#include "pch.h"
#include "CTerrain.h"

#include "CRenderMgr.h"
#include "CResMgr.h"
#include "CKeyMgr.h"

#include "CGameObject.h"
#include "CMeshRender.h"
#include "CTransform.h"
#include "CCamera.h"

#include "CStructuredBuffer.h"

CTerrain::CTerrain()
	: CComponent(COMPONENT_TYPE::TERRAIN)
	, m_iXFace(0)
	, m_iZFace(0)
	, m_fBrushScale(0.3f)
	, m_fMaxTess(5.f)// 2^n  32
{
	m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TerrainMtrl");
	assert(m_pMtrl.Get());

	// 필요한 타일 로딩
	LoadTileTex();

	// 필요한 Brush Texture 로딩
	LoadBrushTex();

	CreateComputeShader();

	//m_pMtrl->SetData(SHADER_PARAM::TEX_0, m_vecTile[0].Get());
	//m_pMtrl->SetData(SHADER_PARAM::TEX_1, m_vecTile_N[0].Get());
	m_pMtrl->SetData(SHADER_PARAM::TEXARR_0, m_pTileTex.Get());
	m_pMtrl->SetData(SHADER_PARAM::TEXARR_1, m_pTileNTex.Get());
	m_pMtrl->SetData(SHADER_PARAM::TEX_0, m_pHeightMapTex.Get());	

	m_pMtrl->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);
}

CTerrain::~CTerrain()
{
	delete m_pPickingPosBuffer;
}

void CTerrain::finalupdate()
{
	CCamera* pCurCam = CRenderMgr::GetInst()->GetCurCam();

	Vec4 vCamWorldPos;
	vCamWorldPos = pCurCam->Transform()->GetWorldPos();
	m_pMtrl->SetData(SHADER_PARAM::VEC4_0, &vCamWorldPos);

	if (KEY_HOLD(KEY_TYPE::SPACE))
	{		
		if (nullptr == pCurCam)
			return;
		
		// World Ray 정보를 Terrain 의 로컬로 보낸다
		tRay ray = pCurCam->GetRay();
		const Matrix& matWorldInv = Transform()->GetWorldInvMat();
		
		ray.vPoint = XMVector3TransformCoord(ray.vPoint, matWorldInv);
		ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
		ray.vDir.Normalize();

		m_csPicking->SetRayPos(ray.vPoint);
		m_csPicking->SetRayDir(ray.vDir);
		m_csPicking->SetFaceCount(m_iXFace, m_iZFace);		
		m_csPicking->SetHeightMap(m_pHeightMapTex);
		m_csPicking->SetOutputBuffer(m_pPickingPosBuffer);
		m_csPicking->Excute();

		/*m_csHeightMap->SetHeightMode(1);
		m_csHeightMap->SetBrushTex(m_vecBrush[0]);
		m_csHeightMap->SetCenterPos(m_pPickingPosBuffer);
		m_csHeightMap->SetBrushScale(m_fBrushScale);
		m_csHeightMap->Excute();*/

		m_csWeightMap->SetTileIdx(3); //m_csWeightMap->SetTileIdx(1); m_csWeightMap->SetTileIdx(0); 
		m_csWeightMap->Excute();
	}
}

void CTerrain::render()
{
	if (nullptr == m_pMesh)
		return;

	Transform()->UpdateData();	
	m_pMtrl->UpdateData();

	m_pMesh->UpdateData(0);
	m_pMesh->render(0);

	m_pMtrl->Clear();
}

void CTerrain::SetFace(UINT _x, UINT _z)
{
	if (m_iXFace == _x && m_iZFace == _z)
	{
		return;
	}

	m_iXFace = _x;
	m_iZFace = _z;		

	m_pMtrl->SetData(SHADER_PARAM::INT_0, &m_iXFace);
	m_pMtrl->SetData(SHADER_PARAM::INT_1, &m_iZFace);

	CreateTerrainMesh();
	CreateWeightMapTexture();
}

void CTerrain::SetHeightMap(Ptr<CTexture> _pHeightMap)
{
	m_pHeightMapTex = _pHeightMap;
	if (m_pHeightMapTex.Get())
	{
		Vec2 vResolution = Vec2((float)m_pHeightMapTex->Width(), (float)m_pHeightMapTex->Height());
		m_pMtrl->SetData(SHADER_PARAM::VEC2_0, &vResolution);
		m_pMtrl->SetData(SHADER_PARAM::TEX_2, m_pHeightMapTex.Get());
	}
}

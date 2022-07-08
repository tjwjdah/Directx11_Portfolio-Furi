#include "pch.h"
#include "CTerrain.h"

#include "CResMgr.h"
#include "CStructuredBuffer.h"
#include "CHeightMapShader.h"
#include "CWeightShader.h"
#include "CResMgr.h"

void CTerrain::LoadTileTex()
{
	Ptr<CTexture> pTileTex = CResMgr::GetInst()->FindRes<CTexture>(L"TILE_01");
	if (pTileTex.Get())
		m_vecTile.push_back(pTileTex);
	else
	{
		m_vecTile.push_back(CResMgr::GetInst()->Load<CTexture>(L"TILE_01", L"texture\\tile\\TILE_01.tga"));
	}

	pTileTex = CResMgr::GetInst()->FindRes<CTexture>(L"TILE_02");
	if (pTileTex.Get())
		m_vecTile.push_back(pTileTex);
	else
	{
		m_vecTile.push_back(CResMgr::GetInst()->Load<CTexture>(L"TILE_02", L"texture\\tile\\TILE_02.tga"));
	}

	pTileTex = CResMgr::GetInst()->FindRes<CTexture>(L"TILE_03");
	if (pTileTex.Get())
		m_vecTile.push_back(pTileTex);
	else
	{
		m_vecTile.push_back(CResMgr::GetInst()->Load<CTexture>(L"TILE_03", L"texture\\tile\\TILE_03.tga"));
	}

	pTileTex = CResMgr::GetInst()->FindRes<CTexture>(L"TILE_01_N");
	if (pTileTex.Get())
		m_vecTile_N.push_back(pTileTex);
	else
	{
		m_vecTile_N.push_back(CResMgr::GetInst()->Load<CTexture>(L"TILE_01_N", L"texture\\tile\\TILE_01_N.tga"));
	}

	pTileTex = CResMgr::GetInst()->FindRes<CTexture>(L"TILE_02_N");
	if (pTileTex.Get())
		m_vecTile_N.push_back(pTileTex);
	else
	{
		m_vecTile_N.push_back(CResMgr::GetInst()->Load<CTexture>(L"TILE_02_N", L"texture\\tile\\TILE_02_N.tga"));
	}

	pTileTex = CResMgr::GetInst()->FindRes<CTexture>(L"TILE_03_N");
	if (pTileTex.Get())
		m_vecTile_N.push_back(pTileTex);
	else
	{
		m_vecTile_N.push_back(CResMgr::GetInst()->Load<CTexture>(L"TILE_03_N", L"texture\\tile\\TILE_03_N.tga"));
	}

	// TileTexture 를 배열텍스쳐로 만들기
	//m_pTileTex = new CTexture;
	//m_pTileTex->CreateArrayTexture(m_vecTile);
	//m_pTileTex->Save();

	//m_pTileNTex = new CTexture;
	//m_pTileNTex->CreateArrayTexture(m_vecTile_N);
	//m_pTileNTex->Save();*/

	m_pTileTex = CResMgr::GetInst()->Load<CTexture>(L"TileArrTex", L"texture\\tile\\TILE_ARR.dds");
	m_pTileNTex = CResMgr::GetInst()->Load<CTexture>(L"TileNArrTex", L"texture\\tile\\TILE_ARR_N.dds");
	m_pBrushTex = CResMgr::GetInst()->Load<CTexture>(L"BrushArrTex", L"texture\\brush\\BrushArr.dds");


}

void CTerrain::LoadBrushTex()
{
	Ptr<CTexture> pBrushTex = CResMgr::GetInst()->FindRes<CTexture>(L"BRUSH_01");
	if (pBrushTex.Get())
		m_vecBrush.push_back(pBrushTex);
	else
	{
		m_vecBrush.push_back(CResMgr::GetInst()->Load<CTexture>(L"BRUSH_01", L"texture\\brush\\Brush_01.png"));
	}

	pBrushTex = CResMgr::GetInst()->FindRes<CTexture>(L"BRUSH_02");
	if (pBrushTex.Get())
		m_vecBrush.push_back(pBrushTex);
	else
	{
		m_vecBrush.push_back(CResMgr::GetInst()->Load<CTexture>(L"BRUSH_02", L"texture\\brush\\Brush_02.png"));
	}
}

void CTerrain::CreateTerrainMesh()
{
	vector<VTX> vecVtx;
	vector<UINT> vecIdx;

	VTX v;

	for (int i = 0; i < (int)m_iZFace + 1; ++i)
	{
		for (int j = 0; j < (int)m_iXFace + 1; ++j)
		{
			v.vPos = Vec3((float)j, 0.f, (float)i);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);
			v.vUV = Vec2((float)j, (float)(m_iZFace - i));
			vecVtx.push_back(v);
		}
	}

	for (int i = 0; i < (int)m_iZFace; ++i)
	{
		for (int j = 0; j < (int)m_iXFace; ++j)
		{
			vecIdx.push_back((m_iXFace + 1) * (i + 1) + j);
			vecIdx.push_back((m_iXFace + 1) * (i)+(j + 1));
			vecIdx.push_back((m_iXFace + 1) * (i)+j);

			vecIdx.push_back((m_iXFace + 1) * (i)+(j + 1));
			vecIdx.push_back((m_iXFace + 1) * (i + 1) + j);
			vecIdx.push_back((m_iXFace + 1) * (i + 1) + (j + 1));
		}
	}

	if (nullptr == m_pMesh)
	{
		m_pMesh = new CMesh;
		m_pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
		CResMgr::GetInst()->AddRes<CMesh>(L"TerrainMesh", m_pMesh);
	}
	else
	{
		m_pMesh->Reset(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	}

	
}

void CTerrain::CreateWeightMapTexture()
{
	// 가중치 텍스쳐
	if (nullptr != m_pWeightMapTex)
	{
		m_pWeightMapTex->Create((UINT)((float)m_iXFace * pow(2.f, m_fMaxTess))
			, (UINT)((float)m_iZFace * pow(2.f, m_fMaxTess))
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
			, DXGI_FORMAT_R32G32B32A32_FLOAT);
	}
	else
	{
		m_pWeightMapTex = CResMgr::GetInst()->CreateTexture(L"WeightMapTex"
			, (UINT)((float)m_iXFace * pow(2.f, m_fMaxTess))
			, (UINT)((float)m_iZFace * pow(2.f, m_fMaxTess))
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
			, DXGI_FORMAT_R32G32B32A32_FLOAT, false);
	}

	// 가중치 쉐이더에 WeightTex 알림
	m_csWeightMap->SetWeightTex(m_pWeightMapTex);
	m_pMtrl->SetData(SHADER_PARAM::TEX_1, m_pWeightMapTex.Get());
}


void CTerrain::CreateComputeShader()
{
	// Picking 위치 출력용 버퍼
	m_pPickingPosBuffer = new CStructuredBuffer;
	m_pPickingPosBuffer->CreateRW(sizeof(Vec4), 1, nullptr);
	
	// HeightMap Shader
	m_csHeightMap = (CHeightMapShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"HeightMapShader").Get();
		
	// Weight Shader	
	m_csWeightMap = (CWeightShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"WeightMapShader").Get();
		
	m_csWeightMap->SetBrushTex(m_pBrushTex);
	m_csWeightMap->SetBrushScale(m_fBrushScale);
	m_csWeightMap->SetBrushPos(m_pPickingPosBuffer);
	m_csWeightMap->SetBrushIdx(0);
	m_csWeightMap->SetTileIdx(0);

	// Picking Shader
	m_csPicking = (CPickingShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"PickingShader").Get();
}

void CTerrain::CreateHeightMap(UINT _iWidth, UINT _iHeight)
{
	m_pHeightMapTex = CResMgr::GetInst()->FindDataTexture(L"TerrainHeightMap");
	if (nullptr != m_pHeightMapTex)
	{
		// 이미 높이맵 텍스쳐가 있지만, 지정된 해상도랑 다른경우(해상도 변경 요청)
		if (m_pHeightMapTex->Width() != _iWidth && m_pHeightMapTex->Height() != _iHeight)
		{
			m_pHeightMapTex->Create(_iWidth, _iHeight
				, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
				, DXGI_FORMAT_R32_FLOAT);
		}
	}
	else
	{
		m_pHeightMapTex = CResMgr::GetInst()->CreateTexture(L"TerrainHeightMap", _iWidth, _iHeight
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
			, DXGI_FORMAT_R32_FLOAT, true);
	}

	// 생성시킨 Texture 정보를 Material 에 전달
	Vec2 vResolution = Vec2((float)m_pHeightMapTex->Width(), (float)m_pHeightMapTex->Height());
	m_pMtrl->SetData(SHADER_PARAM::VEC2_0, &vResolution);
	m_pMtrl->SetData(SHADER_PARAM::TEX_2, m_pHeightMapTex.Get());

	// 생성시킨 높이맵 Texture 를 ComputeShader 에 전달
	m_csHeightMap->SetHeightMap(m_pHeightMapTex);
}
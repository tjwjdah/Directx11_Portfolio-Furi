#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"
#include "CTexture.h"

#include "CHeightMapShader.h"
#include "CWeightShader.h"
#include "CPickingShader.h"

class CStructuredBuffer;

class CTerrain :
    public CComponent
{
private:
    UINT                    m_iXFace;
    UINT                    m_iZFace;

    vector<Ptr<CTexture>>   m_vecTile;
    vector<Ptr<CTexture>>   m_vecTile_N;
    vector<Ptr<CTexture>>   m_vecBrush;    

    Ptr<CTexture>           m_pTileTex;
    Ptr<CTexture>           m_pTileNTex;
    Ptr<CTexture>           m_pBrushTex;

    Ptr<CTexture>           m_pHeightMapTex;
    Ptr<CTexture>           m_pWeightMapTex;

    Ptr<CMesh>              m_pMesh;
    Ptr<CMaterial>          m_pMtrl;

    float                   m_fBrushScale;
    float                   m_fMaxTess; // 2^n  32

    // ComputeShader
    Ptr<CHeightMapShader>   m_csHeightMap;
    Ptr<CWeightShader>      m_csWeightMap;
    Ptr<CPickingShader>     m_csPicking;

    CStructuredBuffer*      m_pPickingPosBuffer;

public:
    virtual void finalupdate();   
    void render();

public:
    void SetFace(UINT _x, UINT _z);
    void SetHeightMap(Ptr<CTexture> _pHeightMap);
    void CreateHeightMap(UINT _iWidth, UINT _iHeight);

private:
    void CreateTerrainMesh();
    void CreateComputeShader();
    void CreateWeightMapTexture();
    void LoadTileTex();
    void LoadBrushTex();

public:
    CLONE(CTerrain);

public:
    CTerrain();
    ~CTerrain();
};


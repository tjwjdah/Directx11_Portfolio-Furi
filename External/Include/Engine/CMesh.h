#pragma once
#include "CRes.h"

#include "CFBXLoader.h"

class CStructuredBuffer;

struct tIndexInfo
{
    ComPtr<ID3D11Buffer> pIB;
    D3D11_BUFFER_DESC    tIBDesc;
    UINT				 iIdxCount;       
    void*                pIdxSysMem;
};


class CMesh :
    public CRes
{
private:  
    ComPtr<ID3D11Buffer>    m_pVB;
    D3D11_BUFFER_DESC       m_tVBDesc;
    UINT                    m_iVtxCount;
    void*                   m_pVtxSysMem;

    // 하나의 버텍스버퍼에 여러개의 인덱스버퍼가 연결
    vector<tIndexInfo>		m_vecIdxInfo;
   
    // Animation3D 정보
    vector<tMTAnimClip>		m_vecAnimClip;
    vector<vector<tMTBone>>	m_vecBones;

    vector<CStructuredBuffer*>      m_pBoneFrameData;   // 전체 본 프레임 정보
    vector<CStructuredBuffer*>      m_pBoneOffset;	    // 각 뼈의 offset 행렬
    vector<UINT>                    i_FrameCount;


    // Nav Mesh Data
    bool                    m_bIsNavMesh; // 네브 메쉬 여부
    vector<NavMesh>         m_vecNavData;
public:
    static CMesh* CreateFromContainer(CFBXLoader& _loader, int _num = 0, bool _Nav = false);

    void Create(VTX* _pVTXSysmem, UINT _iVtxCount, UINT* _IdxSysmem, UINT _iIdxCount);    
    void Reset(VTX* _pVTXSysmem, UINT _iVtxCount, UINT* _IdxSysmem, UINT _iIdxCount);
    void UpdateData(UINT _iSubset);
    void render(UINT _iSubset);
    void render_particle(UINT _iParticleCount);
    void render_instancing(UINT _iSubset);
  

public:
    UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }
    const vector<tMTBone>* GetBones(UINT _i) { return &m_vecBones[_i]; }
    UINT GetBoneCount(UINT _i) { return (UINT)m_vecBones[_i].size(); }
    UINT GetBoneSize() { return m_vecBones.size(); }
    const vector<tMTAnimClip>* GetAnimClip() { return &m_vecAnimClip; }
    bool IsAnimMesh() { return !m_vecAnimClip.empty(); }

    CStructuredBuffer* GetBoneFrameDataBuffer(UINT _i) { return m_pBoneFrameData[_i]; } // 전체 본 프레임 정보
    CStructuredBuffer* GetBoneOffsetBuffer(UINT _i) { return  m_pBoneOffset[_i]; }	   // 각 뼈의 offset 행렬	

    bool    IsNavMesh() { return m_bIsNavMesh;}
    vector<NavMesh>* GetNavData() { return &m_vecNavData; }
    void NavMeshDataTrans(Matrix _TransData);
public:
    virtual void Save(const wstring& _strRelativePath);
    virtual void Load(const wstring& _strFilePath);

public:
    CMesh();
    ~CMesh();
};


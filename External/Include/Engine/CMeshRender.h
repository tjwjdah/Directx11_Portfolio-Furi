#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMeshRender :
    public CComponent
{
private:
    Ptr<CMesh>              m_pMesh;
    vector<Ptr<CMaterial>>  m_vecMtrl;	   

public:
    virtual void update();
    virtual void finalupdate();

    void render();
    void render(UINT _iMtrl);

    void render_shadowmap();
    void render_shadowmap(UINT _iMtrl);

public:
    Ptr<CMesh> GetMesh() { return m_pMesh; }
    void SetMesh(Ptr<CMesh> _pMesh);
    void SetMaterial(Ptr<CMaterial> _pMtrl, UINT _iIdx);
    UINT GetMtrlCount() { return (UINT)m_vecMtrl.size(); }
    ULONG64 GetInstID(UINT _iMtrlIdx);
    ULONG64 GetInstShadowID(UINT _iMtrlIdx);
   
    Ptr<CMaterial> GetSharedMaterial(UINT _iIdx) { return m_vecMtrl[_iIdx]; }
    Ptr<CMaterial> GetCloneMaterial(UINT _iIdx);

    void SetNavMeshTrans();
public:
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);
    CLONE(CMeshRender);

public:
    CMeshRender();
    ~CMeshRender();
};


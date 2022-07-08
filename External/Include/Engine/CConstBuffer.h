#pragma once
#include "CEntity.h"


class CConstBuffer :
    public CEntity
{
private:
    ComPtr<ID3D11Buffer>    m_pCB;
    CB_TYPE                 m_eCBType;
    UINT                    m_iBufferSize;

public:
    void Create(CB_TYPE _eType, UINT _iBufferSize);
    void SetData(void* _pData) const;
    void UpdateData(UINT _iPOV) const;

public:
    CLONE_DISABLE(CConstBuffer);
    virtual void SaveToScene(FILE* _pFile) { assert(nullptr); }
    virtual void LoadFromScene(FILE* _pFile) { assert(nullptr); }

public:
    CConstBuffer();
    ~CConstBuffer();
};


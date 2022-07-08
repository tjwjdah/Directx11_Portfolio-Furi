#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CPickingShader :
    public CComputeShader
{
private:
    tMtrlInfo           m_info;
    Ptr<CTexture>       m_pHeightMap;
    CStructuredBuffer*  m_pOutputBuffer;

public:
    void SetRayPos(Vec3 _vPos) { m_info.arrVec4[0] = _vPos; }
    void SetRayDir(Vec3 _vDir) { m_info.arrVec4[1] = _vDir; }
    void SetFaceCount(UINT _iX, UINT _iZ) { m_info.arrInt[0] = _iX; m_info.arrInt[1] = _iZ; }    
    void SetHeightMap(Ptr<CTexture> _pHeightMap) {m_pHeightMap = _pHeightMap; m_info.arrTexCheck[0] = true;}
    void SetOutputBuffer(CStructuredBuffer* _pOutputBuffer) { m_pOutputBuffer = _pOutputBuffer; }


private:
    virtual void UpdateData();
    virtual void Clear();

public:
    virtual void Excute();

public:
    CPickingShader();
    ~CPickingShader();
};


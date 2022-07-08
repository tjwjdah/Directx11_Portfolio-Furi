#pragma once
#include "CComputeShader.h"
class CCopyShaderCS :
    public CComputeShader
{
private:
    Ptr<CTexture>   m_pDestTex;
    Ptr<CTexture>   m_pSrcTex;

public:
    void SetDestTex(Ptr<CTexture> _pDestTex) { m_pDestTex = _pDestTex; }
    void SetSrcTex(Ptr<CTexture> _pSrcTex) { m_pSrcTex = _pSrcTex; }

public:
    virtual void UpdateData();
    virtual void Clear();
    virtual void Excute();


};


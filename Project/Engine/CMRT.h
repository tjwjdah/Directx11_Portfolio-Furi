#pragma once
#include "CEntity.h"

#include "CTexture.h"
#include "Ptr.h"

class CTexture;

class CMRT :
    public CEntity
{
private:
    Ptr<CTexture>   m_arrRTTex[8];
    Vec4            m_arrClearColor[8];
    Ptr<CTexture>   m_pDSTex;
    D3D11_VIEWPORT  m_tViewPort;   
    UINT            m_iRTCount;
    

public:
    void Create(Ptr<CTexture> _arrTex[8], Vec4 _arrClearColor[8], Ptr<CTexture> _pDSTex);
    void OMSet(bool _bUsePrevDepth = true);
    void Clear();

    Ptr<CTexture> GetRTTex(UINT _iRTIdx) { return m_arrRTTex[_iRTIdx]; }
    UINT GetRTCount() { return m_iRTCount; }

    void GetViewPort(D3D11_VIEWPORT* _pViewOut){*_pViewOut = m_tViewPort;}

public:
    CLONE_DISABLE(CMRT);

public:
    CMRT();
    ~CMRT();
};


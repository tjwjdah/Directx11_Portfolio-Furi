#pragma once
#include "GUI.h"

#include <Engine\CTexture.h>
#include <Engine\CMaterial.h>
#include <Engine\CMRT.h>
#include <Engine\CGraphicsShader.h>
#include <Engine\CMesh.h>

class MRTGUI :
    public GUI
{
private:
    CMaterial*              m_pCopyMtrl;
    vector<Ptr<CTexture>>   m_vecTex;
    Ptr<CTexture>           m_pDepthTex;
    UINT                    m_iRenderCount;
    Ptr<CMesh>              m_pRectMesh;

    CMRT                    m_mrt;
    CGraphicsShader*        m_pShader;

public:
    virtual void init();
    virtual void update();
    virtual void render();

private:
    void render_MRT(MRT_TYPE _eType);
    void CopyTex(Ptr<CTexture> _arrTex[8], int _iCopyCount);
    void render_tex(Ptr<CTexture> _pTex);

public:
    MRTGUI();
    ~MRTGUI();
};


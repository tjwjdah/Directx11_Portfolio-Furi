#pragma once
#include <Engine\CSingleton.h>


class  CGameObjectEx;
class CMaterial;
class CGraphicsShader;

class CToolObjMgr :
    public CSingleton<CToolObjMgr>
{
    SINGLE(CToolObjMgr);
private:
    vector<CGameObjectEx*>  m_vecToolObj;

    CMaterial*              m_pGridMtrl;
    CGraphicsShader*        m_pGridShader;

public:
    void init();
    void progress();
    void render();
};


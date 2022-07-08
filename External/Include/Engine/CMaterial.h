#pragma once
#include "CRes.h"

#include "CGraphicsShader.h"
#include "CTexture.h"

class CMaterial :
    public CRes
{
private:
    tMtrlInfo               m_tInfo;
    Ptr<CTexture>           m_arrTex[(UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0];

    Ptr<CGraphicsShader>    m_pShader;
    bool                    m_bDefault;

public:
    void SetShader(Ptr<CGraphicsShader> _pShader) { m_pShader = _pShader; }
    Ptr<CGraphicsShader> GetShader() { return m_pShader; }
    void SetData(SHADER_PARAM _eType, const void* _pData);
    void SetMaterialCoefficient(Vec4 _vDiff, Vec4 _vSpec, Vec4 _vAmb, Vec4 _vEmis)
    {
        m_tInfo.vDiff = _vDiff;
        m_tInfo.vAmb = _vAmb;
        m_tInfo.vSpec = _vSpec;
        m_tInfo.vEmis = _vEmis;
    }

    void SetAnim3D(bool _bTrue)
    {
        m_tInfo.iAnim3D[0] = (int)_bTrue;
    }
    void SetBoneCount(int _iBoneCount)
    {
        m_tInfo.iAnim3D[1] = _iBoneCount;
    }


    void SetDefault() { m_bDefault = true; }
    void* GetData(SHADER_PARAM _eType);

    void UpdateData(int _iInstancing = 0);
    void Clear();



public:
    virtual CMaterial* Clone();

public:
    virtual void Save(const wstring& _strRelativePath);
    virtual void Load(const wstring& _strFilePath);

public:
    CMaterial();
    CMaterial(const CMaterial& _origin);
    ~CMaterial();

    friend class CResMgr;
};


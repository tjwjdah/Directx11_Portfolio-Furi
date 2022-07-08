#pragma once
#include "CRes.h"


struct ShaderParamInfo
{
    SHADER_PARAM    eParamType;
    wstring         strParamName;
};


class CShader :
    public CRes
{
private:
    vector<ShaderParamInfo>   m_vecParamInfo;

public:
    void AddShaderParam(SHADER_PARAM _eParam, const wstring& _strParamName)
    {
        m_vecParamInfo.push_back(ShaderParamInfo{ _eParam, _strParamName });
    }

    const vector<ShaderParamInfo>& GetParamInfo() { return m_vecParamInfo; }


    virtual void UpdateData() = 0;    

public:
    virtual void Save(const wstring& _strRelativePath) {}
    virtual void Load(const wstring& _strFilePath) {};

public:
    CShader();    
    ~CShader();
};


#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"

#include "CPathMgr.h"

CPrefab::CPrefab()
    : m_pProtoObj(nullptr)
{
}

CPrefab::CPrefab(CGameObject* _pProto)
    : m_pProtoObj(_pProto)
{
}

CPrefab::~CPrefab()
{
    SAFE_DELETE(m_pProtoObj);
}

CGameObject* CPrefab::Instantiate()
{
    assert(m_pProtoObj);
    return m_pProtoObj->Clone();
}

void CPrefab::Save(const wstring& _strRelativePath)
{
    wstring strFilePath = CPathMgr::GetResPath();
    strFilePath += _strRelativePath;

    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
    assert(pFile);

    m_pProtoObj->SaveToScene(pFile);

    fclose(pFile);
}

void CPrefab::Load(const wstring& _strFilePath)
{
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
    assert(pFile);

    m_pProtoObj = new CGameObject;
    m_pProtoObj->LoadFromScene(pFile);

    fclose(pFile);
}

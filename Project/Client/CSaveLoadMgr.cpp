#include "pch.h"
#include "CSaveLoadMgr.h"

#include <Engine\CPathMgr.h>
#include <Engine\CSceneMgr.h>
#include <Engine\CScene.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\CComponent.h>

#include <Script\CScriptMgr.h>
#include <Script\CPlayerScript.h>

void CSaveLoadMgr::init()
{
    g_pScriptSave = &CSaveLoadMgr::SaveScript;
    g_pScriptLoad = &CSaveLoadMgr::LoadScript;
}

void CSaveLoadMgr::SaveScene(CScene* _pScene, const wstring& _strRelativePath)
{
    wstring strFilePath = CPathMgr::GetResPath();
    strFilePath += _strRelativePath;

    FILE* pFile = nullptr;
    HRESULT hr = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
    
    if (nullptr == pFile)
    {
        MessageBox(nullptr, L"Scene Save Failed", L"Error", MB_OK);
        return;
    }

    _pScene->SaveToScene(pFile);

    fclose(pFile);
}

void CSaveLoadMgr::LoadScene(CScene* _pScene, const wstring& _strRelativePath)
{
    wstring strFilePath = CPathMgr::GetResPath();
    strFilePath += _strRelativePath;

    FILE* pFile = nullptr;
    HRESULT hr = _wfopen_s(&pFile, strFilePath.c_str(), L"rb");

    if (nullptr == pFile)
    {
        MessageBox(nullptr, L"Scene Load Failed", L"Error", MB_OK);
        return;
    }

    _pScene->LoadFromScene(pFile);

    fclose(pFile);
}

void CSaveLoadMgr::SaveScript(CScript* _pScript, FILE* _pFile)
{
    _pScript->SaveToScene(_pFile);
}

CScript* CSaveLoadMgr::LoadScript(FILE* _pFile)
{
    wstring strName;
    LoadWString(strName, _pFile);

    int iScriptType = -1;
    fread(&iScriptType, sizeof(UINT), 1, _pFile);

    CScript* pScript = CScriptMgr::GetScript(iScriptType);
    pScript->LoadFromScene(_pFile);
    pScript->SetName(strName);

    return pScript;
}

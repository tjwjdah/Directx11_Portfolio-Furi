#include "pch.h"
#include "extern.h"

tTransform g_transform = {};
tGlobalValue g_global = {};
CCamera* CurCamera;

pSaveFunc g_pScriptSave = nullptr;
pLoadFunc g_pScriptLoad = nullptr;

const wchar_t* g_szResName[(UINT)RES_TYPE::END] =
{
	L"MESHDATA",
	L"PREFAB",
	L"MATERIAL",
	L"SHADER",
	L"MESH",
	L"TEXTURE",
	L"SOUND"
};


vector<tMeshRink> g_MeshRink;
vector<NavMesh>  g_vecNavData;
#pragma once

extern tTransform g_transform;
extern tGlobalValue g_global;
extern vector<tMeshRink> g_MeshRink;
class CCamera;
extern CCamera* CurCamera;
//렌더에서 퀄카메라 함수 못불러서 그냥 전역변수로 선언



class CScript;
typedef void (*pSaveFunc)(CScript*, FILE*);
typedef CScript* (*pLoadFunc)(FILE*);

extern pSaveFunc g_pScriptSave;
extern pLoadFunc g_pScriptLoad;

extern const wchar_t* g_szResName[(UINT)RES_TYPE::END];

extern vector<NavMesh>  g_vecNavData;


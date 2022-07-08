#pragma once

class CScene;
class CScript;

class CSaveLoadMgr
{
public:
	static void init();

	static void SaveScene(CScene* _pScene, const wstring& _strRelativePath);
	static void LoadScene(CScene* _pScene, const wstring& _strRelativePath);

	static void SaveScript(CScript* _pScript, FILE* _pFile);
	static CScript* LoadScript(FILE* _pFile);
};


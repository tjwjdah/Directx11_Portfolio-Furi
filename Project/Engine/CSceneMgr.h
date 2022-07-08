#pragma once

#include "CSingleton.h"

class CScene;
class CGameObject;

class CSceneMgr
	: public CSingleton<CSceneMgr>
{
	SINGLE(CSceneMgr);
private:
	CScene* m_pCurScene;
	bool    m_ColShow;
public:
	void init();
	void progress();
	
public:
	CScene* GetCurScene() { return m_pCurScene; }
	CGameObject* FindObjectByName(const wstring& _strName);

	void ChangeScene(CScene* _pNextScene);
	bool	GetColShow() { return m_ColShow; }
	friend class CEventMgr;
};


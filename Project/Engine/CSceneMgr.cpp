#include "pch.h"
#include "CSceneMgr.h"
#include "CScene.h"

#include "CKeyMgr.h"
#include "CEventMgr.h"

CSceneMgr::CSceneMgr()
	: m_pCurScene(nullptr),
	m_ColShow(true)
{
}

CSceneMgr::~CSceneMgr()
{
	SAFE_DELETE(m_pCurScene);
}

void CSceneMgr::init()
{
	m_pCurScene = new CScene;
}

void CSceneMgr::progress()
{
	if (m_pCurScene->GetState() == SCENE_STATE::PLAY)
	{
		m_pCurScene->update();
		m_pCurScene->lateupdate();
	}

	if (KEY_TAP(KEY_TYPE::ENTER))
	{
		if (m_ColShow)
			m_ColShow = false;
		else
			m_ColShow = true;
	}
	m_pCurScene->finalupdate();
}

CGameObject* CSceneMgr::FindObjectByName(const wstring& _strName)
{
	assert(m_pCurScene);

	return m_pCurScene->FindObjectByName(_strName);
}

void CSceneMgr::ChangeScene(CScene* _pNextScene)
{
	tEvent evn = {};
	evn.eEvent = EVENT_TYPE::SCENE_CHANGE;
	evn.lParam = (DWORD_PTR)m_pCurScene;
	evn.wParam = (DWORD_PTR)_pNextScene;

	CEventMgr::GetInst()->AddEvent(evn);
}

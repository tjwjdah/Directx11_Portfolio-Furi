#pragma once
#include <CScript.h>
#include <CSceneMgr.h>
#include <CScene.h>
#include <CGameObject.h>
#include <CTransform.h>

#include "CScriptMgr.h"

template<typename SCRIPT>
class CSingletonScript :
	public CScript
{
private:
	static CGameObject*	m_pTargetObj;	

public:
	static SCRIPT* GetInst()
	{
		if (nullptr == m_pTargetObj)
		{
			m_pTargetObj = CSceneMgr::GetInst()->FindObject(L"SingletonObject", 0);
			if (nullptr == m_pTargetObj)
			{
				m_pTargetObj = CScript::CreateObject(Vec3(0.f, 0.f, 0.f), 0);				
				m_pTargetObj->SetName(L"SingletonObject");				
			}
		}
		
		const vector<CScript*>& vecScript = m_pTargetObj->GetScripts();
		for (size_t i = 0; i < vecScript.size(); ++i)
		{			
			if (dynamic_cast<SCRIPT*>(vecScript[i]))
			{
				return	(SCRIPT*)vecScript[i];
			}		
		}

		SCRIPT* pMgrScript = new SCRIPT;
		m_pTargetObj->AddComponent(pMgrScript);

		return pMgrScript;
	}

public:
	CSingletonScript(SCRIPT_TYPE _eType)
		: CScript((int)_eType)
	{}
	virtual ~CSingletonScript() 
	{
		m_pTargetObj = nullptr;
	};
};

template<typename SCRIPT>
CGameObject* CSingletonScript<SCRIPT>::m_pTargetObj = nullptr;
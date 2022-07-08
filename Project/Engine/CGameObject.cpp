#include "pch.h"
#include "CGameObject.h"

#include <Script\CScriptMgr.h>
#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"

#include "CComponent.h"
#include "CMeshRender.h"
#include "CParticleSystem.h"
#include "CAnimator2D.h"
#include "CCollider2D.h"
#include "CCamera.h"
#include "CLight2D.h"
#include "CLight3D.h"
#include "CScript.h"

CGameObject::CGameObject()
	: m_arrCom{}
	, m_iLayerIdx(-1)
	, m_pParentObj(nullptr)
	, m_bFrustum(false)
	, m_bDynamicShadow(false)
	, m_bDead(false)
	, m_bUpdateRenderOn(true)
{
}


CGameObject::CGameObject(const CGameObject& _origin)
	:CEntity(_origin)
	,m_arrCom{}
	, m_vecChild()
	, m_pParentObj(nullptr)
	, m_iLayerIdx(-1)
	, m_bFrustum(_origin.m_bFrustum)
	, m_bDynamicShadow(_origin.m_bDynamicShadow)
	, m_bDead(false)
	, m_bUpdateRenderOn(_origin.m_bUpdateRenderOn)
{
	
	// 컴포넌트 복사
	for (int i = 0; i < (int)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != _origin.m_arrCom[i])
		{
			AddComponent(_origin.m_arrCom[i]->Clone());			
		}		
	}

	// Script 복사
	for (size_t i = 0; i < _origin.m_vecScript.size(); ++i)
	{
		AddComponent(_origin.m_vecScript[i]->Clone());
	}

	// 자식 오브젝트 복사
	for (size_t i = 0; i < _origin.m_vecChild.size(); ++i)
	{
		AddChild(_origin.m_vecChild[i]->Clone());
	}
}

CGameObject::~CGameObject()
{
	Safe_Delete_Array(m_arrCom);
	Safe_Delete_Vector(m_vecScript);
	Safe_Delete_Vector(m_vecChild);
}

void CGameObject::awake()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->awake();
	}	

	if (!m_bDead)
	{
		for (size_t i = 0; i < m_vecScript.size(); ++i)
		{
			m_vecScript[i]->awake();
		}
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->awake();
	}
}

void CGameObject::start()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->start();
	}

	if (!m_bDead)
	{
		for (size_t i = 0; i < m_vecScript.size(); ++i)
		{
			m_vecScript[i]->start();
		}
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->start();
	}
}

void CGameObject::update()
{
	if (!m_bUpdateRenderOn)
		return;
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->update();
	}

	if (!m_bDead)
	{
		for (size_t i = 0; i < m_vecScript.size(); ++i)
		{
			m_vecScript[i]->update();
		}
	}
	

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->update();
	}
}

void CGameObject::lateupdate()
{
	if (!m_bUpdateRenderOn)
		return;
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->lateupdate();
	}

	if (!m_bDead)
	{
		for (size_t i = 0; i < m_vecScript.size(); ++i)
		{
			m_vecScript[i]->lateupdate();
		}
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->lateupdate();
	}
}

void CGameObject::finalupdate()
{
	if (!m_bUpdateRenderOn)
		return;
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finalupdate();
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->finalupdate();
	}

	// Register
	CLayer* pCurLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(m_iLayerIdx);
	pCurLayer->RegisterObject(this);
}

void CGameObject::finalupdate_ex()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finalupdate();
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->finalupdate();
	}
}


void CGameObject::AddComponent(CComponent* _pComponent)
{
	UINT iType = (UINT)_pComponent->GetType();
		
	if (iType != (UINT)COMPONENT_TYPE::SCRIPT)
	{
		assert(!m_arrCom[iType]); // 이미 동일한 컴포넌트를 보유하고 있는 경우
		assert(!_pComponent->GetGameObject()); // 컴포넌트가 이미 다른 오브젝트에 속한 경우
		m_arrCom[iType] = _pComponent;
	}
	else
	{
		m_vecScript.push_back((CScript*)_pComponent);
	}
		
	_pComponent->SetGameObject(this);
}

CScript* CGameObject::GetScript(const wstring& _strName)
{

	for (size_t j = 0; j < m_vecScript.size(); ++j)
	{
		if (CScriptMgr::GetScriptName(m_vecScript[j]) == _strName)
			return m_vecScript[j];
	}
	return nullptr;
}
CGameObject* CGameObject::GetChild(const wstring& _strName)
{
	
	if (GetName() == _strName)
	{
		return this;
	}
	else
	{
		for (size_t i = 0; i < m_vecChild.size(); i++)
		{
			if(m_vecChild[i]->GetChild(_strName)!=nullptr)
				if(m_vecChild[i]->GetChild(_strName)->GetName() == _strName)
					return m_vecChild[i]->GetChild(_strName);
		}
		return nullptr;
	}
	
}

void CGameObject::DisconnectWithParent()
{
	// 부모가 없는 오브젝트에 부모해제 함수 호출한 경우
	assert(m_pParentObj);

	vector<CGameObject*>::iterator iter = m_pParentObj->m_vecChild.begin();
	for (; iter != m_pParentObj->m_vecChild.end(); ++iter)
	{
		if (this == (*iter))
		{
			m_pParentObj->m_vecChild.erase(iter);
			m_pParentObj = nullptr;
			return;
		}
	}
	
	// 자식은 부모를 아는데, 부모는 자식을 모르는경우
	assert(nullptr);
}

void CGameObject::RegisterAsParentObj()
{
	// 소속 Layer 가 없는 경우
	if (-1 == m_iLayerIdx)
	{
		return;
	}

	// 소속 Layer 에 부모오브젝트로 등록
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	int iLayerIdx = m_iLayerIdx;
	m_iLayerIdx = -1;
	pCurScene->AddObject(this, iLayerIdx);
}

void CGameObject::ReleaseAsParentObj()
{
	// 소속레이어가 없는 경우(Scene 에 포함된적이 없었다)
	if (-1 == m_iLayerIdx)
		return;
	
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pCurLayer = pCurScene->GetLayer(m_iLayerIdx);	

	vector<CGameObject*>::iterator iter = pCurLayer->m_vecParentObj.begin();
	for (; iter != pCurLayer->m_vecParentObj.end(); ++iter)
	{
		if (this == (*iter))
		{
			pCurLayer->m_vecParentObj.erase(iter);			
			return;
		}
	}

	// 부모오브젝트가 소속 레이어에 등록이 안되어 있었다 --> 에러
	assert(nullptr);
}

void CGameObject::AddChild(CGameObject* _pChildObj)
{
	// 1. 최상위 부모 오브젝트가 자식으로 들어오는 경우
	if (nullptr == _pChildObj->GetParent())
	{
		_pChildObj->ReleaseAsParentObj();
	}

	// 2. 다른 부모가 있는 오브젝트가 자식으로 들어온 경우
	else
	{
		// 이미 해당 오브젝트의 자식인 경우
		if (this == _pChildObj->m_pParentObj)
		{
			return;
		}

		// 연결 끊기
		_pChildObj->DisconnectWithParent();
	}
	
	m_vecChild.push_back(_pChildObj);
	_pChildObj->m_pParentObj = this;

	// 자식오브젝트가 레이어 소속이 없으면 부모를 따라간다.
	if (-1 == _pChildObj->m_iLayerIdx)
	{
		if (m_iLayerIdx == -1)
			int a = 0;
		_pChildObj->m_iLayerIdx = m_iLayerIdx;
	}
	if (_pChildObj->GetChild().size() > 0)
	{
		for (size_t i = 0; i < _pChildObj->GetChild().size(); i++)
		{
			if (_pChildObj->GetChild()[i]->m_iLayerIdx == -1)
			{
				_pChildObj->GetChild()[i]->m_iLayerIdx = m_iLayerIdx;
			};
		}
	}
}

void CGameObject::SaveToScene(FILE* _pFile)
{
	CEntity::SaveToScene(_pFile);

	fwrite(&m_iLayerIdx, sizeof(int), 1, _pFile);

	UINT i = 0;
	for (; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (m_arrCom[i] != nullptr)
		{
			fwrite(&i, sizeof(UINT), 1, _pFile);
			m_arrCom[i]->SaveToScene(_pFile);
		}		
	}

	fwrite(&i, sizeof(UINT), 1, _pFile);
	
	UINT iScriptCount = (UINT)m_vecScript.size();
	fwrite(&iScriptCount, sizeof(UINT), 1, _pFile);

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		g_pScriptSave(m_vecScript[i], _pFile);
	}	

	UINT iChildCount = (UINT)m_vecChild.size();
	fwrite(&iChildCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iChildCount; ++i)
	{
		m_vecChild[i]->SaveToScene(_pFile);
	}
}

void CGameObject::LoadFromScene(FILE* _pFile)
{
	CEntity::LoadFromScene(_pFile);

	fread(&m_iLayerIdx, sizeof(int), 1, _pFile);

	UINT i = 0;	
	while (true)
	{
		fread(&i, sizeof(UINT), 1, _pFile);
		CComponent* pComponent = nullptr;
		switch ((COMPONENT_TYPE)i)
		{
		case COMPONENT_TYPE::TRANSFORM:
			pComponent = new CTransform;			
			break;
		case COMPONENT_TYPE::MESHRENDER:
			pComponent = new CMeshRender;			
			break;
		case COMPONENT_TYPE::CAMERA:
			pComponent = new CCamera;
			break;
		case COMPONENT_TYPE::COLLIDER2D:
			pComponent = new CCollider2D;
			break;
		case COMPONENT_TYPE::COLLIDER3D:
			//pComponent = new CCollider2D;
			break;
		case COMPONENT_TYPE::ANIMATOR2D:
			pComponent = new CAnimator2D;
			break;
		case COMPONENT_TYPE::ANIMATOR3D:
			//pComponent = new CCollider2D;
			break;
		case COMPONENT_TYPE::LIGHT2D:
			pComponent = new CLight2D;
			break;
		case COMPONENT_TYPE::LIGHT3D:
			pComponent = new CLight3D;
			break;
		case COMPONENT_TYPE::PARTICLE:
			pComponent = new CParticleSystem;
			break;			
		}

		if (i == (UINT)COMPONENT_TYPE::END)
			break;
		else
		{
			pComponent->LoadFromScene(_pFile);
			AddComponent(pComponent);
		}
		
	}

	UINT iScriptCount = 0;
	fread(&iScriptCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iScriptCount; ++i)
	{
		CScript* pScript = g_pScriptLoad(_pFile);
		AddComponent(pScript);
	}

	UINT iChildCount = 0;
	fread(&iChildCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iChildCount; ++i)
	{
		CGameObject* pChild = new CGameObject;
		pChild->LoadFromScene(_pFile);	
		int iLayerIdx = pChild->m_iLayerIdx;
		pChild->m_iLayerIdx = -1;
		AddChild(pChild);
		pChild->m_iLayerIdx = iLayerIdx;
	}
}
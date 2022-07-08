#include "pch.h"
#include "CScene.h"

#include "CGameObject.h"
#include "CLayer.h"

CScene::CScene()
	: m_arrLayer{}
	, m_eState(SCENE_STATE::STOP)
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i] = new CLayer(i);
	}
}

CScene::~CScene()
{
	Safe_Delete_Array(m_arrLayer);
}

void CScene::awake()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->awake();
	}
}

void CScene::start()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->start();
	}
}

void CScene::update()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->update();
	}
}

void CScene::lateupdate()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->lateupdate();
	}
}

void CScene::finalupdate()
{
	// 이전 프레임에 등록된 소속 오브젝트를 정리
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->ClearObject();
	}

	// finalupdate 시, 각 오브젝트들은 실제 소속 Layer 에 자신을 등록한다.
	for (int i = 0; i < MAX_LAYER; ++i)
	{		
		m_arrLayer[i]->finalupdate();
	}
}

void CScene::AddObject(CGameObject* _pObject, int _iLayerIdx)
{
	m_arrLayer[_iLayerIdx]->AddObject(_pObject, false);	
}

CGameObject* CScene::FindObjectByName(const wstring& _strName)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObj = m_arrLayer[i]->GetObjects();
		for (size_t j = 0; j < vecObj.size(); ++j)
		{
			if (vecObj[j]->GetName() == _strName)
				return vecObj[j];
		}
	}

	return nullptr;
}

void CScene::SaveToScene(FILE* _pFile)
{
	CEntity::SaveToScene(_pFile);

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->SaveToScene(_pFile);
	}	
}

void CScene::LoadFromScene(FILE* _pFile)
{
	CEntity::LoadFromScene(_pFile);

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->LoadFromScene(_pFile);
	}
}

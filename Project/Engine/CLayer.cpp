#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"



CLayer::CLayer(int _iLayerIdx)
	: m_iLayerIndex(_iLayerIdx)
{
}

CLayer::~CLayer()
{
	Safe_Delete_Vector(m_vecParentObj);
}

void CLayer::awake()
{
	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->awake();
	}
}

void CLayer::start()
{
	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->start();
	}
}

void CLayer::update()
{
	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->update();
	}
}

void CLayer::lateupdate()
{
	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->lateupdate();
	}
}

void CLayer::finalupdate()
{
	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->finalupdate();
	}
}

void CLayer::AddObject(CGameObject* _pObject, bool _bMoveChild)
{
	// Object 가 다른 Layer 소속이었을 경우, 연결 해제를 한다.
	int iLayerIdx = _pObject->GetLayerIndex();

	// 이미 해당 레이어 소속 오브젝트이다
	if (m_iLayerIndex == _pObject->GetLayerIndex())
	{
		return;
	}
	// 소속 Layer 가 존재하고, 현재 Layer 는 아닌경우
	else if (-1 != iLayerIdx && m_iLayerIndex != iLayerIdx)
	{
		// 이전 Layer 와의 관계 해제
		_pObject->ReleaseAsParentObj();
	}

	m_vecParentObj.push_back(_pObject);
	_pObject->SetLayerIndex(m_iLayerIndex);

	// Layer 에 입력되는 오브젝트가 자식오브젝트가 있는 경우
	list<CGameObject*> queue(_pObject->m_vecChild.begin(), _pObject->m_vecChild.end());

	while (!queue.empty())
	{
		CGameObject* pChild = queue.front();
		queue.pop_front();
		
		// 자식들은 부모의 레이어를 따라가는 경우 or 자식 오브젝트가 Layer 소속이 없는경우 부모랑 동일하게 맞춰준다.
		if (_bMoveChild || -1 == pChild->m_iLayerIdx)
		{
			pChild->SetLayerIndex(m_iLayerIndex);
		}	

		// 해당 오브젝트가 또 자식을 보유하고 있으면 큐에 넣어서 계속 순회하도록 한다.
		std::back_insert_iterator< list<CGameObject*>> enditer(queue);
		std::copy(pChild->m_vecChild.begin(), pChild->m_vecChild.end(), enditer);
	}
}

void CLayer::SaveToScene(FILE* _pFile)
{
	CEntity::SaveToScene(_pFile);

	UINT iCount = (UINT)m_vecParentObj.size();
	fwrite(&iCount, sizeof(UINT), 1, _pFile);

	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->SaveToScene(_pFile);
	}
}

void CLayer::LoadFromScene(FILE* _pFile)
{
	CEntity::LoadFromScene(_pFile);

	UINT iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		CGameObject* pNew = new CGameObject;
		pNew->LoadFromScene(_pFile);
		pNew->SetLayerIndex(-1);
		AddObject(pNew, false);
	}
}

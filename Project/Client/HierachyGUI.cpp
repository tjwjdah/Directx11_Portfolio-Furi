#include "pch.h"
#include "HierachyGUI.h"

#include <Engine\CSceneMgr.h>
#include <Engine\CScene.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\CEventMgr.h>
#include <Engine\CScript.h>

#include "CImGUIMgr.h"
#include "InspectorGUI.h"

HierachyGUI::HierachyGUI()
{
	m_Tree.SetDragDropCallBack((DRAG_DROP_CALLBACK)&HierachyGUI::DragDropProgress, this);
	m_Tree.SetSelChangeCallBack((SEL_CHANGE_CALLBACK)&HierachyGUI::ItemSelectChanged, this);
}

HierachyGUI::~HierachyGUI()
{
}

void HierachyGUI::Reset()
{
	m_Tree.Clear();
	tTreeItem* pDummyItem = m_Tree.UseDummyRoot("Dummy");

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurScene->GetLayer(i);
		vector<CGameObject*> vecParent = pLayer->GetParentObj();

		for (size_t j = 0; j < vecParent.size(); ++j)
		{
			AddGameObject(pDummyItem, vecParent[j]);
		}
	}	
}

void HierachyGUI::update()
{
	if (CEventMgr::GetInst()->HasEvnOcrd())
	{
		Reset();		
	}

	m_Tree.GetSelectedItem();
}

void HierachyGUI::AddGameObject(tTreeItem* _pParentItem, CGameObject* _pObject)
{
	wstring strName = _pObject->GetName();
	tTreeItem* pNewItem = m_Tree.AddItem(_pParentItem, string(strName.begin(), strName.end()), (DWORD_PTR)_pObject);

	const vector<CGameObject*>& vecChildObj = _pObject->GetChild();
	for (size_t i = 0; i < vecChildObj.size(); ++i)
	{
		strName = vecChildObj[i]->GetName();
		AddGameObject(pNewItem, vecChildObj[i]);
	}
}

void HierachyGUI::render()
{
	ImGui::Begin("Hierachy");

	m_Tree.render();	

	
	ImGui::End();
}

// Drag, Drop 발생 시 호출되는 함수
void HierachyGUI::DragDropProgress(const tData* _drag, const tData* _dropped)
{
	CGameObject* pDragedItem = (CGameObject*)_drag->dwData;
	CGameObject* pDroppedItem = nullptr;
	if (nullptr != _dropped)
	{
		pDroppedItem = (CGameObject*)_dropped->dwData;
	}

	if (nullptr == pDroppedItem)
	{
		if(pDragedItem->GetParent())
			CScript::DisconnenctWithParent(pDragedItem);
	}
	else
	{
		CScript::AddChild(pDroppedItem, pDragedItem);
	}	
}

// Tree Item 변경 시 호출되는 함수
void HierachyGUI::ItemSelectChanged(const tData* _selectData)
{
	CGameObject* pTargetObj = (CGameObject*)_selectData->dwData;
	
	InspectorGUI* pInspector = (InspectorGUI*)CImGUIMgr::GetInst()->FindGUI(L"Inspector");
	pInspector->SetTargetObject(pTargetObj);
}

#include "pch.h"
#include "TreeCtrl.h"

#include "CImGUIMgr.h"
#include "imgui.h"
#include "GUI.h"

// =========
// tTreeItem
// =========
tTreeItem::tTreeItem()
	: m_pOwner(nullptr)
	, m_bSelected(false)
	, data{}
{
}

tTreeItem::~tTreeItem()
{
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		delete vecChild[i];
	}
}

void tTreeItem::render()
{
	// 자식이 없는 경우, 화살표를 표시하지 않는다.
	int iFlag = 0;
	if (vecChild.empty())
	{
		iFlag = ImGuiTreeNodeFlags_Leaf;
	}

	if(m_bSelected)
		iFlag |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNode(data.strLabel.c_str(), iFlag))
	{
		// 해당 트리아이템에서 Drag 시작 확인
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("BeginDrag", nullptr, 0);

			ImGui::Text(data.strLabel.c_str());
			m_pOwner->SetBeginItem(this);

			ImGui::EndDragDropSource();
		}

		// 해당 트리 아이템에서 Drop 여부 확인
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("BeginDrag"))
			{
				// 드랍됨
				m_pOwner->SetDroppredItem(this);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0) || ImGui::IsItemToggledOpen())
		{
			m_pOwner->SetSelectedItem(this);
		}

		for (size_t i = 0; i < vecChild.size(); ++i)
		{
			vecChild[i]->render();
		}

		ImGui::TreePop();
	}
}


// =========
// TreeCtrl
// =========
TreeCtrl::TreeCtrl()
	: m_pRoot(nullptr)	
	, m_pBeginItem(nullptr)
	, m_pSelectedItem(nullptr)
	, m_pDroppedItem(nullptr)
	, m_eMod(DRAG_DROP_MOD::NONE)
	, m_pCallBackInst(nullptr)
	, m_pCallBack(nullptr)
	, m_pSelChangeInst(nullptr)
	, m_pSelChangeFunc(nullptr)
	, m_bUseDummyRoot(false)
{
}

TreeCtrl::~TreeCtrl()
{
	SAFE_DELETE(m_pRoot);
}

tTreeItem* TreeCtrl::AddItem(tTreeItem* _pParent, const string& _strLabel, DWORD_PTR _dwData)
{
	tTreeItem* pNewItem = new tTreeItem();
	pNewItem->data.strLabel = _strLabel;
	pNewItem->data.dwData = _dwData;

	if (nullptr == _pParent)
	{
		assert(!m_pRoot);
		m_pRoot = pNewItem;
	}
	else
	{
		_pParent->AddChild(pNewItem);
	}

	pNewItem->m_pOwner = this;
	return pNewItem;
}

tTreeItem* TreeCtrl::UseDummyRoot(const string& _strDummyLabel)
{
	m_bUseDummyRoot = 1;
	assert(!m_pRoot);

	return AddItem(nullptr, _strDummyLabel.c_str(), 0);
}

void TreeCtrl::render()
{
	if (nullptr == m_pRoot)
		return;


	if (m_bUseDummyRoot)
	{
		const vector<tTreeItem*>& vecChildItem = m_pRoot->GetChildItem();
		for (size_t i = 0; i < vecChildItem.size(); ++i)
		{
			vecChildItem[i]->render();
		}
	}
	else
	{
		m_pRoot->render();
	}

	if (m_pBeginItem && m_pDroppedItem)
	{
		if (nullptr != m_pCallBack)
		{
			tGUIEvent2 event = {};
			event.pInst = m_pCallBackInst;
			event.pFunc = m_pCallBack;
			event.pData[0] = (void*)&m_pBeginItem->data;
			event.pData[1] = (void*)&m_pDroppedItem->data;
			CImGUIMgr::GetInst()->AddCallBackEvent(event);

			//(*m_pCallBackInst.*m_pCallBack)(&m_pBeginItem->data, &m_pDroppedItem->data);
		}

		if (DRAG_DROP_MOD::SWAP == m_eMod)
		{

		}
		else if (DRAG_DROP_MOD::COPY == m_eMod)
		{

		}


		m_pBeginItem = nullptr;
		m_pDroppedItem = nullptr;
	}

	// Drag, Drop 이벤트 발생 시, 콜백 함수 호출
	if (ImGui::IsMouseReleased(0) && m_pBeginItem && !m_pDroppedItem)
	{
		if (nullptr != m_pCallBack)
		{
			tGUIEvent2 event = {};
			event.pInst = m_pCallBackInst;
			event.pFunc = m_pCallBack;
			event.pData[0] = (void*)&m_pBeginItem->data;
			event.pData[1] = nullptr;
			CImGUIMgr::GetInst()->AddCallBackEvent(event);
			//(*m_pCallBackInst.*m_pCallBack)(&m_pBeginItem->data, nullptr);
		}

		m_pBeginItem = nullptr;
	}


	// TreeCtrl 사용하는 윈도우의 포커싱 상태 여부
	// 포커싱 벗어나면, 선택되었던 아이템 해제
	if (!ImGui::IsWindowFocused())
	{
		if (m_pSelectedItem)
			m_pSelectedItem->m_bSelected = false;

		m_pSelectedItem = nullptr;
	}

}

void TreeCtrl::SetSelectedItem(tTreeItem* _pSelected)
{
	if (nullptr != m_pSelectedItem)
		m_pSelectedItem->m_bSelected = false;
	
	// 아이템 선택이 변경된 경우, 콜백해줄 함수가 있으면 호출해준다.
	if (m_pSelectedItem != _pSelected && m_pSelChangeFunc != nullptr)
	{
		tGUIEvent1 event = {};
		event.pInst = m_pSelChangeInst;
		event.pFunc = m_pSelChangeFunc;
		event.pData = &_pSelected->data;		
		CImGUIMgr::GetInst()->AddCallBackEvent(event);

		//(*m_pSelChangeInst.*m_pSelChangeFunc)(&_pSelected->data);
	}

	m_pSelectedItem = _pSelected;

	if (nullptr != m_pSelectedItem)
		m_pSelectedItem->m_bSelected = true;
}

void TreeCtrl::Clear()
{
	SAFE_DELETE(m_pRoot);
}
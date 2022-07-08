#include "pch.h"
#include "ResGUI.h"

#include <Engine\CRes.h>
#include <Engine\CResMgr.h>

#include "CImGUIMgr.h"
#include "InspectorGUI.h"
#include "TreeCtrl.h"


ResGUI::ResGUI()
{
	m_Tree.SetSelChangeCallBack((SEL_CHANGE_CALLBACK)&ResGUI::SelectResource, this);
}

ResGUI::~ResGUI()
{
}

void ResGUI::Reset()
{
	m_Tree.Clear();
	tTreeItem* pDummyRoot = m_Tree.UseDummyRoot("Dummy");

	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{		
		string strLabel = GetString(g_szResName[i]);
		tTreeItem* pHeadItem = m_Tree.AddItem(pDummyRoot, strLabel, 0);

		const map<wstring, CRes*> mapRes = CResMgr::GetInst()->GetRes((RES_TYPE)i);
		
		for(const auto& pair : mapRes)
		{
			CRes* pRes = pair.second;
			m_Tree.AddItem(pHeadItem, GetString(pRes->GetKey().c_str()), (DWORD_PTR)pRes);
		}
	}
}

void ResGUI::render()
{
	ImGui::Begin("Resouces");

	if (CResMgr::GetInst()->HasEvnOcrd())
	{
		Reset();
	}
	
	m_Tree.render();

	ImGui::End();
}

void ResGUI::SelectResource(const void* _SelChangedData)
{
	const tTreeItem* pSelectedItem = m_Tree.GetSelectedItem();

	string restypename;
	RES_TYPE eResType = RES_TYPE::END;

	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		restypename = GetString(g_szResName[i]);
		if (pSelectedItem->GetParent()->GetLabel() == restypename)
		{
			eResType = (RES_TYPE)i;
			break;
		}
	}

	// 특정 리소스를 클릭한게 아닌 경우
	if (eResType == RES_TYPE::END)
		return;

	tData* pData = (tData*)_SelChangedData;
	InspectorGUI* pInspector = (InspectorGUI*)CImGUIMgr::GetInst()->FindGUI(L"Inspector");
	pInspector->SetTargetResource((CRes*)pData->dwData, eResType);
}

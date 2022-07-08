#include "pch.h"
#include "MeshRenderGUI.h"

#include "ListGUI.h"
#include "CImGUIMgr.h"

#include <Engine\CMeshRender.h>

#include <Engine\CResMgr.h>
#include <Engine\CMesh.h>
#include <Engine\CMaterial.h>
#include <Engine\CCore.h>

MeshRenderGUI::MeshRenderGUI()
	: ComponentGUI(COMPONENT_TYPE::MESHRENDER)
{

}

MeshRenderGUI::~MeshRenderGUI()
{
}

void MeshRenderGUI::update()
{
	ComponentGUI::update();

	if (!IsActive())
		return;
}

void MeshRenderGUI::render()
{
	CMeshRender* pMeshRender = GetTargetObj()->MeshRender();

	Start();

	wstring wstrName = pMeshRender->GetMesh()->GetKey();
	string strName = string(wstrName.begin(), wstrName.end());

	char szBuffer[100] = "";
	strcpy_s(szBuffer, 100, strName.c_str());

	ImGui::PushItemWidth(100);


	ImGui::Text("Mesh\t");
	ImGui::SameLine();
	ImGui::InputText("##MeshName", szBuffer, 100, ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("BeginDrag"))
		{
			// 드랍됨
			int a = 0;
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	if (ImGui::Button("Select##1"))
	{
		ListGUI* pGUI = (ListGUI*)PopupListGUI();

		pGUI->SetCaption("Mesh");

		const map<wstring, CRes*>& mapMesh = CResMgr::GetInst()->GetRes(RES_TYPE::MESH);
		map<wstring, CRes*>::const_iterator iter = mapMesh.begin();
		for (; iter != mapMesh.end(); ++iter)
		{
			pGUI->AddItem(string(iter->first.begin(), iter->first.end()));
		}
		pGUI->AddItem("empty");
	}

	int z = pMeshRender->GetMesh()->GetSubsetCount();
	for (int i = 0; i < z; i++)
	{
		if (pMeshRender->GetSharedMaterial(i).Get() != nullptr)
		{
			wstrName = pMeshRender->GetSharedMaterial(i)->GetKey();
			strName = string(wstrName.begin(), wstrName.end());
			strcpy_s(szBuffer, 100, strName.c_str());
		}
		else {
			strName = "";
			strcpy_s(szBuffer, 100, strName.c_str());
		}

		ImGui::Text("Material");
		ImGui::SameLine();
		string na = "##MtrlName" + std::to_string(i);
		char sz1[100] = "";
		strcpy_s(sz1, 100, na.c_str());

		ImGui::InputText(sz1, szBuffer, 100, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		na = "Select##" + std::to_string(i + 2);
		char sz2[100] = "";
		strcpy_s(sz2, 100, na.c_str());
		if (ImGui::Button(sz2))
		{
			ListGUI* pGUI = (ListGUI*)PopupListGUI(i);

			pGUI->SetCaption("Material");

			const map<wstring, CRes*>& mapMesh = CResMgr::GetInst()->GetRes(RES_TYPE::MATERIAL);
			map<wstring, CRes*>::const_iterator iter = mapMesh.begin();
			for (; iter != mapMesh.end(); ++iter)
			{
				pGUI->AddItem(string(iter->first.begin(), iter->first.end()));
			}
			pGUI->AddItem("empty");
		}
	}

	End();
}

GUI* MeshRenderGUI::PopupListGUI(int i)
{

	ListGUI* pListGUI = (ListGUI*)CImGUIMgr::GetInst()->FindGUI(L"ListGUI");
	pListGUI->SetSelectCallBack(this, (LIST_SELECT_ITEM)&MeshRenderGUI::SelectListItem, i);
	pListGUI->Activate();


	return pListGUI;
}

void MeshRenderGUI::SelectListItem(ListGUI* _pListGUI, const char* _pSelectName, int i)
{
	string strSel = _pListGUI->GetSelect();

	CMeshRender* pMeshRender = GetTargetObj()->MeshRender();

	if (_pListGUI->GetCaption() == "Mesh")
	{
		Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(wstring(strSel.begin(), strSel.end()));
		pMeshRender->SetMesh(pMesh);
		ImGui::SetWindowFocus(nullptr); // 모든 ImGui window focus 해제
	}
	else if (_pListGUI->GetCaption() == "Material")
	{
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstring(strSel.begin(), strSel.end()));
		pMeshRender->SetMaterial(pMtrl, i);
		ImGui::SetWindowFocus(nullptr); // 모든 ImGui window focus 해제
	}
}
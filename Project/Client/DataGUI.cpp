#include "pch.h"
#include "DataGUI.h"

#include "imgui.h"
#include "CImGUIMgr.h"


#include <Engine\CResMgr.h>
#include <Engine\CPrefab.h>
#include <Engine\CTexture.h>
#include <Engine\Ptr.h>

int g_intCount = 0;
char g_szKey[255] = {};

void DataGUI::render_int(const string& _str, void* _pData)
{
	ImGui::Text(_str.c_str()); 
	ImGui::SameLine();
	sprintf_s(g_szKey, "##int_%d", g_intCount++);
	int iData = *(int*)_pData;
	ImGui::InputInt(g_szKey, (int*)&iData);
	*(int*)_pData = iData;
}

void DataGUI::render_float(const string& _str, void* _pData)
{
	ImGui::Text(_str.c_str());
	ImGui::SameLine();
	sprintf_s(g_szKey, "##float_%d", g_intCount++);
	float fData = *(float*)_pData;
	ImGui::InputFloat(g_szKey, (float*)&fData);
	*(float*)_pData = fData;
}

void DataGUI::render_Vec2(const string& _str, void* _pData)
{
	ImGui::Text(_str.c_str());
	ImGui::SameLine();
	sprintf_s(g_szKey, "##Vec2_%d", g_intCount++);
	float arrData[2] = {((Vec2*)_pData)->x,  ((Vec2*)_pData)->y };

	ImGui::InputFloat2(g_szKey, arrData);

	*(Vec2*)_pData = Vec2(arrData[0], arrData[1]);
}

void DataGUI::render_Vec4(const string& _str, void* _pData)
{
	ImGui::Text(_str.c_str());
	ImGui::SameLine();
	sprintf_s(g_szKey, "##Vec4_%d", g_intCount++);
	float arrData[4] = { ((Vec4*)_pData)->x,  ((Vec4*)_pData)->y, ((Vec4*)_pData)->z , ((Vec4*)_pData)->w };

	ImGui::InputFloat4(g_szKey, arrData);

	*(Vec4*)_pData = Vec4(arrData);
}

void DataGUI::render_Prefab(const string& _str, void* _pData)
{
	ImGui::Text(_str.c_str());
	ImGui::SameLine();
	sprintf_s(g_szKey, "##Prefab_%d", g_intCount++);

	Ptr<CPrefab> pPrefab = *((Ptr<CPrefab>*)_pData);

	if (nullptr != pPrefab)
	{
		string strPrefabKey = GetString(pPrefab->GetKey());
		ImGui::InputText(g_szKey, (char*)strPrefabKey.c_str(), strPrefabKey.size(), ImGuiInputTextFlags_ReadOnly);
	}
	else
	{
		char szEmpty[255] = {};
		ImGui::InputText(g_szKey, szEmpty, 255, ImGuiInputTextFlags_ReadOnly);
	}

	// 수정 여부


}

bool DataGUI::render_Texture(const string& _str, void* _pData, GUI* _pInst, LIST_SELECT_ITEM _pFunc)
{
	ImGui::Text(_str.c_str());
	ImGui::SameLine();
	sprintf_s(g_szKey, "##Text_%d", g_intCount++);

	Ptr<CTexture> pTexture = *((Ptr<CTexture>*)_pData);

	string strTexName;
	if (nullptr != pTexture)
	{
		strTexName = GetString(pTexture->GetKey());
	}

	ImGui::PushItemWidth(100);
	ImGui::InputText(g_szKey, (char*)strTexName.c_str(), strTexName.size(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();

	bool bSelect = false;
	sprintf_s(g_szKey, "Select##Texture_%d", g_intCount++);
	if (ImGui::Button(g_szKey))
	{
		ListGUI* pGUI = (ListGUI*)CImGUIMgr::GetInst()->FindGUI(L"ListGUI");

		if (!pGUI->IsActive())
		{
			pGUI->SetCaption("Texture");
			if (nullptr != _pInst && nullptr != _pFunc)
			{
				pGUI->SetSelectCallBack(_pInst, (LIST_SELECT_ITEM)_pFunc);
			}			
			pGUI->Activate();

			const map<wstring, CRes*>& mapTexture = CResMgr::GetInst()->GetRes(RES_TYPE::TEXTURE);
			map<wstring, CRes*>::const_iterator iter = mapTexture.begin();
			for (; iter != mapTexture.end(); ++iter)
			{
				pGUI->AddItem(string(iter->first.begin(), iter->first.end()));
			}
			pGUI->AddItem("empty");
		}
		bSelect = true;
	}

	if (nullptr != pTexture)
	{
		ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
		ImVec2 teszie = ImVec2(100.f, 100.f);				// Texture render size
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.3f); // 50% opaque white
		ImGui::Image(pTexture->GetSRV().Get(), teszie, uv_min, uv_max, tint_col, border_col);
	}

	return bSelect;
}

void DataGUI::render_MemFunc(const string& _str, void* _pData, CScript::SCRIPT_MEMFUNC _pMemFunc)
{
	ImGui::Text(_str.c_str());
	ImGui::SameLine();
	sprintf_s(g_szKey, "Excute##MemFunc_%d", g_intCount++);
	
	if (ImGui::Button(g_szKey))
	{
		CScript* pScriptInst = (CScript*)_pData;
		((*pScriptInst).*_pMemFunc)();
	}
}

void DataGUI::clear()
{
	g_intCount = 0;
}


Vector2& Vector2::operator=(const ImVec2& _imv2)
{
	x = _imv2.x;
	y = _imv2.y;
	return *this;
}


Vector2::operator ImVec2() const
{
	return ImVec2(x, y);
}



Vector4& Vector4::operator=(const ImVec4& _imv4)
{
	x = _imv4.x;
	y = _imv4.y;
	z = _imv4.z;
	w = _imv4.w;
	return *this;
}

Vector4::operator ImVec4() const
{
	return ImVec4(x, y, z, w);
}
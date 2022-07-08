#include "pch.h"
#include "MaterialGUI.h"

#include <Engine\Ptr.h>
#include <Engine\CResMgr.h>
#include <Engine\CMaterial.h>
#include <Engine\CGraphicsShader.h>

#include "ListGUI.h"
#include "CImGUIMgr.h"

#include "DataGUI.h"


MaterialGUI::MaterialGUI()
{
}

MaterialGUI::~MaterialGUI()
{
}

void MaterialGUI::render()
{
	Ptr<CMaterial> pMtrl = (CMaterial*)GetTargetRes();
	Ptr<CGraphicsShader> pShader = pMtrl->GetShader();

	Start();

	// Material Name
	string strName = GetString(pMtrl->GetKey());
	ImGui::InputText("##MtrlName", (char*)strName.c_str(), strName.length(), ImGuiInputTextFlags_ReadOnly);

	// Shader
	ImGui::Text("Shader");
	ImGui::SameLine();



	if (nullptr != pShader)
	{
		string strShaderName = GetString(pShader->GetKey());
		ImGui::InputText("##ShaderName", (char*)strShaderName.c_str(), 100, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
	}


	if (ImGui::Button("Select##Shader"))
	{
		ListGUI* pGUI = (ListGUI*)CImGUIMgr::GetInst()->FindGUI(L"ListGUI");
		pGUI->SetCaption("Shader");
		pGUI->SetSelectCallBack(this, (LIST_SELECT_ITEM)&MaterialGUI::SelectShader);
		pGUI->Activate();

		const map<wstring, CRes*>& mapMtrl = CResMgr::GetInst()->GetRes(RES_TYPE::SHADER);
		map<wstring, CRes*>::const_iterator iter = mapMtrl.begin();
		for (; iter != mapMtrl.end(); ++iter)
		{
			if (dynamic_cast<CGraphicsShader*>(iter->second))
			{
				pGUI->AddItem(string(iter->first.begin(), iter->first.end()));
			}
			pGUI->AddItem("empty");
		}
	}


	// Shader Parameter
	if (nullptr != pShader)
	{
		render_shaderparam();
	}

	End();
}

void MaterialGUI::SelectShader(ListGUI* _pGUI, const char* _strSelect)
{
	string strSelect = _pGUI->GetSelect();

	Ptr<CGraphicsShader> pShader = CResMgr::GetInst()->FindRes<CGraphicsShader>(wstring(strSelect.begin(), strSelect.end()));
	Ptr<CMaterial> pMtrl = (CMaterial*)GetTargetRes();
	pMtrl->SetShader(pShader);
}

void MaterialGUI::SelectTexture(ListGUI* _pGUI, const char* _strSelect)
{
	string strSelect = _pGUI->GetSelect();

	Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(wstring(strSelect.begin(), strSelect.end()));
	Ptr<CMaterial> pMtrl = (CMaterial*)GetTargetRes();
	pMtrl->SetData(m_eSelectParam, pTexture.Get());
}

void MaterialGUI::render_shaderparam()
{
	Ptr<CMaterial> pMtrl = (CMaterial*)GetTargetRes();
	Ptr<CGraphicsShader> pShader = pMtrl->GetShader();

	ImGui::NewLine();
	ImGui::Text("Shader Parameter");
	ImGui::NewLine();

	const vector<ShaderParamInfo>& vecParam = pShader->GetParamInfo();
	string strName;
	for (size_t i = 0; i < vecParam.size(); ++i)
	{
		strName = GetString(vecParam[i].strParamName);
		switch (vecParam[i].eParamType)
		{
		case SHADER_PARAM::INT_0:
		case SHADER_PARAM::INT_1:
		case SHADER_PARAM::INT_2:
		case SHADER_PARAM::INT_3:
			DataGUI::render_int(strName, pMtrl->GetData(vecParam[i].eParamType));
			break;
		case SHADER_PARAM::FLOAT_0:
		case SHADER_PARAM::FLOAT_1:
		case SHADER_PARAM::FLOAT_2:
		case SHADER_PARAM::FLOAT_3:
			DataGUI::render_float(strName, pMtrl->GetData(vecParam[i].eParamType));
			break;
		case SHADER_PARAM::VEC2_0:
		case SHADER_PARAM::VEC2_1:
		case SHADER_PARAM::VEC2_2:
		case SHADER_PARAM::VEC2_3:
			DataGUI::render_Vec2(strName, pMtrl->GetData(vecParam[i].eParamType));
			break;
		case SHADER_PARAM::VEC4_0:
		case SHADER_PARAM::VEC4_1:
		case SHADER_PARAM::VEC4_2:
		case SHADER_PARAM::VEC4_3:
			DataGUI::render_Vec4(strName, pMtrl->GetData(vecParam[i].eParamType));
			break;
		case SHADER_PARAM::MAT_0:
		case SHADER_PARAM::MAT_1:
		case SHADER_PARAM::MAT_2:
		case SHADER_PARAM::MAT_3:
			break;
		case SHADER_PARAM::TEX_0:
		case SHADER_PARAM::TEX_1:
		case SHADER_PARAM::TEX_2:
		case SHADER_PARAM::TEX_3:
		case SHADER_PARAM::TEX_4:
		case SHADER_PARAM::TEX_5:
		case SHADER_PARAM::TEX_6:
		case SHADER_PARAM::TEX_7:
		case SHADER_PARAM::TEXARR_0:
		case SHADER_PARAM::TEXARR_1:
		case SHADER_PARAM::TEXCUBE_0:
		case SHADER_PARAM::TEXCUBE_1:
			if (DataGUI::render_Texture(strName, pMtrl->GetData(vecParam[i].eParamType), this, (LIST_SELECT_ITEM)&MaterialGUI::SelectTexture))
			{
				m_eSelectParam = vecParam[i].eParamType;
			}
			break;
		default:
			break;
		}

	}

}


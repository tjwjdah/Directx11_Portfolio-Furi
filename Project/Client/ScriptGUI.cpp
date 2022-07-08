#include "pch.h"
#include "ScriptGUI.h"

#include <Engine\CScript.h>

#include "DataGUI.h"

ScriptGUI::ScriptGUI()
	: ComponentGUI(COMPONENT_TYPE::SCRIPT)
	, m_pTargetScript(nullptr)
{
}

ScriptGUI::~ScriptGUI()
{
}

void ScriptGUI::update()
{
	ComponentGUI::update();
}

void ScriptGUI::render()
{
	assert(m_pTargetScript);

	Start();

	const vector<tDataDesc>& vecDataDesc = m_pTargetScript->GetDataDesc();
	for (size_t i = 0; i < vecDataDesc.size(); ++i)
	{
		switch (vecDataDesc[i].eType)
		{
		case SCRIPT_DATA_TYPE::INT:
			DataGUI::render_int(vecDataDesc[i].strName.c_str(), vecDataDesc[i].pData);
			break;
		case SCRIPT_DATA_TYPE::FLOAT:
			DataGUI::render_float(vecDataDesc[i].strName.c_str(), vecDataDesc[i].pData);
			break;
		case SCRIPT_DATA_TYPE::VEC2:
			DataGUI::render_Vec2(vecDataDesc[i].strName.c_str(), vecDataDesc[i].pData);
			break;
		case SCRIPT_DATA_TYPE::VEC4:
			DataGUI::render_Vec4(vecDataDesc[i].strName.c_str(), vecDataDesc[i].pData);
			break;
		case SCRIPT_DATA_TYPE::PREFAB:
			DataGUI::render_Prefab(vecDataDesc[i].strName.c_str(), vecDataDesc[i].pData);
			break;	
		case SCRIPT_DATA_TYPE::MEMFUNC:
			DataGUI::render_MemFunc(vecDataDesc[i].strName.c_str(), vecDataDesc[i].pData, vecDataDesc[i].pMemFunc);
			break;
		}
	}



	End();

	ImVec2 vSize = ImGui::GetItemRectSize();
	SetSize(Vec2(0.f, 300.f));
}
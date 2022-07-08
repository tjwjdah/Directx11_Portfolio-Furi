#include "pch.h"
#include "ComponentGUI.h"
#include <Engine\CGameObject.h>

ComponentGUI::ComponentGUI(COMPONENT_TYPE _eType)
	: m_pTargetObj(nullptr)
	, m_eCT(_eType)
{
}

ComponentGUI::~ComponentGUI()
{
}

void ComponentGUI::update()
{
	if (nullptr == GetTargetObj() || nullptr == GetTargetObj()->GetComponent(m_eCT))
		DeActivate();
	else
		Activate();	
}



void ComponentGUI::Start()
{
	string strName = GetString(GetName());

	ImGui::BeginChild(strName.c_str(), ImVec2(m_vSize.x, m_vSize.y));
	float fOrigin = ImGui::GetFontSize();
	
	ImGui::Text(strName.c_str());
	ImGui::NewLine();
}

void ComponentGUI::End()
{
	ImGui::EndChild();
}

#include "pch.h"
#include "ResInfoGUI.h"


ResInfoGUI::ResInfoGUI()
	: m_pTargetRes(nullptr)	
{
}

ResInfoGUI::~ResInfoGUI()
{
}


void ResInfoGUI::Start()
{
	string strName = GetString(GetName());

	ImGui::BeginChild(strName.c_str(), ImVec2(0.f, 0.f));	
	ImGui::Text(strName.c_str());
	ImGui::NewLine();
}

void ResInfoGUI::End()
{
	ImGui::EndChild();
}

#include "pch.h"
#include "InspectorGUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CSceneMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CRes.h>

#include <Script\CScriptMgr.h>

#include "ComponentGUI.h"
#include "TransformGUI.h"
#include "MeshRenderGUI.h"
#include "Collider2DGUI.h"
#include "ScriptGUI.h"

#include "ResInfoGUI.h"
#include "MaterialGUI.h"
#include "Animator3DGUI.h"
#include "Light3DGUI.h"
InspectorGUI::InspectorGUI()
    : m_arrComGUI{}
    , m_arrResInfoGUI{}
    , m_pTargetObj(nullptr)
    , m_pTargetRes(nullptr)
{
}

InspectorGUI::~InspectorGUI()
{
    Safe_Delete_Array(m_arrComGUI);
    Safe_Delete_Array(m_arrResInfoGUI);
    Safe_Delete_Vector(m_vecScriptGUI);
}

void InspectorGUI::init()
{
    ComponentGUI* pNew = new TransformGUI;
  
    pNew->SetName(L"Transform");
    pNew->SetSize(Vec2(0.f, 100.f));
    m_arrComGUI[(UINT)COMPONENT_TYPE::TRANSFORM] = pNew;

    pNew = new MeshRenderGUI;
  
    pNew->SetName(L"MeshRender");
    pNew->SetSize(Vec2(0.f, 80.f));
    m_arrComGUI[(UINT)COMPONENT_TYPE::MESHRENDER] = pNew;

    pNew = new Collider2DGUI;
   
    pNew->SetName(L"Collider2D");
    pNew->SetSize(Vec2(0.f, 100.f));
    m_arrComGUI[(UINT)COMPONENT_TYPE::COLLIDER2D] = pNew;

    pNew = new Animator3DGUI;
    pNew->SetName(L"Animator3d");
    pNew->SetSize(Vec2(0.f, 100.f));
    m_arrComGUI[(UINT)COMPONENT_TYPE::ANIMATOR3D] = pNew;

    pNew = new Light3DGUI;
    pNew->SetName(L"Light3D");
    pNew->SetSize(Vec2(0.f, 100.f));
    m_arrComGUI[(UINT)COMPONENT_TYPE::LIGHT3D] = pNew;


    for (int i = 0; i < 10; ++i)
    {
        m_vecScriptGUI.push_back(new ScriptGUI);
    }    

    ResInfoGUI* pResInfoGUI = new MaterialGUI;
    pResInfoGUI->SetName(L"Material");
    m_arrResInfoGUI[(UINT)RES_TYPE::MATERIAL] = pResInfoGUI;
}

void InspectorGUI::update()
{
    
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr == m_arrComGUI[i])
			continue;

		m_arrComGUI[i]->SetTargetObj(m_pTargetObj);
		m_arrComGUI[i]->update();
	}

    if (m_pTargetObj)
    {
     
        const vector<CScript*> vecScript = m_pTargetObj->GetScripts();
        for (size_t i = 0; i < vecScript.size(); ++i)
        {
            if (m_vecScriptGUI.size() <= i)
            {
                m_vecScriptGUI.push_back(new ScriptGUI);
            }

            m_vecScriptGUI[i]->SetTargetObj(m_pTargetObj);
            m_vecScriptGUI[i]->update();
        }
    }
    else
    {
        for (size_t i = 0; i < m_vecScriptGUI.size(); ++i)
        {
            m_vecScriptGUI[i]->SetTargetObj(nullptr);
            m_vecScriptGUI[i]->update();
        }        
    }
	
   
    if (m_pTargetRes)
    {
        if (nullptr != m_arrResInfoGUI[(UINT)m_eResType])
        {
            m_arrResInfoGUI[(UINT)m_eResType]->SetTargetRes(m_pTargetRes);
            m_arrResInfoGUI[(UINT)m_eResType]->update();
        }
    }
}

void InspectorGUI::render()
{
    ImGui::Begin("Inspector");

    if (m_pTargetObj)
    {
        for(UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
        {
            if (nullptr == m_arrComGUI[i])
                continue;
                        
            if (m_arrComGUI[i]->IsActive())
            {
                m_arrComGUI[i]->render();
                ImGui::Separator();
            }             
        }

        // Object Script 에 대응하는 ScriptGUI render
        const vector<CScript*> vecScript = m_pTargetObj->GetScripts();
        for (size_t i = 0; i < vecScript.size(); ++i)
        {                                
            m_vecScriptGUI[i]->SetName(CScriptMgr::GetScriptName(vecScript[i]));                   
            m_vecScriptGUI[i]->SetScript(vecScript[i]);
            m_vecScriptGUI[i]->render();
        }
    }
    else if (m_pTargetRes)
    {
        if (nullptr != m_arrResInfoGUI[(UINT)m_eResType])
        {
            m_arrResInfoGUI[(UINT)m_eResType]->render();
        }
    }

    ImGui::End();
}

#include "pch.h"
#include "MenuGUI.h"

#include "CSaveLoadMgr.h"
#include "InspectorGUI.h"
#include "CImGUIMgr.h"

#include <Engine\CSceneMgr.h>
#include <Engine\CScene.h>

#include "MRTGUI.h"

MenuGUI::MenuGUI()
{
}

MenuGUI::~MenuGUI()
{
}


void MenuGUI::update()
{
}

void MenuGUI::render()
{
    if (ImGui::BeginMainMenuBar())
    {       
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("Save Scene");
            ImGui::MenuItem("Load Scene");

            ImGui::Separator();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject"))
        {
            ImGui::MenuItem("Create Empty Object");

            if (ImGui::BeginMenu("Add Component"))
            {
                if (ImGui::MenuItem("MeshRender"))
                {

                }

                if (ImGui::MenuItem("Collider2D"))
                {

                }

                if (ImGui::MenuItem("Light2D"))
                {

                }

                if (ImGui::MenuItem("Camera"))
                {

                }

                if (ImGui::MenuItem("ParticleSystem"))
                {

                }

                ImGui::EndMenu();
            }            

            ImGui::Separator();

            ImGui::EndMenu();
        }

        CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

        if (ImGui::BeginMenu("Scene Mode"))
        {
            if (pCurScene->GetState() != SCENE_STATE::PLAY)
            {
                // Play 버튼을 눌렀다.
                if (ImGui::MenuItem("Play"))
                {
                    ChangeSceneState(pCurScene, SCENE_STATE::PLAY);                   
                }
            }
            else
            {
                // Pause 버튼을 눌렀다.
                if (ImGui::MenuItem("Pause"))
                {
                    ChangeSceneState(pCurScene, SCENE_STATE::PAUSE);                   
                }
            }

            // 정지버튼을 누른경우	
            if (ImGui::MenuItem("Stop"))
            {
                ChangeSceneState(pCurScene, SCENE_STATE::STOP);              
            }

            ImGui::EndMenu();
        }
       
        if (ImGui::BeginMenu("Tool Window"))
        {
            if (ImGui::MenuItem("MRT Window"))
            {
                MRTGUI* pMRTGUI = (MRTGUI*)CImGUIMgr::GetInst()->FindGUI(L"MRT");
                pMRTGUI->Activate();
            }

            ImGui::EndMenu();
        }


        ImGui::EndMainMenuBar();
    }
}

void MenuGUI::ChangeSceneState(CScene* _pScene, SCENE_STATE _eState)
{
    // 정지 -> 플레이
    if (SCENE_STATE::STOP == _pScene->GetState() && SCENE_STATE::PLAY == _eState)
    {
        CSaveLoadMgr::SaveScene(_pScene, L"scene\\temp.scene");

        _pScene->awake();
        _pScene->start();
    }

    else if (_pScene->GetState() != SCENE_STATE::STOP && _eState == SCENE_STATE::STOP)
    {
        _pScene = new CScene;

        CSaveLoadMgr::LoadScene(_pScene, L"scene\\temp.scene");
        CSceneMgr::GetInst()->ChangeScene(_pScene);

        // InspeactorGUI 의 타겟 오브젝트 무효화
        InspectorGUI* pInspector = (InspectorGUI*)CImGUIMgr::GetInst()->FindGUI(L"Inspector");
        pInspector->SetTargetObject(nullptr);
    }
    _pScene->SetState(_eState);
}

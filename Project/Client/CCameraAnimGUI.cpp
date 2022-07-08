#include "pch.h"
#include "CCameraAnimGUI.h"

#include <Engine\CSceneMgr.h>
#include <Engine\CScene.h>

#include <Script\CCameraAnimaitorScript.h>

CCameraAnimGUI::CCameraAnimGUI():m_Camera(nullptr)
{
}

CCameraAnimGUI::~CCameraAnimGUI()
{
}


void CCameraAnimGUI::init()
{
   
}

void CCameraAnimGUI::update()
{
    if (m_Camera == nullptr)
    {
        m_Camera = (CCameraAnimaitorScript*)CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Camera_bn")->GetScript(L"CCameraAnimaitorScript");
    }
}

void CCameraAnimGUI::render()
{
   // return;
    bool bOpen = true;
    
    static ImGuiComboFlags flags3 = 0;
 
    const vector<CameraData>* _vecCameraclip = m_Camera->GetCameraClipData(); //현재 재생 클립vec
    if (_vecCameraclip->size() <= 0)
        return;
    static int item_current_idx4 = 0;
    ImGui::Begin("CameraAnim", &bOpen);
    

    vector<string>  vecItem; //콤보박스 vec
    for (int i = 0; i < _vecCameraclip->size(); ++i)
        vecItem.push_back(string(_vecCameraclip->at(i).CameraClipName.begin(), _vecCameraclip->at(i).CameraClipName.end()));

    UINT _maxidx = vecItem.size();
    ImGui::Text("vecitem size : %d", _maxidx);
    ImGui::Text(" Select Animation "); ImGui::SameLine(); ImGui::PushItemWidth(100);
    if (ImGui::BeginCombo("animcombo", vecItem[item_current_idx4].c_str(), flags3))
    {
        for (int n = 0; n < _maxidx; n++)
        {
            const bool is_selected = (item_current_idx4 == n);
            if (ImGui::Selectable(vecItem[n].c_str(), is_selected))
            {
                item_current_idx4 = n;
                m_Camera->SetCameraAnim(n);

                //psc->SetAnimationClipNum(n);
                //pAni->PlayAnimation(_vecAniclip->at(item_current_idx3).strAnimName, contents_type);
                //pAni->ChangeAnimation(_vecAniclip->at(item_current_idx3).strAnimName);
               // SetAnimationClipNum(item_current_idx3);

            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    float recodingfrm = m_Camera->GetRecodeFrm();
    ImGui::Text("RecodingFrm\t");  ImGui::SameLine(); ImGui::InputFloat("##Position", &recodingfrm);
    m_Camera->SetRecodeFrm(recodingfrm);
    if (ImGui::Button("ClipRecodeing##1") || (KEY_TAP(KEY_TYPE::NUM_0)))
    {
        m_Camera->ClipRecoding();
    }
    ImGui::End();
}

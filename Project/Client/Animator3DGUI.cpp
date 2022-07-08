#include "pch.h"
#include "Animator3DGUI.h"
#include "imgui.h"
#include "ListGUI.h"
#include "CImGUIMgr.h"
#include "DataGUI.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>
#include <Engine\CKeyMgr.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CMesh.h>

#include <Script/CAnimatorScript.h>

Animator3DGUI::Animator3DGUI()
    : ComponentGUI(COMPONENT_TYPE::ANIMATOR3D)
{
}

Animator3DGUI::~Animator3DGUI()
{
}

void Animator3DGUI::lateupdate()
{

}

void Animator3DGUI::render()
{
    CAnimator3D* pAni = GetTargetObj()->Animator3D();
    Ptr<CMesh> pMesh = GetTargetObj()->MeshRender()->GetMesh();// ->GetAnimClip();
   // CAnimatorScript* psc = (CAnimatorScript*)GetTargetObj()->GetScript(L"CAnimatorScript");

    Start();

    enum ContentsType { REPEATOFF, REPEATON };
    static int contents_type = REPEATON;

    //ImGui::AlignTextToFramePadding(); 
    //ImGui::Text(" repeat ");
    //ImGui::SameLine(); ImGui::RadioButton("on", &contents_type, REPEATON);
    //ImGui::SameLine(); ImGui::RadioButton("off", &contents_type, REPEATOFF);

    //애니 콤보바ㅓㄱ스
    //ImGui::Text(" Animator3D "); ImGui::SameLine();
    static ImGuiComboFlags flags3 = 0;
    static int item_current_idx3 = pAni->GetCurClip(); //현재 재생 애니 클립 idx
    const vector<tMTAnimClip>* _vecAniclip = pMesh->GetAnimClip(); //현재 재생 클립vec

    vector<string>  vecItem; //콤보박스 vec
    for (int i = 0; i < _vecAniclip->size(); ++i)
        vecItem.push_back(string(_vecAniclip->at(i).strAnimName.begin(), _vecAniclip->at(i).strAnimName.end()));

    UINT _maxidx = vecItem.size();
    ImGui::Text("vecitem size : %d", _maxidx);
    ImGui::Text(" Select Animation "); ImGui::SameLine(); ImGui::PushItemWidth(100);
    if (ImGui::BeginCombo("animcombo", vecItem[item_current_idx3].c_str(), flags3))
    {
        for (int n = 0; n < _maxidx; n++)
        {
            const bool is_selected = (item_current_idx3 == n);
            if (ImGui::Selectable(vecItem[n].c_str(), is_selected))
            {
                item_current_idx3 = n;
                pAni->ChangeClip(n);

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


    //애니메이션 정보
    ImGui::Separator();
    ImGui::Text(" Animation info "); //ImGui::SameLine();// ImGui::PushItemWidth(100);

    static ImGuiTableFlags flags1 = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_ContextMenuInBody;
    static ImGuiTableFlags sizing_policy_flags[4] = { ImGuiTableFlags_SizingFixedFit, ImGuiTableFlags_SizingFixedSame, ImGuiTableFlags_SizingStretchProp, ImGuiTableFlags_SizingStretchSame };

    int table_n = 3;
    ImGui::PushID(table_n);
    if (ImGui::BeginTable("table1", 2, sizing_policy_flags[table_n] | flags1))
    {

        string _aniname = string(_vecAniclip->at(item_current_idx3).strAnimName.begin(), _vecAniclip->at(item_current_idx3).strAnimName.end());
        char buf[BUFSIZ] = "";
        strcpy_s(buf, BUFSIZ, _aniname.c_str());
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("name");
        ImGui::TableNextColumn(); ImGui::Text(buf);

        //sprintf_s(buf, "%d", item_current_idx3);
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("m_iCurClip");
        ImGui::TableNextColumn(); ImGui::Text("%d", item_current_idx3);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("m_iFrameIdx");
        ImGui::TableNextColumn(); ImGui::Text("%d", pAni->GetiFrameIdx());

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("m_dCurTime");
        ImGui::TableNextColumn(); ImGui::Text("%.3f", pAni->GetdCurTime());

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("UpdateTime");
        ImGui::TableNextColumn(); ImGui::Text("%.3f", pAni->GetvecClipUpdateTime());

        ImGui::EndTable();
    }

    // ImGui::PushID(table_n+1);
    if (ImGui::BeginTable("table2", 2, sizing_policy_flags[table_n] | flags1))
    {

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("StartFrame");
        ImGui::TableNextColumn(); ImGui::Text("%d", _vecAniclip->at(item_current_idx3).iStartFrame);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("EndFrame");
        ImGui::TableNextColumn(); ImGui::Text("%d", _vecAniclip->at(item_current_idx3).iEndFrame);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("FrameLength");
        ImGui::TableNextColumn(); ImGui::Text("%d", _vecAniclip->at(item_current_idx3).iFrameLength);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("StartTime");
        ImGui::TableNextColumn(); ImGui::Text("%.3f", _vecAniclip->at(item_current_idx3).dStartTime);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("EndTime");
        ImGui::TableNextColumn(); ImGui::Text("%.3f", _vecAniclip->at(item_current_idx3).dEndTime);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("TimeLength");
        ImGui::TableNextColumn(); ImGui::Text("%.3f", _vecAniclip->at(item_current_idx3).dTimeLength);

        ImGui::EndTable();
    }

    ImGui::PopID();

    if (ImGui::Button("FileSaveClipName##1"))
    {
        pAni->ClipNameSave();
    }
    /*
    //ImGui::Separator();
    //ImGui::Text(" Tool "); //ImGui::SameLine();// ImGui::PushItemWidth(100);


    //delete버튼
    if (ImGui::Button("delete", { 140, 20 }))
    {
        ImGui::OpenPopup("Stacked 1");
    }
    if (ImGui::BeginPopupModal("Stacked 1", NULL, ImGuiWindowFlags_None))
    {
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Button("delete"))
        {
            if (item_current_idx3 != 0)
            {
                pAni->DeleteAnimation(_vecAniclip->at(item_current_idx3).strAnimName);
                item_current_idx3 = 0;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }


    ImGui::SameLine();
    //ani tool버튼
    static bool clicked = false;
    if (ImGui::Button("create", { 140, 20 })) { clicked = true; }
    if (clicked) ShowExampleAppLayout(&clicked, pAni);

    */

    End();


    SetSize(Vec2(0.f, 800.f));
}

void Animator3DGUI::ShowExampleAppLayout(bool* p_open, CAnimator3D* _pani)
{
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("create", p_open, ImGuiWindowFlags_MenuBar))
    {
        // Set Animation Name
        static char _3dAniName[64] = {};
        ImGui::Text("ani name  "); ImGui::SameLine(); ImGui::InputText("##AniName", _3dAniName, IM_ARRAYSIZE(_3dAniName));
        wstring _wAniName(&_3dAniName[0], &_3dAniName[20]); //입력받은 애니메이션 이름 2byte 변환 


        // Set Duration
        static int _startfrm = 0;// = m_fDuration;
        ImGui::Text("start frm "); ImGui::SameLine(); ImGui::InputInt("##start", &_startfrm);
        //m_fDuration = dData;

        static int _endfrm = 0;// = m_fDuration;
        ImGui::Text("end frm   "); ImGui::SameLine(); ImGui::InputInt("##end", &_endfrm);

        ImGui::SameLine();
        if (ImGui::Button("exit", { 140, 20 }))
        {
            *p_open = false;
        }


    }
    ImGui::End();
}

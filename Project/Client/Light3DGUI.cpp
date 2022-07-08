#include "pch.h"
#include "Light3DGUI.h"

#include <Engine/CKeyMgr.h>

#include <Engine/CLight3D.h>

Light3DGUI::Light3DGUI()
    : ComponentGUI(COMPONENT_TYPE::LIGHT3D)
    , m_iLightTypeIdx(0)
{
}

Light3DGUI::~Light3DGUI()
{
}

void Light3DGUI::lateupdate()
{
    if (IsActive() && KEY_TAP(KEY_TYPE::ENTER))
    {
        ImGui::SetWindowFocus(nullptr); // 葛电 ImGui window focus 秦力
    }
}

void Light3DGUI::render()
{
    CLight3D* pLight3D = GetTargetObj()->Light3D();

    float arrData[3][3] = {};
    Vec4 vDiff = pLight3D->GetInfo().color.vDiff;
    Vec4 vSpec = pLight3D->GetInfo().color.vSpec;
    Vec4 vAmb = pLight3D->GetInfo().color.vAmb;
    float fRange = pLight3D->GetInfo().fRange;
    float fAngle = pLight3D->GetInfo().fAngle;

    arrData[0][0] = vDiff.x;
    arrData[0][1] = vDiff.y;
    arrData[0][2] = vDiff.z;

    arrData[1][0] = vSpec.x;
    arrData[1][1] = vSpec.y;
    arrData[1][2] = vSpec.z;

    arrData[2][0] = vAmb.x;
    arrData[2][1] = vAmb.y;
    arrData[2][2] = vAmb.z;

    Start();
    ImGui::Text("Diffuse      ");  ImGui::SameLine(); ImGui::InputFloat3("##Diffuse", arrData[0], "%.2f");
    ImGui::Text("Specular     ");  ImGui::SameLine(); ImGui::InputFloat3("##Specular", arrData[1], "%.2f");
    ImGui::Text("Ambient      ");  ImGui::SameLine(); ImGui::InputFloat3("##Ambient", arrData[2], "%.2f");

    ImGui::Text("Range        ");  ImGui::SameLine(); ImGui::InputFloat("##Range", &fRange, 0, 0, "%.2f");
   // ImGui::Text("Angle        ");  ImGui::SameLine(); ImGui::InputFloat("##Angle", &fAngle, 0, 0, "%.2f");

    const char* items[] = { "DIR", "POINT", "SPOT"};

    LIGHT_TYPE eType = pLight3D->GetLightType();
    int item_current = int(eType);

    ImGui::Text("Light Type   "); ImGui::SameLine();
    ImGui::Combo("##Light Type", &item_current, items, IM_ARRAYSIZE(items));

    End();

    pLight3D->SetDiffusePow(Vec3(arrData[0]));
    pLight3D->SetSpecPow(Vec3(arrData[1]));
    pLight3D->SetAmbPow(Vec3(arrData[2]));
    pLight3D->SetRange(fRange);
//pLight3D->SetAngle(fAngle);

    if (m_iLightTypeIdx != item_current)
    {
        m_iLightTypeIdx = item_current;
        pLight3D->SetLightType((LIGHT_TYPE)m_iLightTypeIdx);
        ImGui::SetWindowFocus(nullptr); // 葛电 ImGui window focus 秦力
    }
}

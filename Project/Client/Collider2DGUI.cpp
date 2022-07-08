#include "pch.h"
#include "Collider2DGUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CCollider2D.h>
#include <Engine\CKeyMgr.h>

Collider2DGUI::Collider2DGUI()
    : ComponentGUI(COMPONENT_TYPE::COLLIDER2D)
    , m_iColliderTypeIdx(0)
{
}

Collider2DGUI::~Collider2DGUI()
{
}

void Collider2DGUI::lateupdate()
{
    if (IsActive() && KEY_TAP(KEY_TYPE::ENTER))
    {
        ImGui::SetWindowFocus(nullptr); // 葛电 ImGui window focus 秦力
    }
}

void Collider2DGUI::render()
{
    CCollider2D* pCollider = GetTargetObj()->Collider2D();

    float arrData[2][2] = {};
    Vec2 vData[2] = { pCollider->GetvOffsetPos(), pCollider->GetvOffsetScale() };

    arrData[0][0] = vData[0].x;
    arrData[0][1] = vData[0].y;

    arrData[1][0] = vData[1].x;
    arrData[1][1] = vData[1].y;

    Start();
    ImGui::Text("Offset Pos     ");  ImGui::SameLine(); ImGui::InputFloat2("##Position", arrData[0], "%.2f");
    ImGui::Text("OffsetScale    ");  ImGui::SameLine(); ImGui::InputFloat2("##Scale", arrData[1], "%.2f");  

    const char* items[] = { "RECT", "CIRCLE"};

    COLLIDER2D_TYPE eType = pCollider->GetCollider2DType();
    int item_current = int(eType);

    ImGui::Text("Collider Type  "); ImGui::SameLine();
    ImGui::Combo("##Collider2D Type", &item_current, items, IM_ARRAYSIZE(items));

    End();


    pCollider->SetvOffsetPos(Vec2(arrData[0]));
    pCollider->SetvOffsetScale(Vec2(arrData[1]));

    if (m_iColliderTypeIdx != item_current)
    {
        m_iColliderTypeIdx = item_current;
        pCollider->SetCollider2DType((COLLIDER2D_TYPE)m_iColliderTypeIdx);
        ImGui::SetWindowFocus(nullptr); // 葛电 ImGui window focus 秦力
    }    
}


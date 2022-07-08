#include "pch.h"
#include "TransformGUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CSceneMgr.h>
#include <Engine\CTransform.h>
#include <Engine/CCamera.h>
#include <Engine/CScene.h>
#include "CToolCamScript.h"
#include <Engine/CRenderMgr.h>
#include <Engine\CTimeMgr.h>
#include <Engine\CKeyMgr.h>
#include "CImGUIMgr.h"
#include "ImGuizmo.h"
#include <math.h>
#include <vector>
#include <algorithm>

TransformGUI::TransformGUI()
    : ComponentGUI(COMPONENT_TYPE::TRANSFORM)
    , m_arrData{}
{
    //m_toolcam = CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"Camera Object");
    //m_toolcam = CRenderMgr::GetInst()->GetCurCam();
}

TransformGUI::~TransformGUI()
{
}

static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

void TransformGUI::init()
{
}

void TransformGUI::lateupdate()
{
    if (IsActive() && KEY_TAP(KEY_TYPE::ENTER))
    {
        ImGui::SetWindowFocus(nullptr); // 모든 ImGui window focus 해제
    }
}

void TransformGUI::render()
{
    CCamera* ptoolcam = CRenderMgr::GetInst()->GetCurCam();

    if (nullptr != ptoolcam && GetTargetObj()->GetParent() == nullptr)
    {
    
        //tool
        CTransform* pTransform = GetTargetObj()->Transform();
        const tRay pray = ptoolcam->GetRay();

        const Matrix _worldmat = pTransform->GetWorldMat();
        float objectMatrix[16] =
        {
            _worldmat._11,_worldmat._12,_worldmat._13,_worldmat._14,
            _worldmat._21,_worldmat._22,_worldmat._23,_worldmat._24,
            _worldmat._31,_worldmat._32,_worldmat._33,_worldmat._34,
            _worldmat._41,_worldmat._42,_worldmat._43,_worldmat._44
        };

        const Matrix _view = ptoolcam->Camera()->GetViewMat();
        const Matrix _proj = ptoolcam->Camera()->GetProjMat();


        float cameraView[16] =
        {
            _view._11,_view._12,_view._13,_view._14,
            _view._21,_view._22,_view._23,_view._24,
            _view._31,_view._32,_view._33,_view._34,
            _view._41,_view._42,_view._43,_view._44
        };

        float cameraProjection[16] =
        {
          _proj._11,_proj._12,_proj._13,_proj._14,
          _proj._21,_proj._22,_proj._23,_proj._24,
          _proj._31,_proj._32,_proj._33,_proj._34,
          _proj._41,_proj._42,_proj._43,_proj._44
        };


        Start();

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("X: %f Y: %f", io.MousePos.x, io.MousePos.y);
        if (ImGuizmo::IsUsing())
        {
            ImGui::Text("Using gizmo");
        }
        else
        {
            ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");
            ImGui::SameLine();
            ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");
            ImGui::SameLine();
            ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");
            ImGui::SameLine();
            ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
        }
        ImGui::Separator();

        // float arr3[3] = { pray.vDir.x , pray.vDir.y , pray.vDir.z };
        // float arr4[3] = { pray.vPoint.x,pray.vPoint.y , pray.vPoint.z };
        // ImGui::Text("1\t");  ImGui::SameLine(); ImGui::InputFloat3("##1", arr3);
        // ImGui::Text("2   \t");  ImGui::SameLine(); ImGui::InputFloat3("##2", arr4);

        EditTransform(cameraView, cameraProjection, objectMatrix, true);

        End();

        SetSize(Vec2(0.f, 200.f));
        return;

    }



    //play
    CTransform* pTransform = GetTargetObj()->Transform();

    float arrData[3][3] = {};
    Vec3 vData[3] = { pTransform->GetLocalPos(), pTransform->GetLocalScale(), pTransform->GetLocalRot() };

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            arrData[i][j] = vData[i][j];
        }
    }

    Start();
    ImGui::Text("Position\t");  ImGui::SameLine(); ImGui::InputFloat3("##Position", arrData[0], "%.3f");
    ImGui::Text("Scale   \t");  ImGui::SameLine(); ImGui::InputFloat3("##Scale", arrData[1], "%.3f");
    ImGui::Text("Rotation\t");  ImGui::SameLine(); ImGui::InputFloat3("##Rotation", arrData[2], "%.3f");
    End();

    pTransform->SetLocalPos(Vec3(arrData[0]));
    pTransform->SetLocalScale(Vec3(arrData[1]));
    pTransform->SetLocalRot(Vec3(arrData[2]));




}

void TransformGUI::EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
    CTransform* pTransform = GetTargetObj()->Transform();

    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::MODE::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    if (editTransformDecomposition)
    {
        if (ImGui::IsKeyPressed(90))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(69))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(82)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;

        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::Text("Position\t");  ImGui::SameLine(); ImGui::InputFloat3("##Tr", matrixTranslation);
        ImGui::Text("Scale   \t");  ImGui::SameLine(); ImGui::InputFloat3("##Sc", matrixScale);
        ImGui::Text("Rotation\t");  ImGui::SameLine(); ImGui::InputFloat3("##Rt", matrixRotation);
  
       
       

        //pTransform->SetLocalPos(Vec3(matrixTranslation));
        //pTransform->SetLocalScale(Vec3(matrixScale));
       // pTransform->SetLocalRot(Vec3(matrixRotation));

        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed(83))
            useSnap = !useSnap;
        ImGui::Checkbox("", &useSnap);
        ImGui::SameLine();


        switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
        }
        ImGui::Checkbox("Bound Sizing", &boundSizing);
        if (boundSizing)
        {
            ImGui::PushID(3);
            ImGui::Checkbox("", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;

    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    /*
    // 기즈모 생성
    ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

    //matrix 값 배열값으로 분해하여 obj에 set
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);

    pTransform->SetLocalPos(Vec3(matrixTranslation));
    pTransform->SetLocalScale(Vec3(matrixScale));
    pTransform->SetLocalRot(Vec3(matrixRotation[0] / 180.f * XM_PI, matrixRotation[1] / 180.f * XM_PI, matrixRotation[2] / 180.f * XM_PI)); //라디안에서 각도로 바꾸어 넣어주어야 함
    //pTransform->SetLocalRot(Vec3(matrixRotation));  // 라디안 값을 그대로 넣어주면 라디안 값에 대해 그대로 obj에 set되고 다시 받아오는 값 matrix에는 각도에서 라디안값으로 바뀌어서 들어오므로 회전값이 계속 바뀜
    */

    ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
    
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
       ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        pTransform->SetLocalPos(Vec3(matrixTranslation));
        pTransform->SetLocalScale(Vec3(matrixScale));
        pTransform->SetLocalRot(Vec3(matrixRotation[0] / 180.f * XM_PI, matrixRotation[1] / 180.f * XM_PI, matrixRotation[2] / 180.f * XM_PI)); //라디안에서 각도로 바꾸어 넣어주어야 함
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);
    

}
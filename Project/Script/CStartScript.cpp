#include "pch.h"
#include "CStartScript.h"

#include <Engine/CCore.h>
#include <Engine\CGameObject.h>
#include <Engine/CCamera.h>
#include <Engine\CTransform.h>
#include <Engine\CSceneMgr.h>
#include <Engine/CRenderMgr.h>
#include <Engine\CScene.h>
#include <Engine\CCamera.h>
#include <Engine\CEventMgr.h>

#include "CMonsterScript.h"
#include "CSecneChangeScript.h"
CStartScript::CStartScript()
    : CScript((UINT)SCRIPT_TYPE::STARTSCRIPT)
    , m_bNextScene(false)
    , m_fStartTime(0.f)
    ,m_bCreateFlag(false)
    , m_bIsDead(false)
{
}

CStartScript::~CStartScript()
{
}

/*
    0. std2d溅捞歹 眠啊
    1. 皋抛府倔 积己
    2. 橇府崎 积己
    3. startscript积己
    4. monsterscript
    5. start2script积己
*/


void CStartScript::awake()
{
    //button
    but1 = CResMgr::GetInst()->FindRes<CPrefab>(L"startbut");
    button1 = but1->Instantiate();
    button1->SetName(L"startbut");
    button1->Transform()->SetLocalPos(Vec3(-600.f, -100.f, 1.f));
    button1->Transform()->SetLocalScale(Vec3(700, 130, 1.f));
    button1->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

    //tEvent even = {};
    //even.eEvent = EVENT_TYPE::CREATE_OBJECT;
    //even.lParam = (DWORD_PTR)button1;
    //even.wParam = (DWORD_PTR)29;
    //CEventMgr::GetInst()->AddEvent(even);

    //button2
    but2 = CResMgr::GetInst()->FindRes<CPrefab>(L"endbut");
    button2 = but2->Instantiate();
    button2->SetName(L"endbut");
    button2->Transform()->SetLocalPos(Vec3(-600.f, -220.f, 1.f));
    button2->Transform()->SetLocalScale(Vec3(700.f, 130.f, 1.f));
    button2->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

    // even = {};
    //even.eEvent = EVENT_TYPE::CREATE_OBJECT;
    //even.lParam = (DWORD_PTR)button2;
    //even.wParam = (DWORD_PTR)29;
    //CEventMgr::GetInst()->AddEvent(even);

    //logo
    startlogo = CResMgr::GetInst()->FindRes<CPrefab>(L"mainlogo");
    logo = startlogo->Instantiate();
    logo->SetName(L"mainlogo");

    logo->Transform()->SetLocalPos(Vec3(0.f, 0.f, 9.f));
    logo->Transform()->SetLocalScale(Vec3(1600.f, 900.f, 1.f));
    logo->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

    tEvent even = {};
    even.eEvent = EVENT_TYPE::CREATE_OBJECT;
    even.lParam = (DWORD_PTR)logo;
    even.wParam = (DWORD_PTR)29;
    CEventMgr::GetInst()->AddEvent(even);

}

void CStartScript::update()
{
    
    if (!m_bNextScene)
    {
        m_fStartTime += fDT;
        if (m_fStartTime > 2.f)
        {
            tEvent _temp = {};
            _temp.eEvent = EVENT_TYPE::DELETE_OBJECT;
            _temp.lParam = (DWORD_PTR)logo;
            CEventMgr::GetInst()->AddEvent(_temp);

            m_bNextScene = true;
            m_fStartTime = 0.f;
        }
    }
    else
    {
        if (!m_bCreateFlag)
        {        
            tEvent even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)button1;
            even.wParam = (DWORD_PTR)29;
            CEventMgr::GetInst()->AddEvent(even);

            even = {};
            even.eEvent = EVENT_TYPE::CREATE_OBJECT;
            even.lParam = (DWORD_PTR)button2;
            even.wParam = (DWORD_PTR)29;
            CEventMgr::GetInst()->AddEvent(even);
            
           

            m_bCreateFlag = true;
          
        }

        CMonsterScript* pbutton1 = (CMonsterScript*)button1->GetScript(L"CMonsterScript");
        bool ret1 = pbutton1->IsMouseOn();
        if (ret1)
        {
            Ptr<CTexture> pstartbut1 = CResMgr::GetInst()->Load<CTexture>(L"newgamebut2", L"texture\\ui\\edit_2.png");
            button1->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, pstartbut1.Get());
            
            if (KEY_TAP(KEY_TYPE::LBTN))
            {
               
                m_bIsDead = true;
                CSecneChangeScript* Chagne = (CSecneChangeScript*)CSceneMgr::GetInst()->FindObjectByName(L"SceneChagne")->GetScript(L"CSecneChangeScript");
                Chagne->ChangeObjList(ObjList::Law);
               

            }
        }
        else if (ret1 == false)
        {
            Ptr<CTexture> pstartbut1 = CResMgr::GetInst()->Load<CTexture>(L"newgamebut", L"texture\\ui\\edit_3.png");
            button1->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, pstartbut1.Get());
        }


        CMonsterScript* pbutton2 = (CMonsterScript*)button2->GetScript(L"CMonsterScript");
        bool ret2 = pbutton2->IsMouseOn();
        if (ret2)
        {
            Ptr<CTexture> pstartbut2 = CResMgr::GetInst()->Load<CTexture>(L"endbut2", L"texture\\ui\\edit_1.png");
            button2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, pstartbut2.Get());

            if (KEY_TAP(KEY_TYPE::LBTN))
            {
                HWND hWnd = CCore::GetInst()->GetWndHwnd();
                DestroyWindow(hWnd);
            }
        }
        else if (ret2 == false)
        {
            Ptr<CTexture> pstartbut2 = CResMgr::GetInst()->Load<CTexture>(L"endbut", L"texture\\ui\\edit_4.png");
            button2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, pstartbut2.Get());
           
        }
    }
    if (m_bIsDead)
    {
        tEvent even = {};
        even.eEvent = EVENT_TYPE::DELETE_OBJECT;
        even.lParam = (DWORD_PTR)button1;
        CEventMgr::GetInst()->AddEvent(even);

        even = {};
        even.eEvent = EVENT_TYPE::DELETE_OBJECT;
        even.lParam = (DWORD_PTR)button2;
        CEventMgr::GetInst()->AddEvent(even);
        m_bIsDead = false;
    }
}

void CStartScript::SaveToScene(FILE* _pFile)

{
    CScript::SaveToScene(_pFile);
}

void CStartScript::LoadFromScene(FILE* _pFile)
{
    CScript::LoadFromScene(_pFile);
}
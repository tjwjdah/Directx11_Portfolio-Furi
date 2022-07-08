#include "pch.h"
#include "CHpbarScript.h"
#include "pch.h"
#include "CHpbarScript.h"

#include <Engine\CGameObject.h>
#include <Engine/CCamera.h>
#include <Engine\CTransform.h>
#include <Engine\CSceneMgr.h>
#include <Engine/CRenderMgr.h>
#include <Engine\CScene.h>
#include <Engine\CCamera.h>
#include <Engine\CEventMgr.h>

#include "CWiseScript.h"
#include "CPlayerScript.h"
#include "CLawScript.h"
#include "CSecneChangeScript.h"
CHpbarScript::CHpbarScript()
    : CScript((UINT)SCRIPT_TYPE::HPBARSCRIPT)
    , m_BossName(BOSS::LAW)
    , m_hp_player(nullptr)
    , m_hp_boss(nullptr)
    , m_Law(nullptr)
    , m_Player(nullptr)
    , m_Wise(nullptr)
{
}

CHpbarScript::~CHpbarScript()
{
}

void CHpbarScript::awake()
{
   // CSecneChangeScript* Change = (CSecneChangeScript*)CSceneMgr::GetInst()->GetCurScene()->FindObjectByName(L"SceneChagne")->GetScript(L"CSecneChangeScript");
    m_hpPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"hpbar");

    m_hp_player = m_hpPrefab->Instantiate();
    m_hp_player->SetName(L"hp_player");
    m_hp_player->Transform()->SetLocalPos(Vec3(-600, 380, 0));
    m_hp_player->Transform()->SetLocalScale(Vec3(270.f, 12.f, 1.f));

    tEvent even = {};
    even.eEvent = EVENT_TYPE::CREATE_OBJECT;
    even.lParam = (DWORD_PTR)m_hp_player;
    even.wParam = (DWORD_PTR)29;
    CEventMgr::GetInst()->AddEvent(even);

   // Change->ChangeObjPushObj(m_hp_player, ObjList::Hpbar);
    m_lifePrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"lifebar");
    float b = 1.f;
    for (int i = 0; i < 5; i++)
    {
        m_life_player[i] = m_lifePrefab->Instantiate();
        m_life_player[i]->SetName(L"lifebar_player");
        m_life_player[i]->Transform()->SetLocalPos(Vec3(-729 + i * 20.f, 360, 0));
        m_life_player[i]->Transform()->SetLocalScale(Vec3(12.f, 12.f, 1.f));
        m_life_player[i]->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &b);
        //Change->ChangeObjPushObj(m_life_player[i], ObjList::Hpbar);
    }


    for (int i = 0; i < 5; i++)
    {
        even = {};
        even.eEvent = EVENT_TYPE::CREATE_OBJECT;
        even.lParam = (DWORD_PTR)m_life_player[i];
        even.wParam = (DWORD_PTR)29;
        CEventMgr::GetInst()->AddEvent(even);

    }
    //-------------------------------------------------------------------------------------------

    m_hpPrefab2 = CResMgr::GetInst()->FindRes<CPrefab>(L"hpbar2");
    m_hp_boss = m_hpPrefab2->Instantiate();
    m_hp_boss->SetName(L"hp_boss");
    m_hp_boss->Transform()->SetLocalPos(Vec3(600, 380, 0));
    m_hp_boss->Transform()->SetLocalScale(Vec3(270.f, 12.f, 1.f));
   // Change->ChangeObjPushObj(m_hp_boss, ObjList::Hpbar);
    even = {};
    even.eEvent = EVENT_TYPE::CREATE_OBJECT;
    even.lParam = (DWORD_PTR)m_hp_boss;
    even.wParam = (DWORD_PTR)29;
    CEventMgr::GetInst()->AddEvent(even);

    //m_hpPrefab2 = CResMgr::GetInst()->FindRes<CPrefab>(L"lifebar");
    float ab = 1.f;
    for (int i = 0; i < 3; i++)
    {
        m_life_boss[i] = m_lifePrefab->Instantiate();
        m_life_boss[i]->SetName(L"lifebar_player");
        m_life_boss[i]->Transform()->SetLocalPos(Vec3(688 + i * 20.f, 360, 0));
        m_life_boss[i]->Transform()->SetLocalScale(Vec3(12.f, 12.f, 1.f));
        m_life_boss[i]->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &ab);
       /// Change->ChangeObjPushObj(m_life_boss[i], ObjList::Hpbar);
    }


    for (int i = 0; i < 3; i++)
    {
        even = {};
        even.eEvent = EVENT_TYPE::CREATE_OBJECT;
        even.lParam = (DWORD_PTR)m_life_boss[i];
        even.wParam = (DWORD_PTR)29;
        CEventMgr::GetInst()->AddEvent(even);
    }
    //---------------------------------------------------------------------------------------------

    CGameObject* Play = CSceneMgr::GetInst()->FindObjectByName(L"Player");
    m_Player = (CPlayerScript*)Play->GetScript(L"CPlayerScript");


    CGameObject* laww = CSceneMgr::GetInst()->FindObjectByName(L"Law");

    m_Law = (CLawScript*)laww->GetScript(L"CLawScript");

   // CGameObject* Wise = CSceneMgr::GetInst()->FindObjectByName(L"Wise");

   // m_Wise = (CWiseScript*)Wise->GetScript(L"CWiseScript");



}

void CHpbarScript::update()
{
    //--------------------------------------------------------------------------------------------- hp

    float fhp_player = m_Player->GetfHP() / 100.f;
    //보스 추가때문에 스위치 넣음
    float fhp_boss;
    UINT HpState;
    // boss life bar
    int m_flife_boss;

    switch (m_BossName)
    {
    case BOSS::LAW:
    {

        fhp_boss = m_Law->GetLawHp() / 100.f;
        HpState = (UINT)m_Law->GetLawCam(); //0 파랑 1 빨강
        m_flife_boss = m_Law->GetiLife();
    }
    break;
    case BOSS::WISE:
    {
        fhp_boss = m_Wise->GetfHP() / 100.f;
        HpState = (UINT)m_Wise->GetHpColor(); //0 파랑 1 빨강
        m_flife_boss = m_Wise->GetLife();
    }
    break;

    }



    int tt = 0;
    m_hp_player->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::INT_0, &tt);
    m_hp_player->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &fhp_player);  //hp의비율

    m_hp_boss->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::INT_1, &HpState);
    m_hp_boss->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &fhp_boss);  //hp의비율





    //--------------------------------------------------------------------------------------------- life

    // player life bar
    int m_flife_player = m_Player->GetiLife();
    if (m_flife_player < 0 || m_flife_player >= 5) {} //return; //5까지는 노란 life 4부터는 빨간 life
    else
    {
        // 다른 메테리얼로 바꾸어주고 빨간 tex 삽입
        m_life_player[m_flife_player]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"lifebarMtrl2"), 0);
        Ptr<CTexture> hpbartex = CResMgr::GetInst()->Load<CTexture>(L"hp_bar", L"texture\\ui\\hp_bar.png");
        m_life_player[m_flife_player]->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, hpbartex.Get());
    }


    if (m_flife_boss < 0 || m_flife_boss >= 3) {} //return;
    else
    {
        // 다른 메테리얼로 바꾸어주고 빨간 tex 삽입
        Ptr<CTexture> hpbartex = CResMgr::GetInst()->Load<CTexture>(L"hp_bar", L"texture\\ui\\hp_bar.png");
        m_life_boss[m_flife_boss]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"lifebarMtrl2"), 0);
        m_life_boss[m_flife_boss]->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, hpbartex.Get());
    }

}


void CHpbarScript::SaveToScene(FILE* _pFile)
{
    CScript::SaveToScene(_pFile);
}

void CHpbarScript::LoadFromScene(FILE* _pFile)
{
    CScript::LoadFromScene(_pFile);
}
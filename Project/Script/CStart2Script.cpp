#include "pch.h"
#include "CStart2Script.h"
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
#include <Engine/CAnimation2D.h>
#include <Engine/CAnimator2D.h>

#include "CMonsterScript.h"
#include "CSecneChangeScript.h"
CStart2Script::CStart2Script()
    : CScript((UINT)SCRIPT_TYPE::START2SCRIPT)
    , m_iAniNumber(0)
    , m_bMainFlag(false)
    , m_bAniFlag(false)
    , m_bStart(true)
    , m_bIsDead(false)
{
}

CStart2Script::~CStart2Script()
{
}


void CStart2Script::awake()
{
   

}

void CStart2Script::update()
{
    if (m_bStart)
    {
        //logo
        start2logo = CResMgr::GetInst()->FindRes<CPrefab>(L"main2logo");
        logo = start2logo->Instantiate();
        logo->SetName(L"main2logo");

        logo->Transform()->SetLocalPos(Vec3(0.f, 0.f, 9.f));
        logo->Transform()->SetLocalScale(Vec3(1600.f, 900.f, 1.f));
        logo->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

        tEvent even = {};
        even.eEvent = EVENT_TYPE::CREATE_OBJECT;
        even.lParam = (DWORD_PTR)logo;
        even.wParam = (DWORD_PTR)29;
        CEventMgr::GetInst()->AddEvent(even);

        logo->Animator2D()->PlayAnimation(L"MAIN_1", false);
        m_bStart = false;
    }
    
    if (m_bMainFlag == false && logo->Animator2D()->GetCurAnim()->IsFinish() && m_bAniFlag == false)
    {
        if (!m_bAniFlag)
        {
            logo->Animator2D()->PlayAnimation(L"MAIN_2", false);
            m_bAniFlag = true;
        }
    }
    else if (m_bMainFlag == false && m_bAniFlag == true)
    {
        if (logo->Animator2D()->GetCurAnim()->IsFinish())
        {
            m_bMainFlag = true;
            m_bAniFlag = false;
        }
    }
    else if (m_bMainFlag)
    {
        switch (m_iAniNumber)
        {
        case 0:
        {
            if (!m_bAniFlag)
            {
                logo->Animator2D()->PlayAnimation(L"MAIN_3", false);
                m_bAniFlag = true;
            }
            break;
        }
        case 1:
        {
            if (!m_bAniFlag)
            {
                logo->Animator2D()->PlayAnimation(L"MAIN_4", false);
                m_bAniFlag = true;
            }
            break;
        }
        }

        if (logo->Animator2D()->GetCurAnim()->IsFinish())
        {
            if (m_iAniNumber == 0) m_iAniNumber = 1;
            else if (m_iAniNumber == 1) m_iAniNumber = 0;
            m_bAniFlag = false;
        }
    }
    if (KEY_TAP(KEY_TYPE::LBTN))
    {
        m_bIsDead = true;
        CSecneChangeScript* Chagne = (CSecneChangeScript*)CSceneMgr::GetInst()->FindObjectByName(L"SceneChagne")->GetScript(L"CSecneChangeScript");
        Chagne->ChangeObjList(ObjList::Wise);
    }

    if (m_bIsDead)
    {
        m_bIsDead = false;
        tEvent even = {};
        even.eEvent = EVENT_TYPE::DELETE_OBJECT;
        even.lParam = (DWORD_PTR)logo;
        CEventMgr::GetInst()->AddEvent(even);
    }
}

void CStart2Script::SaveToScene(FILE* _pFile)

{
    CScript::SaveToScene(_pFile);
}

void CStart2Script::LoadFromScene(FILE* _pFile)
{
    CScript::LoadFromScene(_pFile);
}
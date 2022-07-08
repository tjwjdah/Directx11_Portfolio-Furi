#include "pch.h"
#include "CImGUIMgr.h"

#include <Engine\CCore.h>
#include <Engine\CDevice.h>
#include <Engine\CSceneMgr.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"
#include "ListGUI.h"
#include "InspectorGUI.h"
#include "MenuGUI.h"

#include "HierachyGUI.h"
#include "ResGUI.h"
#include "DataGUI.h"
#include "MRTGUI.h"
#include "BtnGUI.h"
#include "CCameraAnimGUI.h"

CImGUIMgr::CImGUIMgr()
{

}

CImGUIMgr::~CImGUIMgr()
{
    Safe_Delete_Map(m_mapGUI);
}

void CImGUIMgr::init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

     // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    HWND hWnd = CCore::GetInst()->GetWndHwnd();

    bool bSuccess = ImGui_ImplWin32_Init(hWnd);
    bSuccess = ImGui_ImplDX11_Init(DEVICE, CONTEXT);

    CreateGUI();

    for (const auto& pair : m_mapGUI)
    {
        pair.second->init();
    }
}

void CImGUIMgr::progress()
{
    // callback event 처리
    for (size_t i = 0; i < m_vecEvent.size(); ++i)
    {
        m_vecEvent[i]->Excute();
        delete m_vecEvent[i];
    }
    m_vecEvent.clear();
        
    
    for (auto& pair : m_mapGUI)
    {
        pair.second->update();
    }

    for (auto& pair : m_mapGUI)
    {
        pair.second->lateupdate();
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::SetOrthographic(true); // 기즈모
    ImGuizmo::BeginFrame(); // 기즈모

    for (auto& pair : m_mapGUI)
    {
        if(pair.second->IsActive())
            pair.second->render();
    }

    //bool open = true;
    //ImGui::ShowDemoWindow(&open);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    // DataGUI clear
    DataGUI::clear();
}

GUI* CImGUIMgr::FindGUI(const wstring& _strName)
{
    map<wstring, GUI*>::iterator iter =  m_mapGUI.find(_strName);
    if (iter == m_mapGUI.end())
        return nullptr;

    return iter->second;
}

void CImGUIMgr::AddGUI(const wstring& _strName, GUI* _pGUI)
{
    _pGUI->SetName(_strName);
    m_mapGUI.insert(make_pair(_strName, _pGUI));
}

void CImGUIMgr::CreateGUI()
{
    GUI* pListGUi = new ListGUI;    
    pListGUi->SetName(L"ListGUI");
    pListGUi->DeActivate();
    m_mapGUI.insert(make_pair(L"ListGUI", pListGUi));

    GUI* pGUI = new InspectorGUI;    
    pGUI->SetName(L"Inspector");
    m_mapGUI.insert(make_pair(L"Inspector", pGUI));

    MenuGUI* pMenu = new MenuGUI;
    pMenu->SetName(L"Menu");
    m_mapGUI.insert(make_pair(L"Menu", pMenu));

    HierachyGUI* pHierachy = new HierachyGUI;    
    pHierachy->SetName(L"Hierachy");
    pHierachy->Reset();
    m_mapGUI.insert(make_pair(L"Hierachy", pHierachy));
        
    ResGUI* pResGUI = new ResGUI;
    pResGUI->Reset();
    pResGUI->SetName(L"Resources");
    m_mapGUI.insert(make_pair(L"Resources", pResGUI));

    MRTGUI* pMRTGUI = new MRTGUI;    
    pMRTGUI->SetName(L"MRT");
    pMRTGUI->DeActivate();
    m_mapGUI.insert(make_pair(L"MRT", pMRTGUI));

    /* BtnGUI* pBtnGUI = new BtnGUI;
    pBtnGUI->SetName(L"BtnGUI");    
    m_mapGUI.insert(make_pair(L"BtnGUI", pBtnGUI));*/

    CCameraAnimGUI* pCameraAnim = new CCameraAnimGUI;
    pCameraAnim->SetName(L"CameraAnim");
    m_mapGUI.insert(make_pair(L"CameraAnim", pCameraAnim));
}

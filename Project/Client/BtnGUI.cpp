#include "pch.h"
#include "BtnGUI.h"

#include "imgui.h"

#include <Engine\CPathMgr.h>
#include <Engine\CSceneMgr.h>
#include <Engine\CScene.h>

#include "CSaveLoadMgr.h"
#include "InspectorGUI.h"
#include "CImGUIMgr.h"

BtnGUI::BtnGUI()
	: m_arrIcon{}
{
}

BtnGUI::~BtnGUI()
{
	Safe_Delete_Array(m_arrIcon);
}

void BtnGUI::init()
{
	wstring strFilePath = CPathMgr::GetResPath();

	m_arrIcon[(UINT)SCENE_STATE::PLAY] = new CTexture;
	m_arrIcon[(UINT)SCENE_STATE::PLAY]->Load(strFilePath + L"icon\\Play.png");

	m_arrIcon[(UINT)SCENE_STATE::PAUSE] = new CTexture;
	m_arrIcon[(UINT)SCENE_STATE::PAUSE]->Load(strFilePath + L"icon\\Pause.png");

	m_arrIcon[(UINT)SCENE_STATE::STOP] = new CTexture;
	m_arrIcon[(UINT)SCENE_STATE::STOP]->Load(strFilePath + L"icon\\Stop.png");
}

void BtnGUI::update()
{
}

void BtnGUI::render()
{	
	// 현제 Scene
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();


	bool bOpen = true;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("##BtnGUI", &bOpen, window_flags);

	Vec2 uv_min = Vec2(0.0f, 0.0f);                 // Top-left
	Vec2 uv_max = Vec2(1.0f, 1.0f);                 // Lower-right
	Vec2 teszie = Vec2(50.f, 50.f);				// Texture render size
	Vec4 tint_col = Vec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	Vec4 bg_col = Vec4(1.0f, 1.0f, 1.0f, 0.f); // 50% opaque white


	if (pCurScene->GetState() != SCENE_STATE::PLAY)
	{
		// Play 버튼을 눌렀다.
		if (ImGui::ImageButton(m_arrIcon[(UINT)SCENE_STATE::PLAY]->GetSRV().Get(), teszie, uv_min, uv_max, 1, bg_col, tint_col))
		{
			ChangeSceneState(pCurScene, SCENE_STATE::PLAY);
		}
	}
	else 
	{
		// Pause 버튼을 눌렀다.
		if (ImGui::ImageButton(m_arrIcon[(UINT)SCENE_STATE::PAUSE]->GetSRV().Get(), teszie, uv_min, uv_max, 1, bg_col, tint_col))
		{
			ChangeSceneState(pCurScene, SCENE_STATE::PAUSE);
		}
	}
	
	ImGui::SameLine();

	// 정지버튼을 누른경우	
	if (ImGui::ImageButton(m_arrIcon[(UINT)SCENE_STATE::STOP]->GetSRV().Get(), teszie, uv_min, uv_max, 1, bg_col, tint_col))
	{
		ChangeSceneState(pCurScene, SCENE_STATE::STOP);
	}

	ImGui::End();
}

void BtnGUI::ChangeSceneState(CScene* _pScene, SCENE_STATE _eState)
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

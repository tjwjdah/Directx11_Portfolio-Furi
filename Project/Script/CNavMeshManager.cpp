#include "pch.h"
#include "CNavMeshManager.h"

#include <Engine\CScene.h>
#include <Engine\CLayer.h>
#include <Engine\CMesh.h>
#include <Engine/CMeshRender.h>
#include <Engine\CSceneMgr.h>
CNavMeshManager::CNavMeshManager():CScript((int)SCRIPT_TYPE::NAVMESHMANAGER)
{
}

CNavMeshManager::~CNavMeshManager()
{
}

void CNavMeshManager::update()
{
}

void CNavMeshManager::awake()
{
	NavMeshVector();
}

void CNavMeshManager::NavMeshVector()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	const vector<CGameObject*>& vecObj = pCurScene->GetLayer(0)->GetObjects(); 
	//씬의 모든 오브젝트들 소환
	for (size_t i = 0; i < vecObj.size(); ++i)
	{
		if (vecObj[i]->MeshRender())
		{
			if (vecObj[i]->MeshRender()->GetMesh()->IsNavMesh()) //네비메쉬일때만
			{
				//네비메쉬의 매트릭스 값 불러와서
				//정점마다 스케일, 회전값 이동값 곱해줌.(행렬)
				vecObj[i]->MeshRender()->SetNavMeshTrans(); 
				
				m_NavMeshVector.push_back(vecObj[i]->MeshRender()->GetMesh()->GetNavData()); 
				//네미메쉬데이터를 벡터에 저장
			}
		}
	}

}
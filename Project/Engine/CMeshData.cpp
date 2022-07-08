#include "pch.h"
#include "CMeshData.h"

#include "CPathMgr.h"
#include "CMesh.h"
#include "CResMgr.h"
#include "CMaterial.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CGameObject.h"
#include "CAnimator3D.h"
#include "CScript.h"
CMeshData::CMeshData()
{
}

CMeshData::~CMeshData()
{
}


CGameObject* CMeshData::Instantiate()
{
	// Mesh
	// Material
	
	CGameObject* pRootObj = new CGameObject;
	pRootObj->AddComponent(new CTransform);
	//pRootObj->AddComponent(new CMeshRender);
	size_t MeshRinkSize = m_vMeshRink.size();
	for (size_t i = 0; i < MeshRinkSize; i++)
	{
		wstring wstr(m_vMeshRink[i].s_NodeName.begin(), m_vMeshRink[i].s_NodeName.end());
		CGameObject* pChildObj = new CGameObject;
		pChildObj->AddComponent(new CTransform);
		pChildObj->SetDynamicShadow(true);
		pChildObj->SetName(wstr);
		pChildObj->AddComponent(new CMeshRender);
		size_t MeshDataSize = m_MeshData.size();
		for (size_t l = 0; l < MeshDataSize; l++)
		{
			string MeshName = GetString(m_MeshData[l].m_pMesh.Get()->GetName());
			if (MeshName == m_vMeshRink[i].s_MeshName)
			{
				pChildObj->MeshRender()->SetMesh(m_MeshData[l].m_pMesh);
				for (size_t j = 0; j < m_MeshData[l].m_vecMtrl.size(); ++j)
				{
					pChildObj->MeshRender()->SetMaterial(m_MeshData[l].m_vecMtrl[j], j);
				}

				if (true == pChildObj->MeshRender()->GetMesh()->IsAnimMesh())
				{
					CAnimator3D* pAnimator = new CAnimator3D;
					pChildObj->AddComponent(pAnimator);
					UINT BoneSize = pChildObj->MeshRender()->GetMesh()->GetBoneSize();
					for (UINT zz = 0; zz < BoneSize; zz++)
						pAnimator->SetBones(pChildObj->MeshRender()->GetMesh()->GetBones(zz));
					pAnimator->SetAnimClip(pChildObj->MeshRender()->GetMesh()->GetAnimClip());
				}

				break;
			}
		}
		pRootObj->AddChild(pChildObj);
	}
		
	/*
	wstring wstr(m_vMeshRink[0].s_ParentNode.begin(), m_vMeshRink[0].s_ParentNode.end());
	pRootObj->SetName(wstr);
	size_t MeshRinkSize = m_vMeshRink.size();
	for (size_t i = 0; i < MeshRinkSize; i++)
	{
		wstring wstr1(m_vMeshRink[i].s_ParentNode.begin(), m_vMeshRink[i].s_ParentNode.end());
		CGameObject* FindObj = pRootObj->GetChild(wstr1);

		CGameObject* pChildObj = new CGameObject;
		pChildObj->AddComponent(new CTransform);
		
		pChildObj->Transform()->SetLocalPos(m_vMeshRink[i].v_ChildPos);
		pChildObj->Transform()->SetLocalScale(m_vMeshRink[i].v_ChildScale);
		pChildObj->Transform()->SetLocalRot(m_vMeshRink[i].v_ChildRot);
		wstring wstr2(m_vMeshRink[i].s_ChildNode.begin(), m_vMeshRink[i].s_ChildNode.end());
		pChildObj->SetName(wstr2);

		if (m_vMeshRink[i].b_ChildMesh)
		{
			pChildObj->AddComponent(new CMeshRender);

			size_t MeshDataSize = m_MeshData.size();
			for (size_t l = 0; l < MeshDataSize; l++)
			{
				string MeshName = GetString(m_MeshData[l].m_pMesh.Get()->GetName());
				if (MeshName == m_vMeshRink[i].s_ChildMesh)
				{

					pChildObj->MeshRender()->SetMesh(m_MeshData[l].m_pMesh);
					for (size_t j = 0; j < m_MeshData[l].m_vecMtrl.size(); ++j)
					{
						pChildObj->MeshRender()->SetMaterial(m_MeshData[l].m_vecMtrl[j], j);
					}

					if (true == pChildObj->MeshRender()->GetMesh()->IsAnimMesh())
					{ 
						CAnimator3D* pAnimator = new CAnimator3D;
						pChildObj->AddComponent(pAnimator);
						UINT BoneSize = pChildObj->MeshRender()->GetMesh()->GetBoneSize();
						for(UINT zz=0; zz< BoneSize;zz++)
							pAnimator->SetBones(pChildObj->MeshRender()->GetMesh()->GetBones(zz));
						pAnimator->SetAnimClip(pChildObj->MeshRender()->GetMesh()->GetAnimClip());
					}
					
					break;
				}
			}	
		}
		if (FindObj !=nullptr)
			FindObj->AddChild(pChildObj);

	}
	//pNewObj->MeshRender()->SetMesh(m_pMesh);

	/*for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		pNewObj->MeshRender()->SetMaterial(m_vecMtrl[i], i);
	}

	if (false == m_pMesh->IsAnimMesh())
		return pNewObj;

	CAnimator3D* pAnimator = new CAnimator3D;
	pNewObj->AddComponent(pAnimator);

	pAnimator->SetBones(m_pMesh->GetBones());
	pAnimator->SetAnimClip(m_pMesh->GetAnimClip());

	return pNewObj;
	*/

	//pRootObj;
	//while (true)
	//{
		//pRootObj
	//}
	
	
	return pRootObj;
}

CMeshData* CMeshData::LoadFromFBX(const wstring& _strPath, LOADFROMFBXTYPE _Type)
{
	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += _strPath;

	CFBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath, _Type);
	
	CMeshData* pMeshData = new CMeshData;

	size_t RinkCount = g_MeshRink.size();
	for (size_t i = 0; i < RinkCount; i++)
	{
		pMeshData->m_vMeshRink.push_back(g_MeshRink[i]);
	}

	g_MeshRink.clear();

	int ContainerCount = loader.GetContainerCount();
	for (int l = 0; l < ContainerCount; l++)
	{
		CMesh* pMesh;
		if(_Type == LOADFROMFBXTYPE::NAV)
			pMesh = CMesh::CreateFromContainer(loader, l,true);
		else
			pMesh = CMesh::CreateFromContainer(loader, l, false);
		
		// ResMgr 에 메쉬 등록
		wstring strMeshName = L"mesh\\";
		strMeshName += loader.GetContainer(l).strName;
		//strMeshName += path(strFullPath).stem();
		strMeshName += L".mesh";

		pMesh->SetName(strMeshName);
		pMesh->SetRelativePath(strMeshName);

		CResMgr::GetInst()->AddRes<CMesh>(pMesh->GetName(), pMesh);

		vector<Ptr<CMaterial>> vecMtrl;

		// 메테리얼 가져오기
		for (UINT i = 0; i < loader.GetContainer(l).vecMtrl.size(); ++i)
		{
			// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(l).vecMtrl[i].strMtrlName);
			vecMtrl.push_back(pMtrl);
		}


		MeshMaterial MM;
		MM.m_pMesh = pMesh;
		MM.m_vecMtrl = vecMtrl;
		pMeshData->m_MeshData.push_back(MM);
	}


	return pMeshData;

	/*
	// 메쉬 가져오기
	CMesh* pMesh = CMesh::CreateFromContainer(loader);

	// ResMgr 에 메쉬 등록
	wstring strMeshName = L"mesh\\";
	strMeshName += path(strFullPath).stem();
	strMeshName += L".mesh";

	pMesh->SetName(strMeshName);
	pMesh->SetRelativePath(strMeshName);

	CResMgr::GetInst()->AddRes<CMesh>(pMesh->GetName(), pMesh);

	vector<Ptr<CMaterial>> vecMtrl;

	// 메테리얼 가져오기
	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(0).vecMtrl[i].strMtrlName);
		vecMtrl.push_back(pMtrl);
	}

	CMeshData* pMeshData = new CMeshData;
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
	*/
}

void CMeshData::Save(const wstring& _strFilePath)
{
	wstring strFileName = CPathMgr::GetResPath();
	strFileName += _strFilePath;
	SetRelativePath(_strFilePath);

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"wb");

	UINT MeshMaterialCount = (UINT)m_MeshData.size();
	fwrite(&MeshMaterialCount, sizeof(UINT), 1, pFile);
	UINT i = 0;
	for (; i < MeshMaterialCount; i++)
	{
		// Mesh 를 파일로 저장
		m_MeshData[i].m_pMesh->Save(m_MeshData[i].m_pMesh->GetRelativePath());
		// Mesh Key 저장	
	    // Mesh Data 저장
		SaveWString(m_MeshData[i].m_pMesh->GetKey(), pFile);
		SaveWString(m_MeshData[i].m_pMesh->GetRelativePath(), pFile);


		// material 정보 저장
		UINT iMtrlCount = (UINT)m_MeshData[i].m_vecMtrl.size();
		//fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);
		UINT l = 0;
		for (; l < m_MeshData[i].m_vecMtrl.size(); ++l)
		{
			if (nullptr == m_MeshData[i].m_vecMtrl[l])
				iMtrlCount--;
				
		}
		fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);
		for (l=0; l < iMtrlCount; ++l)
		{
			if (nullptr == m_MeshData[i].m_vecMtrl[l])
				continue;
			// Material 을 파일로 저장
			m_MeshData[i].m_vecMtrl[l]->Save(m_MeshData[i].m_vecMtrl[l]->GetRelativePath());

			// Material 인덱스, Key, Path 저장
			fwrite(&l, sizeof(UINT), 1, pFile);
			SaveWString(m_MeshData[i].m_vecMtrl[l]->GetKey(), pFile);
			SaveWString(m_MeshData[i].m_vecMtrl[l]->GetRelativePath(), pFile);

		}

		
	}
	
	i = 0;
	UINT m_vMeshRinkCount = (UINT)m_vMeshRink.size();
	fwrite(&m_vMeshRinkCount, sizeof(UINT), 1, pFile);
	for (; i < m_vMeshRinkCount; i++)
	{
		SaveStringg(m_vMeshRink[i].s_MeshName, pFile);
		SaveStringg(m_vMeshRink[i].s_NodeName, pFile);
		/*
		SaveStringg(m_vMeshRink[i].s_ParentNode,pFile);
		SaveStringg(m_vMeshRink[i].s_ParentMesh, pFile);
		fwrite(&m_vMeshRink[i].b_ParentMesh, sizeof(bool), 1, pFile);
		SaveStringg(m_vMeshRink[i].s_ChildNode, pFile);
		SaveStringg(m_vMeshRink[i].s_ChildMesh, pFile);
		fwrite(&m_vMeshRink[i].b_ChildMesh, sizeof(bool), 1, pFile);
		fwrite(&m_vMeshRink[i].v_ChildPos, sizeof(Vec3), 1, pFile);
		fwrite(&m_vMeshRink[i].v_ChildScale, sizeof(Vec3), 1, pFile);
		fwrite(&m_vMeshRink[i].v_ChildRot, sizeof(Vec3), 1, pFile);

		*/
	}
	

	fclose(pFile);
}



void CMeshData::Load(const wstring& _strFilePath)
{
	FILE* pFile = NULL;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// Mesh Load
	UINT MeshMaterialCount = 0;
	fread(&MeshMaterialCount, sizeof(UINT), 1, pFile);
	m_MeshData.resize(MeshMaterialCount);
	for (UINT i = 0; i < MeshMaterialCount; i++)
	{
		wstring strMeshKey, strMeshPath;
		LoadWString(strMeshKey, pFile);
		LoadWString(strMeshPath, pFile);
		m_MeshData[i].m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(strMeshKey);
		if (nullptr == m_MeshData[i].m_pMesh)
			m_MeshData[i].m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMeshKey, strMeshPath);

		UINT iMtrlCount = 0;
		fread(&iMtrlCount, sizeof(UINT), 1, pFile);
		
		m_MeshData[i].m_vecMtrl.resize(iMtrlCount);

		for (UINT l = 0; l < iMtrlCount; ++l)
		{
			UINT idx = -1;
			fread(&idx, 4, 1, pFile);
	
			wstring strKey, strPath;
			LoadWString(strKey, pFile);
			LoadWString(strPath, pFile);

			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(strKey);
			if (nullptr == pMtrl)
				pMtrl = CResMgr::GetInst()->Load<CMaterial>(strKey, strPath);

			m_MeshData[i].m_vecMtrl[idx] = pMtrl;
		}
	}
	/*
	wstring strMeshKey, strMeshPath;
	LoadWString(strMeshKey, pFile);
	LoadWString(strMeshPath, pFile);
	m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(strMeshKey);
	if (nullptr == m_pMesh)
		m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMeshKey, strMeshPath);

	// material 정보 읽기
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(UINT), 1, pFile);

	m_vecMtrl.resize(iMtrlCount);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		UINT idx = -1;
		fread(&idx, 4, 1, pFile);
		if (idx == -1)
			break;

		wstring strKey, strPath;
		LoadWString(strKey, pFile);
		LoadWString(strPath, pFile);

		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(strKey);
		if (nullptr == pMtrl)
			pMtrl = CResMgr::GetInst()->Load<CMaterial>(strKey, strPath);

		m_vecMtrl[i] = pMtrl;
	}
	*/
	
	UINT m_vMeshRinkCount = 0;
	fread(&m_vMeshRinkCount, sizeof(UINT), 1, pFile);
	m_vMeshRink.resize(m_vMeshRinkCount);
	for (UINT i=0; i < m_vMeshRinkCount; i++)
	{


		LoadStringg(m_vMeshRink[i].s_MeshName, pFile);
		LoadStringg(m_vMeshRink[i].s_NodeName, pFile);
		/*
		LoadStringg(m_vMeshRink[i].s_ParentNode, pFile);
		LoadStringg(m_vMeshRink[i].s_ParentMesh, pFile);
		fread(&m_vMeshRink[i].b_ParentMesh, sizeof(bool), 1, pFile);
		LoadStringg(m_vMeshRink[i].s_ChildNode, pFile);
		LoadStringg(m_vMeshRink[i].s_ChildMesh, pFile);
		fread(&m_vMeshRink[i].b_ChildMesh, sizeof(bool), 1, pFile);
		fread(&m_vMeshRink[i].v_ChildPos, sizeof(Vec3), 1, pFile);
		fread(&m_vMeshRink[i].v_ChildScale, sizeof(Vec3), 1, pFile);
		fread(&m_vMeshRink[i].v_ChildRot, sizeof(Vec3), 1, pFile);

		*/
	}


	fclose(pFile);
}



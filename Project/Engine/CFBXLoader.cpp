#include "pch.h"
#include "CFBXLoader.h"
#include "CMesh.h"

#include "CResMgr.h"
#include "CPathMgr.h"

CFBXLoader::CFBXLoader()
	: m_pManager(NULL)
	, m_pScene(NULL)
	, m_pImporter(NULL)
	, m_CameraAnim(false)
{
}

CFBXLoader::~CFBXLoader()
{
	m_pScene->Destroy();
	m_pManager->Destroy();
		
	for (size_t i = 0; i < m_vecBone.size(); ++i)
	{
		for (size_t l = 0; l < m_vecBone[i].size(); ++l)
		{
			SAFE_DELETE(m_vecBone[i][l]);
		}
	}

	for (size_t i = 0; i < m_vecAnimClip.size(); ++i)
	{
		SAFE_DELETE(m_vecAnimClip[i]);
	}

	for (int i = 0; i < m_arrAnimName.Size(); ++i)
	{
		SAFE_DELETE(m_arrAnimName[i]);
	}
}

void CFBXLoader::init()
{
	m_pManager = FbxManager::Create();
	if (NULL == m_pManager)
		assert(NULL);

	FbxIOSettings* pIOSettings = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(pIOSettings);

	m_pScene = FbxScene::Create(m_pManager, "");
	if (NULL == m_pScene)
		assert(NULL);
}

void CFBXLoader::LoadFbx(const wstring & _strPath, LOADFROMFBXTYPE _Type)
{
	m_vecContainer.clear();
	if (_Type == LOADFROMFBXTYPE::CAMERA)
		m_CameraAnim = true;;
	m_pImporter = FbxImporter::Create(m_pManager, "");

	//wstring str = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(strName.c_str());
	string strPath(_strPath.begin(), _strPath.end());
		
	if (!m_pImporter->Initialize(strPath.c_str(), -1, m_pManager->GetIOSettings()))
		assert(nullptr);
	
	m_pImporter->Import(m_pScene);

	/*FbxAxisSystem originAxis = FbxAxisSystem::eMax;
	originAxis = m_pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem DesireAxis = FbxAxisSystem::DirectX;
	DesireAxis.ConvertScene(m_pScene);
	originAxis = m_pScene->GetGlobalSettings().GetAxisSystem();*/
	
	m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);


	m_pScene->FillAnimStackNameArray(m_arrAnimName);
	// Bone 정보 읽기
	LoadSkeleton(m_pScene->GetRootNode());

	// Animation 이름정보 
	

	// Animation Clip 정보
	LoadAnimationClip();
	
	// 삼각화(Triangulate)
	Triangulate(m_pScene->GetRootNode());
	
	// 메쉬 데이터 얻기
	LoadMeshDataFromNode(m_pScene->GetRootNode());

	m_pImporter->Destroy();
	
	m_pScene->GetAnimationEvaluator();
	// 필요한 텍스쳐 로드
	LoadTexture();

	// 필요한 메테리얼 생성
	CreateMaterial();
	if (m_CameraAnim)
		SaveCameraClip();
}

void CFBXLoader::SaveCameraClip()
{
	FILE* pClipNameListFile = nullptr;
	wstring strFileName = CPathMgr::GetResPath();
	wstring name = strFileName + L"CamearaClip\\CameraClipData.Camera";
	_wfopen_s(&pClipNameListFile, name.c_str(), L"wb");

	UINT ClipSize = m_CameraData.size();
	fwrite(&ClipSize, sizeof(int), 1, pClipNameListFile);
	for (UINT i = 0; i < m_CameraData.size(); i++)
	{
		bool ca = m_CameraData[i].m_bnCamera;
		fwrite(&ca, sizeof(bool), 1, pClipNameListFile);
		wstring caname = m_CameraData[i].ClipName;
		SaveWString(caname, pClipNameListFile);
		UINT TranslationSize = m_CameraData[i].bnC_Translation.size();
		fwrite(&TranslationSize, sizeof(int), 1, pClipNameListFile);
		for (UINT l = 0; l < TranslationSize; l++)
		{
			Vec3 cv3 = m_CameraData[i].bnC_Translation[l];
			fwrite(&cv3, sizeof(Vec3), 1, pClipNameListFile);
		}
		for (UINT l = 0; l < TranslationSize; l++)
		{
			Vec3 cv3 = m_CameraData[i].bnC_Scali[l];
			fwrite(&cv3, sizeof(Vec3), 1, pClipNameListFile);
		}
		for (UINT l = 0; l < TranslationSize; l++)
		{
			Vec3 cv3 = m_CameraData[i].bnC_Rotation[l];
			fwrite(&cv3, sizeof(Vec3), 1, pClipNameListFile);
		}
	}
	fclose(pClipNameListFile);
	//m_CameraData
}

void CFBXLoader::LoadMeshDataFromNode(FbxNode * _pNode)
{
	// 노드의 메쉬정보 읽기
	FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();
	string a =_pNode->GetName();


	
	if (pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType())
	{
		FbxMesh* pMesh = _pNode->GetMesh();
		if (NULL != pMesh)
			LoadMesh(pMesh, _pNode->GetName());
	}

	// 해당 노드의 재질정보 읽기
	UINT iMtrlCnt = _pNode->GetMaterialCount();
	if (iMtrlCnt > 0)
	{
		for (UINT i = 0; i < iMtrlCnt; ++i)
		{
			FbxSurfaceMaterial* pMtrlSur = _pNode->GetMaterial(i);
			LoadMaterial(pMtrlSur);
		}
	}

	
	if (_pNode->GetMesh() != NULL)
	{
		tMeshRink Mesh_Name;
		//Mesh_Name.b_ParentMesh = true;
		string strMeshName = "mesh\\";
		strMeshName += _pNode->GetName();
		strMeshName += ".mesh";
		Mesh_Name.s_MeshName = strMeshName;
		Mesh_Name.s_NodeName= _pNode->GetName();
		//s_MeshName
		g_MeshRink.push_back(Mesh_Name);
	}
	
	//FbxDouble3 dd = _pNode->LclScaling.Get();
	//FbxDouble3 dd2 = _pNode->LclTranslation.Get();
	// 자식 노드 정보 읽기
	int iChildCnt = _pNode->GetChildCount();
	for (int i = 0; i < iChildCnt; ++i)
	{
		//Mesh_Name.s_ParentNode = _pNode->GetName();
		//Mesh_Name.s_ChildNode = _pNode->GetChild(i)->GetName();
		
		//FbxDouble3 FbxPos =_pNode->GetChild(i)->LclTranslation.Get();
		//FbxDouble3 FbxScl = _pNode->GetChild(i)->LclScaling.Get();
		//FbxDouble3 FbxRot = _pNode->GetChild(i)->LclRotation.Get();
		/*
		if (_pNode->GetMesh() != NULL)
		{
			Mesh_Name.b_ParentMesh = true;
			string strMeshName = "mesh\\";
			strMeshName += _pNode->GetName();
			strMeshName += ".mesh";
			Mesh_Name.s_ParentMesh = strMeshName;
			
		}
		else
		{
			Mesh_Name.b_ParentMesh = false;
		}
		if (_pNode->GetChild(i)->GetMesh() != NULL)
		{
			Mesh_Name.b_ChildMesh = true;
			string strMeshName = "mesh\\";
			strMeshName += _pNode->GetChild(i)->GetName();
			strMeshName += ".mesh";
			Mesh_Name.s_ChildMesh = strMeshName;
		}
		else
		{
			Mesh_Name.b_ChildMesh = false;
		}
		g_MeshRink.push_back(Mesh_Name);
		*/
		LoadMeshDataFromNode(_pNode->GetChild(i));
	}
}

void CFBXLoader::LoadMesh(FbxMesh * _pFbxMesh, const string& Name)
{
	m_vecContainer.push_back(tContainer{});
	tContainer& Container = m_vecContainer[m_vecContainer.size() - 1];
	
	string strName = Name;
	Container.strName = wstring(strName.begin(), strName.end());

	int iVtxCnt = _pFbxMesh->GetControlPointsCount();
	Container.Resize(iVtxCnt);

	FbxVector4* pFbxPos = _pFbxMesh->GetControlPoints();

	for (int i = 0; i < iVtxCnt; ++i)
	{
		Container.vecPos[i].x = (float)pFbxPos[i].mData[0];
		Container.vecPos[i].y = (float)pFbxPos[i].mData[1];
		Container.vecPos[i].z = (float)pFbxPos[i].mData[2];
		//쓰리디 맥스면은 
		///Container.vecPos[i].x = (float)pFbxPos[i].mData[0];
		//Container.vecPos[i].y = (float)pFbxPos[i].mData[2];
		//Container.vecPos[i].z = (float)pFbxPos[i].mData[1];
	}
	
	// 폴리곤 개수
	int iPolyCnt = _pFbxMesh->GetPolygonCount();

	// 재질의 개수 ( ==> SubSet 개수 ==> Index Buffer Count)
	int iMtrlCnt = _pFbxMesh->GetNode()->GetMaterialCount();
	Container.vecIdx.resize(iMtrlCnt);	

	// 정점 정보가 속한 subset 을 알기위해서...
	FbxGeometryElementMaterial* pMtrl = _pFbxMesh->GetElementMaterial();

	// 폴리곤을 구성하는 정점 개수
	int iPolySize = _pFbxMesh->GetPolygonSize(0);
	if (3 != iPolySize)
		assert(NULL); // Polygon 구성 정점이 3개가 아닌 경우

	UINT arrIdx[3] = {};
	UINT iVtxOrder = 0; // 폴리곤 순서로 접근하는 순번

	for (int i = 0; i < iPolyCnt; ++i)
	{
		for (int j = 0; j < iPolySize; ++j)
		{
			// i 번째 폴리곤에, j 번째 정점
			int iIdx = _pFbxMesh->GetPolygonVertex(i, j);
			arrIdx[j] = iIdx;

			GetTangent(_pFbxMesh, &Container, iIdx, iVtxOrder );
			GetBinormal(_pFbxMesh, &Container, iIdx, iVtxOrder);
			GetNormal(_pFbxMesh, &Container, iIdx, iVtxOrder);
			GetUV(_pFbxMesh, &Container, iIdx, _pFbxMesh->GetTextureUVIndex(i, j));
			
			++iVtxOrder;
		}
		UINT iSubsetIdx = pMtrl->GetIndexArray().GetAt(i);
		Container.vecIdx[iSubsetIdx].push_back(arrIdx[0]);
		Container.vecIdx[iSubsetIdx].push_back(arrIdx[2]);
		Container.vecIdx[iSubsetIdx].push_back(arrIdx[1]);
	}

	LoadAnimationData(_pFbxMesh, &Container);
}

void CFBXLoader::LoadMaterial(FbxSurfaceMaterial * _pMtrlSur)
{
	tFbxMaterial tMtrlInfo{};

	string str = _pMtrlSur->GetName();
	tMtrlInfo.strMtrlName = wstring(str.begin(), str.end());
	//tMtrlInfo.tMtrl.vEmis
	// Diff
	tMtrlInfo.tMtrl.vDiff = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sDiffuse
		, FbxSurfaceMaterial::sDiffuseFactor);

	// Amb
	tMtrlInfo.tMtrl.vAmb = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sAmbient
		, FbxSurfaceMaterial::sAmbientFactor);

	// Spec
	tMtrlInfo.tMtrl.vSpec = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sSpecular
		, FbxSurfaceMaterial::sSpecularFactor);

	// Emisv
	tMtrlInfo.tMtrl.vEmis = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sEmissive
		, FbxSurfaceMaterial::sEmissiveFactor);

	// Texture Name
	tMtrlInfo.strDiff = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sDiffuse);
	tMtrlInfo.strNormal = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sNormalMap);
	tMtrlInfo.strSpec = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sSpecular);
	tMtrlInfo.strEmis = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sEmissive);
	m_vecContainer.back().vecMtrl.push_back(tMtrlInfo);
}

void CFBXLoader::GetTangent(FbxMesh * _pMesh
	, tContainer * _pContainer
	, int _iIdx		 /*해당 정점의 인덱스*/
	, int _iVtxOrder /*폴리곤 단위로 접근하는 순서*/)
{
	int iTangentCnt = _pMesh->GetElementTangentCount();
	if (1 != iTangentCnt)
		assert(NULL); // 정점 1개가 포함하는 탄젠트 정보가 2개 이상이다.

	// 탄젠트 data 의 시작 주소
	FbxGeometryElementTangent* pTangent = _pMesh->GetElementTangent();
	UINT iTangentIdx = 0;
	
	FbxLayerElement::EMappingMode mode = pTangent->GetMappingMode();

	if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			iTangentIdx = _iVtxOrder;
		else
			iTangentIdx = pTangent->GetIndexArray().GetAt(_iVtxOrder);
	}
	else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			iTangentIdx = _iIdx;
		else
			iTangentIdx = pTangent->GetIndexArray().GetAt(_iIdx);
	}

	FbxVector4 vTangent = pTangent->GetDirectArray().GetAt(iTangentIdx);
	
	_pContainer->vecTangent[_iIdx].x = (float)vTangent.mData[0];
	_pContainer->vecTangent[_iIdx].y = (float)vTangent.mData[2];
	_pContainer->vecTangent[_iIdx].z = (float)vTangent.mData[1];
}

void CFBXLoader::GetBinormal(FbxMesh * _pMesh, tContainer * _pContainer, int _iIdx, int _iVtxOrder)
{
	int iBinormalCnt = _pMesh->GetElementBinormalCount();
	if (1 != iBinormalCnt)
		assert(NULL); // 정점 1개가 포함하는 종법선 정보가 2개 이상이다.

	// 종법선 data 의 시작 주소
	FbxGeometryElementBinormal* pBinormal = _pMesh->GetElementBinormal();
	UINT iBinormalIdx = 0;

	if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iBinormalIdx = _iVtxOrder;
		else
			iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iVtxOrder);
	}
	else if (pBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iBinormalIdx = _iIdx;
		else
			iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iIdx);
	}

	FbxVector4 vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIdx);

	_pContainer->vecBinormal[_iIdx].x = (float)vBinormal.mData[0];
	_pContainer->vecBinormal[_iIdx].y = (float)vBinormal.mData[2];
	_pContainer->vecBinormal[_iIdx].z = (float)vBinormal.mData[1];
}

void CFBXLoader::GetNormal(FbxMesh * _pMesh, tContainer * _pContainer, int _iIdx, int _iVtxOrder)
{
	int iNormalCnt = _pMesh->GetElementNormalCount();
	if (1 != iNormalCnt)
		assert(NULL); // 정점 1개가 포함하는 종법선 정보가 2개 이상이다.

					  // 종법선 data 의 시작 주소
	FbxGeometryElementNormal* pNormal = _pMesh->GetElementNormal();
	UINT iNormalIdx = 0;

	if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iNormalIdx = _iVtxOrder;
		else
			iNormalIdx = pNormal->GetIndexArray().GetAt(_iVtxOrder);
	}
	else if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iNormalIdx = _iIdx;
		else
			iNormalIdx = pNormal->GetIndexArray().GetAt(_iIdx);
	}

	FbxVector4 vNormal = pNormal->GetDirectArray().GetAt(iNormalIdx);

	_pContainer->vecNormal[_iIdx].x = (float)vNormal.mData[0];
	_pContainer->vecNormal[_iIdx].y = (float)vNormal.mData[2];
	_pContainer->vecNormal[_iIdx].z = (float)vNormal.mData[1];
}

void CFBXLoader::GetUV(FbxMesh * _pMesh, tContainer * _pContainer, int _iIdx, int _iUVIndex)
{
	FbxGeometryElementUV* pUV = _pMesh->GetElementUV();

	UINT iUVIdx = 0;
	if (pUV->GetReferenceMode() == FbxGeometryElement::eDirect)
		iUVIdx = _iIdx;
	else
		iUVIdx = pUV->GetIndexArray().GetAt(_iIdx);

	iUVIdx = _iUVIndex;
	FbxVector2 vUV = pUV->GetDirectArray().GetAt(iUVIdx);
	_pContainer->vecUV[_iIdx].x = (float)vUV.mData[0];
	_pContainer->vecUV[_iIdx].y = 1.f - (float)vUV.mData[1]; // fbx uv 좌표계는 좌하단이 0,0
	//_pContainer->vecUV[_iIdx].y = (float)vUV.mData[1]; // fbx uv 좌표계는 좌하단이 0,0
}

Vec4 CFBXLoader::GetMtrlData(FbxSurfaceMaterial * _pSurface
	, const char * _pMtrlName
	, const char * _pMtrlFactorName)
{
	FbxDouble3  vMtrl;
	FbxDouble	dFactor = 0.;

	FbxProperty tMtrlProperty = _pSurface->FindProperty(_pMtrlName);
	FbxProperty tMtrlFactorProperty = _pSurface->FindProperty(_pMtrlFactorName);

	if (tMtrlProperty.IsValid() && tMtrlFactorProperty.IsValid())
	{
		vMtrl = tMtrlProperty.Get<FbxDouble3>();
		dFactor = tMtrlFactorProperty.Get<FbxDouble>();
	}

	Vec4 vRetVal = Vec4((float)vMtrl.mData[0] * (float)dFactor, (float)vMtrl.mData[1] * (float)dFactor, (float)vMtrl.mData[2] * (float)dFactor, (float)dFactor);
	return vRetVal;
}

wstring CFBXLoader::GetMtrlTextureName(FbxSurfaceMaterial * _pSurface, const char * _pMtrlProperty)
{
	string strName;

	FbxProperty TextureProperty = _pSurface->FindProperty(_pMtrlProperty);
	if (TextureProperty.IsValid())
	{
		UINT iCnt = TextureProperty.GetSrcObjectCount();

		if (1 <= iCnt)
		{
			FbxFileTexture* pFbxTex = TextureProperty.GetSrcObject<FbxFileTexture>(0);
			if (NULL != pFbxTex)
				strName = pFbxTex->GetFileName();
		}
	}
	path ppath = CPathMgr::GetResPath();
	path pname = strName;
	ppath += pname.filename();
	strName = ppath.string();
	return wstring(strName.begin(), strName.end());
}

void CFBXLoader::LoadTexture()
{
	path path_content = CPathMgr::GetResPath();

	path path_fbx_texture = path_content.wstring() + L"texture\\FBXTexture\\";
	if (false == exists(path_fbx_texture))
	{
		create_directory(path_fbx_texture);
	}	
	
	path path_origin;
	path path_filename;
	path path_dest;

	for (UINT i = 0; i < m_vecContainer.size(); ++i)
	{
		for (UINT j = 0; j < m_vecContainer[i].vecMtrl.size(); ++j)
		{
			vector<path> vecPath;
			vecPath.push_back(m_vecContainer[i].vecMtrl[j].strDiff.c_str());
			vecPath.push_back(m_vecContainer[i].vecMtrl[j].strNormal.c_str());
			
			vecPath.push_back(m_vecContainer[i].vecMtrl[j].strSpec.c_str());

			for (size_t k = 0; k < vecPath.size(); ++k)
			{
				
				path_origin = vecPath[k];
				path_filename = vecPath[k].filename();
				path_dest = path_fbx_texture.wstring() + path_filename.wstring();
				path test = path_fbx_texture;
				test += L".";
				if (path_dest == test.wstring())
				{
					continue;
				}

				if (false == exists(path_dest))
				{
					copy(path_origin, path_dest);
				}

				path_dest = CPathMgr::GetRelativePath(path_dest);
				CResMgr::GetInst()->Load<CTexture>(path_dest, path_dest);

				switch (k)
				{
				case 0: m_vecContainer[i].vecMtrl[j].strDiff = path_dest; break;
				case 1: m_vecContainer[i].vecMtrl[j].strNormal = path_dest; break;
				case 2: m_vecContainer[i].vecMtrl[j].strSpec = path_dest; break;
				}
			}		
		}
		path_origin = path_origin.parent_path();
		//remove_all(path_origin);
	}
}

void CFBXLoader::CreateMaterial()
{
	wstring strKey;
	wstring strPath;

	for (UINT i = 0; i < m_vecContainer.size(); ++i)
	{
		for (UINT j = 0; j < m_vecContainer[i].vecMtrl.size(); ++j)
		{			
			CMaterial* pMaterial = new CMaterial;

			// Material 이름짓기
			strKey = m_vecContainer[i].vecMtrl[j].strMtrlName + std::to_wstring(i);
			if (strKey.empty())
				strKey = path(m_vecContainer[i].vecMtrl[j].strDiff).stem();

			strPath = L"material\\";
			strPath += strKey;		

			// 상대경로가 곧 이름(확장자명은 제외)
			m_vecContainer[i].vecMtrl[j].strMtrlName = strKey;
			pMaterial->SetKey(strKey);
			pMaterial->SetRelativePath(strPath + L".mtrl");

			pMaterial->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3D_DeferredShader"));
						
			wstring strTexKey = m_vecContainer[i].vecMtrl[j].strDiff;
			Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(strTexKey);
			if(NULL != pTex)
				pMaterial->SetData(SHADER_PARAM::TEX_0, pTex.Get());
			
			strTexKey = m_vecContainer[i].vecMtrl[j].strNormal;
			pTex = CResMgr::GetInst()->FindRes<CTexture>(strTexKey);
			if (NULL != pTex)
				pMaterial->SetData(SHADER_PARAM::TEX_1, pTex.Get());
			
			strTexKey = m_vecContainer[i].vecMtrl[j].strSpec;
			pTex = CResMgr::GetInst()->FindRes<CTexture>(strTexKey);
			if (NULL != pTex)
				pMaterial->SetData(SHADER_PARAM::TEX_2, pTex.Get());
			
			pMaterial->SetMaterialCoefficient(
				m_vecContainer[i].vecMtrl[j].tMtrl.vDiff
				, m_vecContainer[i].vecMtrl[j].tMtrl.vSpec
				, m_vecContainer[i].vecMtrl[j].tMtrl.vAmb
				, m_vecContainer[i].vecMtrl[j].tMtrl.vEmis);

			CResMgr::GetInst()->AddRes<CMaterial>(pMaterial->GetKey(), pMaterial);
		}
	}
}

void CFBXLoader::LoadSkeleton(FbxNode * _pNode)
{
	int iChildCount = _pNode->GetChildCount();
	int iAnimCount = m_arrAnimName.GetCount();
	m_vecBone.resize(iAnimCount);
	for (int z = 0; z < iAnimCount; ++z)
	{
		
		FbxAnimStack* pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_arrAnimName[z]->Buffer());
		m_pScene->SetCurrentAnimationStack(pAnimStack);
		LoadSkeleton_Re(_pNode, 0, 0, -1,z);
	}
}

void CFBXLoader::LoadSkeleton_Re(FbxNode* _pNode, int _iDepth, int _iIdx, int _iParentIdx, int _z)
{
	FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

		if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			tBone* pBone = new tBone;

			string strBoneName = _pNode->GetName();

			pBone->strBoneName = wstring(strBoneName.begin(), strBoneName.end());
			pBone->iDepth = _iDepth++;
			pBone->iParentIndx = _iParentIdx;

			m_vecBone[(size_t)_z].push_back(pBone);
		}
	int iChildCount = _pNode->GetChildCount();
	for (int i = 0; i < iChildCount; ++i)
	{
		LoadSkeleton_Re(_pNode->GetChild(i), _iDepth, (int)m_vecBone.size(), _iIdx, _z);
	}
}

void CFBXLoader::LoadAnimationClip()
{
	int iAnimCount = m_arrAnimName.GetCount();

	for (int i = 0; i < iAnimCount; ++i)
	{
		
		FbxAnimStack* pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_arrAnimName[i]->Buffer());
		
		if (!pAnimStack)
			continue;

		tAnimClip* pAnimClip = new tAnimClip;

		string strClipName = pAnimStack->GetName();
		pAnimClip->strName = wstring(strClipName.begin(), strClipName.end());

		FbxTakeInfo* pTakeInfo = m_pScene->GetTakeInfo(pAnimStack->GetName());
		pAnimClip->tStartTime = pTakeInfo->mLocalTimeSpan.GetStart();
		pAnimClip->tEndTime = pTakeInfo->mLocalTimeSpan.GetStop();

		pAnimClip->eMode = m_pScene->GetGlobalSettings().GetTimeMode();
		pAnimClip->llTimeLength = pAnimClip->tEndTime.GetFrameCount(pAnimClip->eMode) - pAnimClip->tStartTime.GetFrameCount(pAnimClip->eMode);

		m_vecAnimClip.push_back(pAnimClip);
	}
}

void CFBXLoader::Triangulate(FbxNode * _pNode)
{
	FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

	if (pAttr &&
		(pAttr->GetAttributeType() == FbxNodeAttribute::eMesh
			|| pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs
			|| pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
	{
		FbxGeometryConverter converter(m_pManager);
		converter.Triangulate(pAttr, true);
	}

	int iChildCount = _pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		Triangulate(_pNode->GetChild(i));
	}
}

void CFBXLoader::LoadAnimationData(FbxMesh * _pMesh, tContainer * _pContainer)
{
	// Animation Data 로드할 필요가 없음
	int iSkinCount = _pMesh->GetDeformerCount(FbxDeformer::eSkin);
	
	if (iSkinCount <= 0 || m_vecAnimClip.empty())
		return;
	
	_pContainer->bAnimation = true;

	// Skin 개수만큼 반복을하며 읽는다.	
	for (int i = 0; i < iSkinCount; ++i)
	{
		FbxSkin* pSkin = (FbxSkin*)_pMesh->GetDeformer(i, FbxDeformer::eSkin);

		if (pSkin)
		{
			FbxSkin::EType eType = pSkin->GetSkinningType();
			if (FbxSkin::eRigid == eType || FbxSkin::eLinear)
			{
				// Cluster 를 얻어온다
				// Cluster == Joint == 관절
				int iClusterCount = pSkin->GetClusterCount();
				
				for (int j = 0; j < iClusterCount; ++j)
				{
					FbxCluster* pCluster = pSkin->GetCluster(j);

					if (!pCluster->GetLink())
						continue;

					// 현재 본 인덱스를 얻어온다.
					int iBoneIdx = FindBoneIndex(pCluster->GetLink()->GetName());
					if (-1 == iBoneIdx)
						assert(NULL);
					
					FbxAMatrix matNodeTransform = GetTransform(_pMesh->GetNode());

					// Weights And Indices 정보를 읽는다.
					LoadWeightsAndIndices(pCluster, iBoneIdx, _pContainer);
					int iAnimCount = m_arrAnimName.GetCount();

					for (int ii = 0; ii < iAnimCount; ++ii)
					{

						FbxAnimStack* pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_arrAnimName[ii]->Buffer());
						m_pScene->SetCurrentAnimationStack(pAnimStack);
						// Bone 의 OffSet 행렬 구한다.

						LoadOffsetMatrix(pCluster, matNodeTransform, iBoneIdx, _pContainer,ii);

						// Bone KeyFrame 별 행렬을 구한다.
						LoadKeyframeTransform(_pMesh->GetNode(), pCluster, matNodeTransform, iBoneIdx, _pContainer,ii);
					}
					
				}
			}
		}
	}
	CheckWeightAndIndices(_pMesh, _pContainer);
}


void CFBXLoader::CheckWeightAndIndices(FbxMesh* _pMesh, tContainer * _pContainer)
{
	vector<vector<tWeightsAndIndices>>::iterator iter = _pContainer->vecWI.begin();

	int iVtxIdx = 0;
	for (iter; iter != _pContainer->vecWI.end(); ++iter, ++iVtxIdx)
	{
		if ((*iter).size() > 1)
		{
			// 가중치 값 순으로 내림차순 정렬
			sort((*iter).begin(), (*iter).end()
				, [](const tWeightsAndIndices& left, const tWeightsAndIndices& right)
			{
				return left.dWeight > right.dWeight;
			}
			);

			double dWeight = 0.f;
			for (UINT i = 0; i < (*iter).size(); ++i)
			{
				dWeight += (*iter)[i].dWeight;
			}

			// 가중치의 합이 1이 넘어가면 처음부분에 더해준다.
			double revision = 0.f;
			if (dWeight > 1.0)
			{
				revision = 1.0 - dWeight;
				(*iter)[0].dWeight += revision;
			}

			if ((*iter).size() >= 4)
			{
				(*iter).erase((*iter).begin() + 4, (*iter).end());
			}
		}

		// 정점 정보로 변환, 
		float fWeights[4] = {};
		float fIndices[4] = {};

		for (UINT i = 0; i < (*iter).size(); ++i)
		{
			fWeights[i] = (float)(*iter)[i].dWeight;
			fIndices[i] = (float)(*iter)[i].iBoneIdx;
		}

		memcpy(&_pContainer->vecWeights[iVtxIdx], fWeights, sizeof(Vec4));
		memcpy(&_pContainer->vecIndices[iVtxIdx], fIndices, sizeof(Vec4));
	}
}

void CFBXLoader::LoadKeyframeTransform(FbxNode * _pNode, FbxCluster * _pCluster
	, const FbxAMatrix & _matNodeTransform, int _iBoneIdx, tContainer * _pContainer, int _i)
{
	if (m_vecAnimClip.empty())
		return;

	FbxVector4	v1 = { 1, 0, 0, 0 };
	FbxVector4	v2 = { 0, 0, 1, 0 };
	FbxVector4	v3 = { 0, 1, 0, 0 };
	FbxVector4	v4 = { 0, 0, 0, 1 };
	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	m_vecBone[(size_t)_i][_iBoneIdx]->matBone = _matNodeTransform;

	FbxTime::EMode eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();

	FbxLongLong llStartFrame = m_vecAnimClip[_i]->tStartTime.GetFrameCount(eTimeMode);
	FbxLongLong llEndFrame = m_vecAnimClip[_i]->tEndTime.GetFrameCount(eTimeMode);
	CameraClip CameraData;
	if (m_CameraAnim)
	{
		string Cname = _pCluster->GetLink()->GetName();
		
		CameraData.ClipName = m_vecAnimClip[_i]->strName;;
		if (Cname == "bn_Root")
		{
			CameraData.m_bnCamera = true;
			CameraData.bnC_Translation.resize(llEndFrame);
			CameraData.bnC_Scali.resize(llEndFrame);
			CameraData.bnC_Rotation.resize(llEndFrame);
		}
		else if (Cname == "bn_Hips")
		{
			CameraData.m_bnCamera = false;
			CameraData.bnC_Translation.resize(llEndFrame);
			CameraData.bnC_Scali.resize(llEndFrame);
			CameraData.bnC_Rotation.resize(llEndFrame);
		}
		
	}


	for (FbxLongLong i = llStartFrame; i < llEndFrame; ++i)
	{
		tKeyFrame tFrame = {};
		FbxTime   tTime = 0;

		tTime.SetFrame(i, eTimeMode);
		
		FbxAMatrix matFromNode = _pNode->EvaluateGlobalTransform(tTime) * _matNodeTransform;
		FbxAMatrix matCurTrans = matFromNode.Inverse() * _pCluster->GetLink()->EvaluateGlobalTransform(tTime);

		tFrame.dTime = tTime.GetSecondDouble();
		tFrame.matTransform = matCurTrans;

		
		if(m_CameraAnim )
		{
			string Cname = _pCluster->GetLink()->GetName();
			if (Cname == "bn_Root" || Cname == "bn_Hips") {
				FbxNode* m_Node = _pCluster->GetLink();
				Vec3 NTranslation;
				Vec3 NScali;
				Vec3 NRotation;
				FbxVector4& t1 = _pCluster->GetLink()->EvaluateLocalTranslation(tTime);
				FbxVector4& s1 = _pCluster->GetLink()->EvaluateLocalScaling(tTime);
				FbxVector4& r1 = _pCluster->GetLink()->EvaluateLocalRotation(tTime);

				NTranslation = Vec3(-(t1.mData[0]), (t1.mData[1] ), (t1.mData[2]));
				NScali = Vec3(s1.mData[0], s1.mData[1], s1.mData[2]);
				float ro1;
				float ro2;
				float ro3;

				if (r1.mData[0] > 0)
					ro1 = r1.mData[0];
				else
					ro1 = 360.f - r1.mData[0];

				if (r1.mData[1] > 0)
					ro2 = -r1.mData[1];
				else
					ro2 = -(360.f + r1.mData[1]);

				if (r1.mData[2] > 0)
					ro3 = -r1.mData[2];
				else
					ro3 = -(360.f + r1.mData[2]);
				NRotation = Vec3(ro1 / 180.f * XM_PI, ro2 / 180.f * XM_PI, ro3 / 180.f * XM_PI);
				CameraData.bnC_Translation[i] = NTranslation;
				CameraData.bnC_Scali[i] = NScali;
				CameraData.bnC_Rotation[i] = NRotation;
			}
		}
		//matCurTrans = matReflect * matCurTrans * matReflect;
		m_vecBone[(size_t)_i][_iBoneIdx]->vecKeyFrame.push_back(tFrame);
	}
	if (m_CameraAnim)
	{
		string Cname = _pCluster->GetLink()->GetName();
		if (Cname == "bn_Root" || Cname == "bn_Hips")
			m_CameraData.push_back(CameraData);
	}

}

void CFBXLoader::LoadOffsetMatrix(FbxCluster * _pCluster
	, const FbxAMatrix & _matNodeTransform
	, int _iBoneIdx, tContainer* _pContainer,int _i)
{
	FbxAMatrix matClusterTrans;
	FbxAMatrix matClusterLinkTrans;

	_pCluster->GetTransformMatrix(matClusterTrans);
	_pCluster->GetTransformLinkMatrix(matClusterLinkTrans);

	// Reflect Matrix
	FbxVector4 V0 = { 1, 0, 0, 0 };
	FbxVector4 V1 = { 0, 0, 1, 0 };
	FbxVector4 V2 = { 0, 1, 0, 0 };
	FbxVector4 V3 = { 0, 0, 0, 1 };

	FbxAMatrix matReflect;
	matReflect[0] = V0;
	matReflect[1] = V1;
	matReflect[2] = V2;
	matReflect[3] = V3;

	FbxAMatrix matOffset;
	matOffset = matClusterLinkTrans.Inverse() * matClusterTrans *_matNodeTransform;
	//matOffset = matReflect * matOffset * matReflect;

	m_vecBone[(size_t)_i][_iBoneIdx]->matOffset = matOffset;
}


void CFBXLoader::LoadWeightsAndIndices(FbxCluster * _pCluster
	, int _iBoneIdx
	, tContainer * _pContainer)
{
	int iIndicesCount = _pCluster->GetControlPointIndicesCount();

	for (int i = 0; i < iIndicesCount; ++i)
	{
		tWeightsAndIndices tWI = {};

		// 각 정점에게 본 인덱스 정보와, 가중치 값을 알린다.
		tWI.iBoneIdx = _iBoneIdx;
		tWI.dWeight = _pCluster->GetControlPointWeights()[i];

		int iVtxIdx = _pCluster->GetControlPointIndices()[i];

		_pContainer->vecWI[iVtxIdx].push_back(tWI);
	}
}



int CFBXLoader::FindBoneIndex(string _strBoneName)
{
	wstring strBoneName = wstring(_strBoneName.begin(), _strBoneName.end());

	for (UINT i = 0; i < m_vecBone[0].size(); ++i)
	{
		if (m_vecBone[0][i]->strBoneName == strBoneName)
			return i;
	}
	
	return -1;
}

FbxAMatrix CFBXLoader::GetTransform(FbxNode * _pNode)
{
	const FbxVector4 vT = _pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 vR = _pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 vS = _pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(vT, vR, vS);
}

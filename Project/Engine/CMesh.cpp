#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"
#include "CPathMgr.h"

#include "CStructuredBuffer.h"
#include "CInstancingBuffer.h"
#include "CTransform.h"
CMesh::CMesh()
	: m_tVBDesc{}	
	, m_iVtxCount(0)
	, m_pVtxSysMem(nullptr)
	, m_bIsNavMesh(false)

{
}

CMesh::~CMesh()
{
	if (nullptr != m_pVtxSysMem)
		delete m_pVtxSysMem;

	for (int i = 0; i < (int)m_vecIdxInfo.size(); ++i)
	{
		if (nullptr != m_vecIdxInfo[i].pIdxSysMem)
		{			
			delete m_vecIdxInfo[i].pIdxSysMem;
		}
	}
	for (UINT i = 0; i < m_pBoneFrameData.size(); i++)
	{
		SAFE_DELETE(m_pBoneFrameData[i]);
	}
	for (UINT i = 0; i < m_pBoneFrameData.size(); i++)
	{
		SAFE_DELETE(m_pBoneOffset[i]);
	}
	
}

void CMesh::Create(VTX* _pVTXSysmem, UINT _iVtxCount, UINT* _IdxSysmem, UINT _iIdxCount)
{
	m_iVtxCount = _iVtxCount;	
	
	// GPU 메모리에 Vertex Buffer 만들기
	m_tVBDesc.ByteWidth = sizeof(VTX) * m_iVtxCount;	// GPU 에 생성할 버퍼 메모리 크기
	m_tVBDesc.Usage = D3D11_USAGE_DEFAULT;				// 수정 불가능
	m_tVBDesc.CPUAccessFlags = 0;						// 수정 불가능
	m_tVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		// Vertex Buffer 용도

	D3D11_SUBRESOURCE_DATA tSubDesc = {};
	tSubDesc.pSysMem = _pVTXSysmem;							// 초기 데이터

	HRESULT hr = DEVICE->CreateBuffer(&m_tVBDesc, &tSubDesc, m_pVB.GetAddressOf());

	// GPU 메모리에 Index Buffer 만들기
	tIndexInfo info = {};
	info.iIdxCount = _iIdxCount;
	info.tIBDesc.ByteWidth = sizeof(UINT) * info.iIdxCount;	// GPU 에 생성할 버퍼 메모리 크기
	info.tIBDesc.Usage = D3D11_USAGE_DEFAULT;				// 수정 불가능 버퍼
	info.tIBDesc.CPUAccessFlags = 0;						// 수정 불가능 버퍼
	info.tIBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;		// Index Buffer 용도

	tSubDesc = {};
	tSubDesc.pSysMem = _IdxSysmem;						// 초기 데이터

	hr = DEVICE->CreateBuffer(&info.tIBDesc, &tSubDesc, info.pIB.GetAddressOf());


	m_vecIdxInfo.push_back(info);
}

void CMesh::Reset(VTX* _pVTXSysmem, UINT _iVtxCount, UINT* _IdxSysmem, UINT _iIdxCount)
{
	m_pVB = nullptr;
	m_tVBDesc = {};
	m_iVtxCount = 0;

	m_vecIdxInfo.clear();

	Create(_pVTXSysmem, _iVtxCount, _IdxSysmem, _iIdxCount);
}


CMesh* CMesh::CreateFromContainer(CFBXLoader& _loader,int _num,bool _Nav)
{

	CMesh* pMesh = new CMesh;

	const tContainer* container = &_loader.GetContainer(_num);

	UINT iVtxCount = (UINT)container->vecPos.size();

	D3D11_BUFFER_DESC tVtxDesc = {};
	tVtxDesc.ByteWidth = sizeof(VTX) * iVtxCount;
	tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
	if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
		tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = malloc(tVtxDesc.ByteWidth);
	VTX* pSys = (VTX*)tSub.pSysMem;
	
	// NavMesh
	/*
	if (_Nav)
	{
		//vector<NavMesh> vecNavData;
		UINT _Iidx = 0; // 삼각형 인덱스
		NavMesh navmeshdd;
		//  D3D11_SUBRESOURCE_DATA tSub = {};
		//   tSub.pSysMem = malloc(tVtxDesc.ByteWidth);
		  // VTX* pSys = (VTX*)tSub.pSysMem;
		pMesh->m_bIsNavMesh = true;
		for (UINT i = 0; i < iVtxCount; ++i)
		{
			pSys[i].vPos = container->vecPos[i];
			pSys[i].vUV = container->vecUV[i];
			pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
			pSys[i].vNormal = container->vecNormal[i];
			pSys[i].vTangent = container->vecTangent[i];
			pSys[i].vBinormal = container->vecBinormal[i];
			pSys[i].vWeights = container->vecWeights[i];
			pSys[i].vIndices = container->vecIndices[i];

			int _idx = i % 3;
			navmeshdd.fPointPos[_idx] = pSys[i].vPos;
			//vecNavData[_Iidx].fPointPos[_idx] = pSys[i].vPos;

			if (i >= 0 && _idx == 0) pMesh->m_vecNavData.push_back(navmeshdd);
		}
		//vecNavData.resize(vecNavData.size());

		int _iMeshCount = pMesh->m_vecNavData.size();
		for (UINT i = 0; i < _iMeshCount; i++)
		{
			for (UINT j = 0; j < _iMeshCount; j++)
			{
				if (i == j) continue;

				for (int c = 0; c < 3; c++)
				{
					bool _EndFlag = false;
					for (int d = 0; d < 3; d++)
					{
						if (pMesh->m_vecNavData[j].fPointPos[d] == pMesh->m_vecNavData[i].fPointPos[c])
						{
							pMesh->m_vecNavData[i].vecAdjIdx.push_back(j);
							_EndFlag = true;
							break;
						}
					}
					if (_EndFlag) break;
				}
			}
		}
		int aadas = 0;
	}
	*/
	
	for (UINT i = 0; i < iVtxCount; ++i)
	{
		pSys[i].vPos = container->vecPos[i];
		pSys[i].vUV = container->vecUV[i];
		pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
		pSys[i].vNormal = container->vecNormal[i];
		pSys[i].vTangent = container->vecTangent[i];
		pSys[i].vBinormal = container->vecBinormal[i];
		pSys[i].vWeights = container->vecWeights[i];
		pSys[i].vIndices = container->vecIndices[i];
	}

	if (_Nav)
	{
		NavMesh navmeshdd;
		pMesh->m_bIsNavMesh = true;
		for (UINT i = 0; i<(UINT)container->vecIdx[0].size(); i++) //인덱스 버퍼
		{
			int _idx = i % 3; //3개 단위로 삼각형의 정점을 읽어옴
			if (i > 0 && _idx == 0) pMesh->m_vecNavData.push_back(navmeshdd); //i==0일때는 무시
		
			navmeshdd.fPointPos[_idx] = pSys[container->vecIdx[0][i]].vPos; //정점의 pos 저장
			//vecNavData[_Iidx].fPointPos[_idx] = pSys[i].vPos;
		}
		int _iMeshCount = pMesh->m_vecNavData.size();
		for (UINT i = 0; i < _iMeshCount; i++)
		{
			for (UINT j = 0; j < _iMeshCount; j++)
			{
				if (i == j) continue; //첫번째거는 어짜피 같은 정점이므로 무시

				for (int c = 0; c < 3; c++) //0, (1,2,3,4 ...) 순서로 삼각형 하나(정점3개)와 다음 삼각형을 (정점3개)			
				{
					bool _EndFlag = false;
					for (int d = 0; d < 3; d++)
					{
						if (pMesh->m_vecNavData[j].fPointPos[d] == pMesh->m_vecNavData[i].fPointPos[c]) // pos를 비교하여
						{
							pMesh->m_vecNavData[i].vecAdjIdx.push_back(j); 		//같으면 인접 삼각형 벡터에 삼각형의 인덱스를 저장한다.
							_EndFlag = true; //인접 삼각형이면 남은 정점 검사를 더이상 안해도 되므로 끝낸다
							break;
						}
					}
					if (_EndFlag) break;
				}
			}
		}
		
	}
	
	ComPtr<ID3D11Buffer> pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
	{
		return NULL;
	}


	pMesh->m_pVB = pVB;
	pMesh->m_iVtxCount = iVtxCount;
	pMesh->m_tVBDesc = tVtxDesc;
	pMesh->m_pVtxSysMem = pSys;


	// 인덱스 정보
	UINT iIdxBufferCount = (UINT)container->vecIdx.size();
	D3D11_BUFFER_DESC tIdxDesc = {};

	for (UINT i = 0; i < iIdxBufferCount; ++i)
	{
		tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format 이 R32_UINT 이기 때문
		tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
		if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
			tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				
		void* pSysMem = malloc(tIdxDesc.ByteWidth);
		memcpy(pSysMem, container->vecIdx[i].data(), tIdxDesc.ByteWidth);
		tSub.pSysMem = pSysMem;

		ComPtr<ID3D11Buffer> pIB = nullptr;
		if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
		{
			return NULL;
		}
		(UINT)container->vecIdx[i][0];
		tIndexInfo info = {};
		info.tIBDesc = tIdxDesc;
		info.iIdxCount = (UINT)container->vecIdx[i].size();
		info.pIdxSysMem = pSysMem;
		info.pIB = pIB;

		pMesh->m_vecIdxInfo.push_back(info);
	}

	// Animation3D
	if (!container->bAnimation)
		return pMesh;
	UINT ii = _loader.GetBonesSize();
	pMesh->m_vecBones.resize(ii);
	for (UINT zz = 0; zz < ii; ++zz)
	{
		vector<tBone*>& vecBone = _loader.GetBones(zz);
		
		pMesh->i_FrameCount.push_back(0);
		for (UINT i = 0; i < vecBone.size(); ++i)
		{
			tMTBone bone = {};
			bone.iDepth = vecBone[i]->iDepth;
			bone.iParentIndx = vecBone[i]->iParentIndx;
			bone.matBone = GetMatrixFromFbxMatrix(vecBone[i]->matBone);
			bone.matOffset = GetMatrixFromFbxMatrix(vecBone[i]->matOffset);
			bone.strBoneName = vecBone[i]->strBoneName;

			for (UINT j = 0; j < vecBone[i]->vecKeyFrame.size(); ++j)
			{
				tMTKeyFrame tKeyframe = {};
				tKeyframe.dTime = vecBone[i]->vecKeyFrame[j].dTime;
				tKeyframe.iFrame = j;
				tKeyframe.vTranslate.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0];
				tKeyframe.vTranslate.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1];
				tKeyframe.vTranslate.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2];

				tKeyframe.vScale.x =     (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0];
				tKeyframe.vScale.y =     (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1];
				tKeyframe.vScale.z =     (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2];

				tKeyframe.qRot.x =       (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0];
				tKeyframe.qRot.y =       (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1];
				tKeyframe.qRot.z =       (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2];
				tKeyframe.qRot.w =       (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3];

				bone.vecKeyFrame.push_back(tKeyframe);
			}

			pMesh->i_FrameCount[zz]=(max(pMesh->i_FrameCount[zz], (UINT)bone.vecKeyFrame.size()));

			pMesh->m_vecBones[zz].push_back(bone);
		}
	}
	vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();

	for (UINT i = 0; i < vecAnimClip.size(); ++i)
	{
		tMTAnimClip tClip = {};

		tClip.strAnimName = vecAnimClip[i]->strName;
		tClip.dStartTime = vecAnimClip[i]->tStartTime.GetSecondDouble();
		tClip.dEndTime = vecAnimClip[i]->tEndTime.GetSecondDouble();
		tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

		tClip.iStartFrame = (int)vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode);
		tClip.iEndFrame = (int)vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode);
		tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
		tClip.eMode = vecAnimClip[i]->eMode;

		pMesh->m_vecAnimClip.push_back(tClip);
	}
	pMesh->m_pBoneOffset.resize(ii);
	pMesh->m_pBoneFrameData.resize(ii);
	for (UINT zz = 0; zz < ii; ++zz)
	{
		// Animation 이 있는 Mesh 경우 BoneTexture 만들어두기
		if (pMesh->IsAnimMesh())
		{
			// BoneOffet 행렬
			vector<Matrix> vecOffset;
			vector<tFrameTrans> vecFrameTrans;
			vecFrameTrans.resize((UINT)pMesh->m_vecBones[zz].size() * pMesh->i_FrameCount[zz]);

			for (size_t i = 0; i < pMesh->m_vecBones[zz].size(); ++i)
			{
				vecOffset.push_back(pMesh->m_vecBones[zz][i].matOffset);

				for (size_t j = 0; j < pMesh->m_vecBones[zz][i].vecKeyFrame.size(); ++j)
				{
					vecFrameTrans[(UINT)pMesh->m_vecBones[zz].size() * j + i]
						= tFrameTrans{ Vec4(pMesh->m_vecBones[zz][i].vecKeyFrame[j].vTranslate)
						, Vec4(pMesh->m_vecBones[zz][i].vecKeyFrame[j].vScale)
						, pMesh->m_vecBones[zz][i].vecKeyFrame[j].qRot };
				}
			}
			pMesh->m_pBoneOffset[zz] = new CStructuredBuffer;
			pMesh->m_pBoneOffset[zz]->Create(sizeof(Matrix), (UINT)vecOffset.size(), vecOffset.data());

			pMesh->m_pBoneFrameData[zz] = new CStructuredBuffer;
			pMesh->m_pBoneFrameData[zz]->Create(sizeof(tFrameTrans)
				, (UINT)vecOffset.size() * pMesh->i_FrameCount[zz]
				, vecFrameTrans.data());
		}
	}
	return pMesh;
	
}

void CMesh::UpdateData(UINT _iSubset)
{	
	UINT iStride = sizeof(VTX);
	UINT iOffset = 0;

	CONTEXT->IASetVertexBuffers(0, 1, m_pVB.GetAddressOf(), &iStride, &iOffset);
	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::render(UINT _iSubset)
{
	CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubset].iIdxCount, 0, 0);
}

void CMesh::render_particle(UINT _iParticleCount)
{
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, _iParticleCount, 0, 0, 0);
}

void CMesh::render_instancing(UINT _iSubset)
{
	if (_iSubset >= m_vecIdxInfo.size())
		assert(nullptr);

	ID3D11Buffer* arrBuffer[2] = { m_pVB.Get(), CInstancingBuffer::GetInst()->GetBuffer().Get() };
	UINT		  iStride[2] = { sizeof(VTX), sizeof(tInstancingData) };
	UINT		  iOffset[2] = { 0, 0 };

	CONTEXT->IASetVertexBuffers(0, 2, arrBuffer, iStride, iOffset);

	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[_iSubset].iIdxCount
		, CInstancingBuffer::GetInst()->GetInstanceCount(), 0, 0, 0);
}

void CMesh::NavMeshDataTrans(Matrix _TransData)
{
	
	for (UINT i = 0; i < m_vecNavData.size(); i++)
	{
		for (UINT l = 0; l < 3; l++)
			m_vecNavData[i].fPointPos[l] = XMVector3TransformCoord(m_vecNavData[i].fPointPos[l], _TransData);

	}
	//vertices[0] = XMVector3TransformCoord(_vertices1, _TransData);
	//vertices[1] = XMVector3TransformCoord(_vertices2, _TransData);
	//vertices[2] = XMVector3TransformCoord(_vertices3, _TransData);
}

void CMesh::Save(const wstring& _strRelativePath)
{
	wstring strFileName = CPathMgr::GetResPath();
	strFileName += _strRelativePath;
	SetRelativePath(_strRelativePath);

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"wb");

	assert(pFile);

	// 키값
	SaveWString(GetName(), pFile);

	// 상대 경로
	SaveWString(GetRelativePath(), pFile);

	// 정점 데이터 저장			
	fwrite(&m_iVtxCount, sizeof(int), 1, pFile);

	int iByteSize = sizeof(VTX) * m_iVtxCount;
	fwrite(&iByteSize, sizeof(int), 1, pFile);
	fwrite(m_pVtxSysMem, iByteSize, 1, pFile);

	// 인덱스 정보
	UINT iMtrlCount = (UINT)m_vecIdxInfo.size();
	fwrite(&iMtrlCount, sizeof(int), 1, pFile);

	UINT iIdxBuffSize = 0;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		fwrite(&m_vecIdxInfo[i], sizeof(tIndexInfo), 1, pFile);
		fwrite(m_vecIdxInfo[i].pIdxSysMem
			, m_vecIdxInfo[i].iIdxCount * GetSizeofFormat(DXGI_FORMAT_R32_UINT)
			, 1, pFile);
	}

	// Animation3D 정보 
	UINT iCount = (UINT)m_vecAnimClip.size();
	fwrite(&iCount, sizeof(int), 1, pFile);
	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWString(m_vecAnimClip[i].strAnimName, pFile);
		fwrite(&m_vecAnimClip[i].dStartTime, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].dEndTime, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].dTimeLength, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].eMode, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].fUpdateTime, sizeof(float), 1, pFile);
		fwrite(&m_vecAnimClip[i].iStartFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].iEndFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].iFrameLength, sizeof(int), 1, pFile);
	}

	iCount = (UINT)m_vecBones.size();
	fwrite(&iCount, sizeof(int), 1, pFile);
	for (UINT i = 0; i < iCount; ++i)
	{
		UINT iCount2 = m_vecBones[i].size();
		fwrite(&iCount2, sizeof(int), 1, pFile);
		for (UINT l = 0; l < iCount2; l++)
		{
			SaveWString(m_vecBones[i][l].strBoneName, pFile);
			fwrite(&m_vecBones[i][l].iParentIndx, sizeof(int), 1, pFile);
			fwrite(&m_vecBones[i][l].matBone, sizeof(Matrix), 1, pFile);
			fwrite(&m_vecBones[i][l].iDepth, sizeof(int), 1, pFile);
			fwrite(&m_vecBones[i][l].matOffset, sizeof(Matrix), 1, pFile);

			int iFrameCount = (int)m_vecBones[i][l].vecKeyFrame.size();
			fwrite(&iFrameCount, sizeof(int), 1, pFile);

			for (int j = 0; j < m_vecBones[i][l].vecKeyFrame.size(); ++j)
			{
				fwrite(&m_vecBones[i][l].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
			}
		}
	}


	fclose(pFile);
}

void CMesh::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// 키값, 상대경로
	wstring strKey, strRelativePath;
	LoadWString(strKey, pFile);
	SetName(strKey);
	LoadWString(strRelativePath, pFile);

	SetKey(strKey);
	SetRelativePath(strRelativePath);

	// 정점데이터	
	fread(&m_iVtxCount, sizeof(int), 1, pFile);

	UINT iByteSize = 0;
	fread(&iByteSize, sizeof(int), 1, pFile);

	m_pVtxSysMem = (VTX*)malloc(iByteSize);
	fread(m_pVtxSysMem, 1, iByteSize, pFile);


	D3D11_BUFFER_DESC tDesc = {};
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.ByteWidth = sizeof(VTX) * m_iVtxCount;
	tDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA tSubData = {};
	tSubData.pSysMem = m_pVtxSysMem;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tSubData, m_pVB.GetAddressOf())))
	{
		assert(nullptr);
	}

	// 인덱스 정보
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		tIndexInfo info = {};
		fread(&info, sizeof(tIndexInfo), 1, pFile);

		UINT iByteWidth = info.iIdxCount * GetSizeofFormat(DXGI_FORMAT_R32_UINT);

		void* pSysMem = malloc(iByteWidth);
		info.pIdxSysMem = pSysMem;
		fread(info.pIdxSysMem, iByteWidth, 1, pFile);

		tSubData.pSysMem = info.pIdxSysMem;

		if (FAILED(DEVICE->CreateBuffer(&info.tIBDesc, &tSubData, info.pIB.GetAddressOf() )))
		{
			assert(nullptr);
		}

		m_vecIdxInfo.push_back(info);
	}

	// Animation3D 정보 읽기

	int iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	for (int i = 0; i < iCount; ++i)
	{
		tMTAnimClip tClip = {};

		LoadWString(tClip.strAnimName, pFile);
		fread(&tClip.dStartTime, sizeof(double), 1, pFile);
		fread(&tClip.dEndTime, sizeof(double), 1, pFile);
		fread(&tClip.dTimeLength, sizeof(double), 1, pFile);
		fread(&tClip.eMode, sizeof(int), 1, pFile);
		fread(&tClip.fUpdateTime, sizeof(float), 1, pFile);
		fread(&tClip.iStartFrame, sizeof(int), 1, pFile);
		fread(&tClip.iEndFrame, sizeof(int), 1, pFile);
		fread(&tClip.iFrameLength, sizeof(int), 1, pFile);

		m_vecAnimClip.push_back(tClip);
	}

	iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	m_vecBones.resize(iCount);

	i_FrameCount.resize(iCount);
	for (int i = 0; i < iCount; ++i)
	{
		i_FrameCount[i] = 0;
		int iCount2 = 0;
		fread(&iCount2, sizeof(int), 1, pFile);
		m_vecBones[i].resize(iCount2);
		for (int l = 0; l < iCount2; l++)
		{
			LoadWString(m_vecBones[i][l].strBoneName, pFile);
			fread(&m_vecBones[i][l].iDepth, sizeof(int), 1, pFile);
			fread(&m_vecBones[i][l].iParentIndx, sizeof(int), 1, pFile);
			fread(&m_vecBones[i][l].matBone, sizeof(Matrix), 1, pFile);
			fread(&m_vecBones[i][l].matOffset, sizeof(Matrix), 1, pFile);

			UINT iFrameCount = 0;
			fread(&iFrameCount, sizeof(int), 1, pFile);
			m_vecBones[i][l].vecKeyFrame.resize(iFrameCount);
			i_FrameCount[i] = max(i_FrameCount[i], iFrameCount);
			for (UINT j = 0; j < iFrameCount; ++j)
			{
				fread(&m_vecBones[i][l].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
			}
		}
	}

	// Animation 이 있는 Mesh 경우 Bone StructuredBuffer 만들기
	if (m_vecAnimClip.size() > 0 && m_vecBones.size() > 0)
	{
		m_pBoneOffset.resize(m_vecBones.size());
		m_pBoneFrameData.resize(m_vecBones.size());
		for (UINT z = 0; z < m_vecBones.size(); z++)
		{
			wstring strBone = GetName();
			
			// BoneOffet 행렬
			vector<Matrix> vecOffset;
			vector<tFrameTrans> vecFrameTrans;
			vecFrameTrans.resize((UINT)m_vecBones[z].size() * i_FrameCount[z]);

			for (size_t i = 0; i < m_vecBones[z].size(); ++i)
			{
				vecOffset.push_back(m_vecBones[z][i].matOffset);

				for (size_t j = 0; j < m_vecBones[z][i].vecKeyFrame.size(); ++j)
				{
					vecFrameTrans[(UINT)m_vecBones[z].size() * j + i]
						= tFrameTrans{ Vec4(m_vecBones[z][i].vecKeyFrame[j].vTranslate)
						, Vec4(m_vecBones[z][i].vecKeyFrame[j].vScale)
						, Vec4(m_vecBones[z][i].vecKeyFrame[j].qRot) };
				}
			}

			m_pBoneOffset[z] = new CStructuredBuffer;
			m_pBoneOffset[z]->Create(sizeof(Matrix), (UINT)vecOffset.size(), vecOffset.data());

			m_pBoneFrameData[z] = new CStructuredBuffer;
			m_pBoneFrameData[z]->Create(sizeof(tFrameTrans)
				, (UINT)vecOffset.size() * (UINT)i_FrameCount[z]
				, vecFrameTrans.data());
		}
	}

	fclose(pFile);
}
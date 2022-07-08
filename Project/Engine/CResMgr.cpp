#include "pch.h"
#include "CResMgr.h"

#include "CTexture.h"

CResMgr::CResMgr()
	: m_bEvent(false)
{
	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		m_vecCloneRes[i].reserve(1000);
	}
}

CResMgr::~CResMgr()
{
	Safe_Delete_Map(m_mapDataTex);

	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		Safe_Delete_Vector(m_vecCloneRes[i]);
	}	

	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		Safe_Delete_Map(m_mapRes[i]);
	}
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, UINT _iWidth, UINT _iHeight, UINT _eFlag, DXGI_FORMAT _eFormat, bool _bData)
{
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);

	assert(nullptr == pTex);
	
	pTex = new CTexture;
	if (FAILED(pTex->Create(_iWidth, _iHeight, _eFlag, _eFormat)))
	{
		MessageBox(nullptr, L"텍스쳐 생성 실패", L"오류", MB_OK);
		return nullptr;
	}

	pTex->SetKey(_strKey);

	if (_bData)
	{
		m_mapDataTex.insert(make_pair(_strKey, pTex.Get()));
	}
	else
	{
		m_mapRes[(UINT)RES_TYPE::TEXTURE].insert(make_pair(_strKey, pTex.Get()));
		m_bEvent = true;
	}
	return pTex;
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _pTex2D, bool _bData)
{
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);

	assert(nullptr == pTex);

	pTex = new CTexture;
	if (FAILED(pTex->Create(_pTex2D)))
	{
		MessageBox(nullptr, L"텍스쳐 생성 실패", L"오류", MB_OK);
		return nullptr;
	}

	pTex->SetKey(_strKey);

	if (_bData)
	{
		m_mapDataTex.insert(make_pair(_strKey, pTex.Get()));
	}
	else
	{
		m_mapRes[(UINT)RES_TYPE::TEXTURE].insert(make_pair(_strKey, pTex.Get()));
		m_bEvent = true;
	}

	
	return pTex;
}


Ptr<CTexture> CResMgr::FindDataTexture(const wstring& _strKey)
{
	map<wstring, CTexture*>::iterator iter = m_mapDataTex.find(_strKey);

	if (iter == m_mapDataTex.end())
		return nullptr;

	return iter->second;	
}

Ptr<CMeshData> CResMgr::LoadFBX(const wstring& _strPath, LOADFROMFBXTYPE _Type)
{
	wstring strFileName = path(_strPath).stem();

	wstring strName = L"meshdata\\";
	strName += strFileName + L".mdat";

	Ptr<CMeshData> pMeshData = FindRes<CMeshData>(strName);

	if (nullptr != pMeshData)
		return pMeshData;

	pMeshData = CMeshData::LoadFromFBX(_strPath, _Type);
	pMeshData->SetKey(strName);
	pMeshData->SetRelativePath(strName);
	m_mapRes[(UINT)RES_TYPE::MESHDATA].insert(make_pair(strName, pMeshData.Get()));

	return pMeshData;
}

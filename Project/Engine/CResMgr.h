#pragma once
#include "CSingleton.h"

#include "CPathMgr.h"

#include "Ptr.h"
#include "CMesh.h"
#include "CGraphicsShader.h"
#include "CComputeShader.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "CPrefab.h"
#include "CSound.h"
#include "CMeshData.h"

class CResMgr
	: public CSingleton<CResMgr>
{
	SINGLE(CResMgr);
private:
	map<wstring, CRes*>		m_mapRes[(UINT)RES_TYPE::END];
	vector<CRes*>			m_vecCloneRes[(UINT)RES_TYPE::END];

	map<wstring, CTexture*> m_mapDataTex;

	bool					m_bEvent; // Resource 가 새롭게 추가 되거나 삭제되는 경우를 감지하는 용도

public:
	void init();
	void ClearCloneRes();
	bool HasEvnOcrd() 
	{ 
		bool bReturn = m_bEvent;
		m_bEvent = false;
		return bReturn;
	}

public:
	Ptr<CTexture> CreateTexture(const wstring& _strKey, UINT _iWidth, UINT _iHeight, UINT _eBindFlag, DXGI_FORMAT _eFormat, bool _bData = false);
	Ptr<CTexture> CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _pTex2D, bool _bData = false);

	template<typename T>
	Ptr<T> Load(const wstring& _strKey, const wstring& _strRelativePath);

	template<typename T>
	void AddRes(const wstring& _strKey, Ptr<T> _pRes);

	template<typename T>
	void AddCloneRes(Ptr<T> _pCloneRes);

	template<typename T>
	Ptr<T> FindRes(const wstring& _strKey);
	const map<wstring, CRes*>& GetRes(RES_TYPE _eType){return m_mapRes[(UINT)_eType];}

	Ptr<CTexture> FindDataTexture(const wstring& _strKey);

	Ptr<CMeshData> LoadFBX(const wstring& _strPath, LOADFROMFBXTYPE _Type= LOADFROMFBXTYPE::NONE);

private:
	void CreateDefaultMesh();
	void CreateDefaultShader();
	void CreateComputeShader();
	void CreateDefaultMaterial();	
	void InitSound();
};

template<typename T>
RES_TYPE GetResType()
{
	const type_info& info = typeid(T);

	if (typeid(CMesh).hash_code() == info.hash_code())
		return RES_TYPE::MESH;
	else if (typeid(CGraphicsShader).hash_code() == info.hash_code()
		|| typeid(CComputeShader).hash_code() == info.hash_code())
		return RES_TYPE::SHADER;
	else if (typeid(CTexture).hash_code() == info.hash_code())
		return RES_TYPE::TEXTURE;
	else if (typeid(CMaterial).hash_code() == info.hash_code())
		return RES_TYPE::MATERIAL;
	else if (typeid(CPrefab).hash_code() == info.hash_code())
		return RES_TYPE::PREFAB;	
	else if (typeid(CSound).hash_code() == info.hash_code())
		return RES_TYPE::SOUND;
	else if (typeid(CMeshData).hash_code() == info.hash_code())
		return RES_TYPE::MESHDATA;
	else
		return RES_TYPE::END;
	
}


template<typename T>
inline Ptr<T> CResMgr::Load(const wstring& _strKey, const wstring& _strRelativePath)
{
	RES_TYPE eType = GetResType<T>();

	CRes* pRes = FindRes<T>(_strKey).Get();
	if (pRes)
	{
		return (T*)pRes;
	}
	//assert(!pRes);

	wstring strFilePath = CPathMgr::GetResPath();
	strFilePath += _strRelativePath;

	pRes = new T;
	pRes->Load(strFilePath);

	pRes->SetKey(_strKey);
	pRes->SetRelativePath(_strRelativePath);

	m_mapRes[(UINT)eType].insert(make_pair(_strKey, pRes));
	m_bEvent = true;

	return (T*)pRes;
}

template<typename T>
inline void CResMgr::AddRes(const wstring& _strKey, Ptr<T> _pRes)
{
	RES_TYPE type = GetResType<T>();

	// 중복키 검사
	// 이미 동일한 키로 리소스가 등록되어있는경우 assert
	assert( nullptr == FindRes<T>(_strKey));

	_pRes->SetKey(_strKey);
	m_mapRes[(UINT)type].insert(make_pair(_strKey, _pRes.Get()));
	m_bEvent = true;

}

template<typename T>
inline void CResMgr::AddCloneRes(Ptr<T> _pCloneRes)
{
	RES_TYPE type = GetResType<T>();
	m_vecCloneRes[(UINT)type].push_back(_pCloneRes.Get());
}

template<typename T>
inline Ptr<T> CResMgr::FindRes(const wstring& _strKey)
{
	RES_TYPE type = GetResType<T>();

	map<wstring, CRes*>::iterator iter = m_mapRes[(UINT)type].find(_strKey);

	// 키에 해당하는 리소스가 없는 경우
	if (iter == m_mapRes[(UINT)type].end())
		return nullptr;

	return (T*)iter->second;
}

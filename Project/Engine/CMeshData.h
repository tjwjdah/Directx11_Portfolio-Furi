#pragma once

#include "CFBXLoader.h"

#include "Ptr.h"
#include "CMaterial.h"
#include "CMesh.h"

class CGameObject;

struct MeshMaterial {
	Ptr<CMesh>				m_pMesh;
	vector<Ptr<CMaterial>>	m_vecMtrl;
};


class CMeshData :
	public CRes
{
private:
	vector<MeshMaterial> m_MeshData;
	vector<tMeshRink> m_vMeshRink;
public:
	static CMeshData* LoadFromFBX(const wstring& _strFilePath, LOADFROMFBXTYPE _Type = LOADFROMFBXTYPE::NONE);

	virtual void Save(const wstring& _strRelativePath);
	virtual void Load(const wstring& _strFilePath);

	CGameObject* Instantiate();

	CLONE_DISABLE(CMeshData);

public:
	CMeshData();
	virtual ~CMeshData();
};


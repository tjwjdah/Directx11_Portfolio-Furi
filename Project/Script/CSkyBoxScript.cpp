#include "pch.h"
#include "CSkyBoxScript.h"

CSkyBoxScript::CSkyBoxScript()
	: CScript((UINT)SCRIPT_TYPE::SKYBOXSCRIPT)
	, m_iBoxType (0)
{
	AddDesc(tDataDesc(SCRIPT_DATA_TYPE::MEMFUNC, "Change BoxType", this, (SCRIPT_MEMFUNC)&CSkyBoxScript::SetBoxType));
}


CSkyBoxScript::~CSkyBoxScript()
{
}

void CSkyBoxScript::awake()
{
}

void CSkyBoxScript::update()
{
}

void CSkyBoxScript::SetBoxType()
{
	Ptr<CMaterial> pMtrl = MeshRender()->GetSharedMaterial(0);

	if (1 == m_iBoxType)
	{
		// sphere -> cube		
		MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
		MeshRender()->SetMaterial(pMtrl, 0);
		m_iBoxType = 0;
	}
	else
	{
		// cube -> sphere
		MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		MeshRender()->SetMaterial(pMtrl, 0);
		m_iBoxType = 1;
	}

	MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::INT_0, &m_iBoxType);
}


void CSkyBoxScript::SaveToScene(FILE* _pFile)
{
	CScript::SaveToScene(_pFile);
}

void CSkyBoxScript::LoadFromScene(FILE* _pFile)
{
	CScript::LoadFromScene(_pFile);
}

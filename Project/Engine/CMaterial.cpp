#include "pch.h"
#include "CMaterial.h"

#include "CResMgr.h"
#include "CConstBuffer.h"
#include "CDevice.h"

#include "CSceneMgr.h"
#include "CScene.h"

CMaterial::CMaterial()
	: m_tInfo{}
	, m_pShader(nullptr)
	, m_bDefault(false)
{
	SetMaterialCoefficient(Vec4(1.f, 1.f, 1.f, 1.f)
		, Vec4(1.f, 1.f, 1.f, 1.f)
		, Vec4(1.f, 1.f, 1.f, 1.f)
		, Vec4(1.f, 1.f, 1.f, 1.f));
}

CMaterial::CMaterial(const CMaterial& _origin)
	: m_tInfo(_origin.m_tInfo)
	, m_pShader(_origin.m_pShader)
	, m_arrTex{}
{
	for (int i = 0; i < (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0; ++i)
	{
		m_arrTex[i] = _origin.m_arrTex[i];
	}	
}

CMaterial::~CMaterial()
{
}

void CMaterial::SetData(SHADER_PARAM _eType, const void* _pData)
{
	switch (_eType)
	{
	case SHADER_PARAM::INT_0:		
	case SHADER_PARAM::INT_1:		
	case SHADER_PARAM::INT_2:
	case SHADER_PARAM::INT_3:
		m_tInfo.arrInt[(UINT)_eType - (UINT)SHADER_PARAM::INT_0] = *(int*)_pData;
		break;
	case SHADER_PARAM::FLOAT_0:
	case SHADER_PARAM::FLOAT_1:
	case SHADER_PARAM::FLOAT_2:
	case SHADER_PARAM::FLOAT_3:
		m_tInfo.arrFloat[(UINT)_eType - (UINT)SHADER_PARAM::FLOAT_0] = *(float*)_pData;
		break;
	case SHADER_PARAM::VEC2_0:
	case SHADER_PARAM::VEC2_1:
	case SHADER_PARAM::VEC2_2:
	case SHADER_PARAM::VEC2_3:
		m_tInfo.arrVec2[(UINT)_eType - (UINT)SHADER_PARAM::VEC2_0] = *(Vec2*)_pData;
		break;
	case SHADER_PARAM::VEC4_0:
	case SHADER_PARAM::VEC4_1:
	case SHADER_PARAM::VEC4_2:
	case SHADER_PARAM::VEC4_3:
		m_tInfo.arrVec4[(UINT)_eType - (UINT)SHADER_PARAM::VEC4_0] = *(Vec4*)_pData;
		break;
	case SHADER_PARAM::MAT_0:
	case SHADER_PARAM::MAT_1:
	case SHADER_PARAM::MAT_2:
	case SHADER_PARAM::MAT_3:
		m_tInfo.arrMat[(UINT)_eType - (UINT)SHADER_PARAM::MAT_0] = *(Matrix*)_pData;
		break;
	case SHADER_PARAM::TEX_0:
	case SHADER_PARAM::TEX_1:
	case SHADER_PARAM::TEX_2:
	case SHADER_PARAM::TEX_3:
	case SHADER_PARAM::TEX_4:
	case SHADER_PARAM::TEX_5:
	case SHADER_PARAM::TEX_6:
	case SHADER_PARAM::TEX_7:
	case SHADER_PARAM::TEXARR_0:
	case SHADER_PARAM::TEXARR_1:
	case SHADER_PARAM::TEXCUBE_0:
	case SHADER_PARAM::TEXCUBE_1:
		m_arrTex[(UINT)_eType - (UINT)SHADER_PARAM::TEX_0] = (CTexture*)_pData;
		break;
	}

	if (!m_bDefault && CSceneMgr::GetInst()->GetCurScene()->GetState() == SCENE_STATE::STOP)
	{
		// Scene 이 Stop 상태일 때만 저장
		wstring strRelativepath = L"material\\";
		strRelativepath += GetKey();
		strRelativepath += L".mtrl";
		Save(strRelativepath);
	}
}

void* CMaterial::GetData(SHADER_PARAM _eType)
{
	switch (_eType)
	{
	case SHADER_PARAM::INT_0:
	case SHADER_PARAM::INT_1:
	case SHADER_PARAM::INT_2:
	case SHADER_PARAM::INT_3:
		return &m_tInfo.arrInt[(UINT)_eType - (UINT)SHADER_PARAM::INT_0];
	case SHADER_PARAM::FLOAT_0:
	case SHADER_PARAM::FLOAT_1:
	case SHADER_PARAM::FLOAT_2:
	case SHADER_PARAM::FLOAT_3:
		return &m_tInfo.arrFloat[(UINT)_eType - (UINT)SHADER_PARAM::FLOAT_0];
	case SHADER_PARAM::VEC2_0:
	case SHADER_PARAM::VEC2_1:
	case SHADER_PARAM::VEC2_2:
	case SHADER_PARAM::VEC2_3:
		return &m_tInfo.arrVec2[(UINT)_eType - (UINT)SHADER_PARAM::VEC2_0];
	case SHADER_PARAM::VEC4_0:
	case SHADER_PARAM::VEC4_1:
	case SHADER_PARAM::VEC4_2:
	case SHADER_PARAM::VEC4_3:
		return &m_tInfo.arrVec4[(UINT)_eType - (UINT)SHADER_PARAM::VEC4_0];		
	case SHADER_PARAM::MAT_0:
	case SHADER_PARAM::MAT_1:
	case SHADER_PARAM::MAT_2:
	case SHADER_PARAM::MAT_3:
		return &m_tInfo.arrMat[(UINT)_eType - (UINT)SHADER_PARAM::MAT_0];
	case SHADER_PARAM::TEX_0:
	case SHADER_PARAM::TEX_1:
	case SHADER_PARAM::TEX_2:
	case SHADER_PARAM::TEX_3:
	case SHADER_PARAM::TEX_4:
	case SHADER_PARAM::TEX_5:
	case SHADER_PARAM::TEX_6:
	case SHADER_PARAM::TEX_7:
	case SHADER_PARAM::TEXARR_0:
	case SHADER_PARAM::TEXARR_1:
	case SHADER_PARAM::TEXCUBE_0:
	case SHADER_PARAM::TEXCUBE_1:
		return &m_arrTex[(UINT)_eType - (UINT)SHADER_PARAM::TEX_0];
	}

	assert(nullptr);
	return nullptr;
}

void CMaterial::UpdateData(int _iInstancing)
{
	m_pShader->UpdateData(_iInstancing);

	UINT iTexCount = (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0;
	for (UINT i = 0; i < iTexCount; ++i)
	{
		if (nullptr != m_arrTex[i])
		{
			m_arrTex[i]->UpdateData(i, (UINT)PIPELINE_STAGE::PS_ALL);
			m_tInfo.arrTexCheck[i] = 1;
		}
		else
		{
			CTexture::Clear(i, (UINT)PIPELINE_STAGE::PS_ALL);
			m_tInfo.arrTexCheck[i] = 0;
		}
	}

	static const CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_tInfo);
	pMtrlBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_ALL);
}


void CMaterial::Clear()
{
	UINT iTexCount = (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0;
	for (UINT i = 0; i < iTexCount; ++i)
	{
		if (nullptr != m_arrTex[i])
		{
			CTexture::Clear(i, (UINT)PIPELINE_STAGE::PS_ALL);
		}
	}

	m_tInfo.iAnim3D[0] = 0;
	m_tInfo.iAnim3D[1] = 0;
}

CMaterial* CMaterial::Clone()
{	
	CMaterial* pCloneMtrl = new CMaterial(*this);

	// ResMgr 에 복사된 Material 을 등록
	CResMgr::GetInst()->AddCloneRes<CMaterial>(pCloneMtrl);

	return pCloneMtrl;
}

void CMaterial::Save(const wstring& _strRelativePath)
{
	wstring strFilePath = CPathMgr::GetResPath();
	strFilePath += _strRelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");
		
	errno_t err = GetLastError();
	if (32 == err)
	{
		return;
	}
	else
	{
		assert(pFile);
	}	

	fwrite(&m_tInfo, sizeof(tMtrlInfo), 1, pFile);

	int i = 0;
	for (; i < (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0; ++i)
	{
		if (nullptr != m_arrTex[i])
		{
			fwrite(&i, sizeof(int), 1, pFile);
			SaveResRefInfo(m_arrTex[i], pFile);
		}		
	}
	i = -1;
	fwrite(&i, sizeof(int), 1, pFile);
	
	SaveResRefInfo(m_pShader, pFile);

	fclose(pFile);
}

void CMaterial::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
	assert(pFile);

	fread(&m_tInfo, sizeof(tMtrlInfo), 1, pFile);


	int i = 0;
	while (true)
	{
		fread(&i, sizeof(int), 1, pFile);
		if (-1 == i)
			break;

		LoadResRefInfo(m_arrTex[i], pFile);
	}

	LoadResRefInfo(m_pShader, pFile);
}

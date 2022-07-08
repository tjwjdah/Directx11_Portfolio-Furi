#include "pch.h"
#include "CLight2D.h"

#include "CRenderMgr.h"
#include "CTransform.h"

CLight2D::CLight2D()
	: CComponent(COMPONENT_TYPE::LIGHT2D)
	, m_info{}
{
}

CLight2D::~CLight2D()
{
}

void CLight2D::finalupdate()
{
	m_info.vWorldPos = Transform()->GetWorldPos();
	m_info.vDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	CRenderMgr::GetInst()->RegisterLight2D(this);
}

void CLight2D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	fwrite(&m_info, sizeof(tLightInfo), 1, _pFile);
}

void CLight2D::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	fread(&m_info, sizeof(tLightInfo), 1, _pFile);
}
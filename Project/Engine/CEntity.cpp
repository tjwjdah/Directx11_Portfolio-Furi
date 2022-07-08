#include "pch.h"
#include "CEntity.h"

UINT CEntity::g_iGlobalID = 0;


CEntity::CEntity()
	: m_iID(g_iGlobalID++)
{
}

CEntity::CEntity(const CEntity& _origin)
	: m_strName(_origin.m_strName)
	, m_iID(g_iGlobalID++)
{
}

CEntity::~CEntity()
{
}

void CEntity::SaveToScene(FILE* _pFile)
{
	SaveWString(m_strName, _pFile);
}

void CEntity::LoadFromScene(FILE* _pFile)
{
	LoadWString(m_strName, _pFile);
}

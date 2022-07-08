#include "pch.h"
#include "CSetPlayerBosPos.h"

#include <Engine\CTransform.h>

void CSetPlayerBosPos::SaveToScene(FILE* _pFile)
{
}

void CSetPlayerBosPos::LoadFromScene(FILE* _pFile)
{
}

CSetPlayerBosPos::CSetPlayerBosPos() : CScript((UINT)SCRIPT_TYPE::SETPLAYERBOSPOS)
, m_Bos(nullptr)
, m_Player(nullptr)
{
}

CSetPlayerBosPos::~CSetPlayerBosPos()
{
}

void CSetPlayerBosPos::awake()
{
}

void CSetPlayerBosPos::update()
{
	Vec2 PlayerPos;
	Vec2 BosPos;
	PlayerPos.x = m_Player->Transform()->GetLocalPos().x;
	//PlayerPos.y = m_Player->Transform()->GetLocalPos().z;
	BosPos.x = m_Bos->Transform()->GetLocalPos().x;
	//BosPos.y = m_Bos->Transform()->GetLocalPos().z;
	Vec2 vPos = (BosPos - PlayerPos) / 2.f;
	
	PlayerPos.x	+= vPos.x;
	//PlayerPos.y += vPos.y;
	
	Transform()->SetLocalPosX(PlayerPos.x);
	//Transform()->SetLocalPosZ(PlayerPos.y);
}


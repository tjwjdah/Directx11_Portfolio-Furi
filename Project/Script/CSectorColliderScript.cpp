#include "pch.h"
#include "CSectorColliderScript.h"

#include <Engine/func.h>
#include <Engine\CTimeMgr.h>
#include <Engine\CEventMgr.h>
#include <Engine\CGameObject.h>

#include <Engine/CSceneMgr.h>

CSectorColliderScript::CSectorColliderScript()
	:CScript((int)SCRIPT_TYPE::SECTORCOLLIDERSCRIPT)
	, m_IsInCollision(false)
{
}

CSectorColliderScript::~CSectorColliderScript()
{
}

void CSectorColliderScript::awake()
{

}

void CSectorColliderScript::update()
{
	if (GetGameObject()->IsDead())
		return;


	//ColliderCheck();
}
void CSectorColliderScript::OnCollisionEnter(CGameObject* _pOther)
{
	//if (ColliderCheck(Vec3 _mPos, Vec3 _TarPos, float _StartAngle, float _EndAngle))
	{
		//플레이어 hit함수 호출
	}
}

void CSectorColliderScript::OnCollision(CGameObject* _pOther)
{
	//if (_pOther->GetScript(L"CTileCollsion")
	//	|| _pOther->GetScript(L"CMonsterScript"))
	//{
	//}
}

void CSectorColliderScript::OnCollisionExit(CGameObject* _pOther)
{
	//if (_pOther->GetScript(L"CTileCollsion")
	//	|| _pOther->GetScript(L"CMonsterScript"))
	//{
	//}
}

#include "pch.h"
#include "CScaleUpScript.h"

#include <Engine\CTimeMgr.h>
#include <Engine\CEventMgr.h>
#include <Engine\CGameObject.h>

#include <Engine/CSceneMgr.h>

#include "CPlayerScript.h"
CScaleUpScript::CScaleUpScript()
	:CScript((int)SCRIPT_TYPE::SCALEUPSCRIPT)
	, m_value(1.2f)
{
}

CScaleUpScript::~CScaleUpScript()
{
}

void CScaleUpScript::awake()
{

}

void CScaleUpScript::update()
{
	if (GetGameObject()->IsDead())
		return;

	Vec3 _myScale = Transform()->GetLocalScale();
	_myScale.x += m_value * fDT * _myScale.x;
	_myScale.z += m_value * fDT * _myScale.z;
	Transform()->SetLocalScale(_myScale);

}

void CScaleUpScript::OnCollisionEnter(CGameObject* _pOther)
{
	if (_pOther->GetName() == L"Player")
	{
		CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");
		if (Player->KnockBack(Transform()->GetLocalPos(), true, HitScale::HitLight, 10))
		{


		}

	}
}

void CScaleUpScript::OnCollisionExit(CGameObject* _pOther)
{
}

void CScaleUpScript::OnCollision(CGameObject* _pOther)
{
}
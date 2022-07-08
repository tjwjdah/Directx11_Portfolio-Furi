#include "pch.h"
#include "CToolCamScript.h"

#include <Engine\CTimeMgr.h>
#include <Engine\CTransform.h>
#include <Engine\CCamera.h>

CToolCamScript::CToolCamScript()
	: CScript(-1)
	, m_fSpeed(500.f)
{
}

CToolCamScript::~CToolCamScript()
{
}

void CToolCamScript::update()
{	
	Vec2 vScale = Camera()->GetScale();

	float fSpeed = m_fSpeed;

	if (KEY_HOLD(KEY_TYPE::LSHIFT))
		fSpeed *= 5.f;

	if (KEY_HOLD(KEY_TYPE::NUM_1))
	{
		vScale += 1.f * fDT;
	}
	else if (KEY_HOLD(KEY_TYPE::NUM_2))
	{
		vScale -= 1.f * fDT;
	}

	Vec3 vPos = Transform()->GetLocalPos();

	if (KEY_HOLD(KEY_TYPE::KEY_W))
	{
		Vec3 vFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);
		vPos += vFront * fDT * fSpeed;
	}

	if (KEY_HOLD(KEY_TYPE::KEY_S))
	{
		Vec3 vBack = -Transform()->GetWorldDir(DIR_TYPE::FRONT);
		vPos += vBack * fDT * fSpeed;
	}

	if (KEY_HOLD(KEY_TYPE::KEY_A))
	{
		Vec3 vLeft = -Transform()->GetWorldDir(DIR_TYPE::RIGHT);
		vPos += vLeft * fDT * fSpeed;
	}

	if (KEY_HOLD(KEY_TYPE::KEY_D))
	{
		Vec3 vRight = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
		vPos += vRight * fDT * fSpeed;
	}

	if (KEY_HOLD(KEY_TYPE::RBTN) && PROJ_TYPE::PERSPECTIVE == Camera()->GetProjType())
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

		Vec3 vRot = Transform()->GetLocalRot();

		vRot.x -= vMouseDir.y * fDT * 5.f;
		vRot.y += vMouseDir.x * fDT * 5.f;

		Transform()->SetLocalRot(vRot);
	}

	Transform()->SetLocalPos(vPos);
}
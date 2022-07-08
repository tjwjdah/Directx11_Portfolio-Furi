#include "pch.h"
#include "CCameraScript.h"
#include <Engine\CCamera.h>

CCameraScript::CCameraScript()
	: CScript((UINT)SCRIPT_TYPE::CAMERASCRIPT)
	, m_fSpeed(500.f)
{
}

CCameraScript::~CCameraScript()
{
}

void CCameraScript::update()
{
	Vec2 vScale = CurCamera->GetScale();

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

	if (KEY_HOLD(KEY_TYPE::KEY_UP))
	{
		
		Vec3 vFront = Transform()->GetLocalDir(DIR_TYPE::FRONT);
		vPos += vFront * fDT * fSpeed;
	}

	if (KEY_HOLD(KEY_TYPE::KEY_DOWN))
	{
		Vec3 vBack = -Transform()->GetLocalDir(DIR_TYPE::FRONT);
		vPos += vBack * fDT * fSpeed;
	}

	if (KEY_HOLD(KEY_TYPE::KEY_RIGHT))
	{
		Vec3 vRight = Transform()->GetLocalDir(DIR_TYPE::RIGHT);
		vPos += vRight * fDT * fSpeed;
	}

	if (KEY_HOLD(KEY_TYPE::KEY_LEFT))
	{
		Vec3 vLeft = -Transform()->GetLocalDir(DIR_TYPE::RIGHT);
		vPos += vLeft * fDT * fSpeed;
	}

	if (KEY_HOLD(KEY_TYPE::LSHIFT) && KEY_HOLD(KEY_TYPE::RBTN) && PROJ_TYPE::PERSPECTIVE == CurCamera->GetProjType())
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

		Vec3 vRot = Transform()->GetLocalRot();

		vRot.x -= vMouseDir.y * fDT * 5.f;
		vRot.y += vMouseDir.x * fDT * 5.f;

		Transform()->SetLocalRot(vRot);
	}

	Transform()->SetLocalPos(vPos);
}
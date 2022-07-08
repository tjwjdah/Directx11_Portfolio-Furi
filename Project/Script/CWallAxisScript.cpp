#include "pch.h"
#include "CWallAxisScript.h"

CWallAxisScript::CWallAxisScript()
	: CScript((int)SCRIPT_TYPE::WALLAXISSCRIPT)
	, m_fSpeed(1.f)
	, m_bClockwise(true)
{

}

CWallAxisScript::~CWallAxisScript()
{

}


void CWallAxisScript::update()
{
	Vec3 vRot = Transform()->GetLocalRot();

	if (m_bClockwise)
	{
		vRot.y += fDT * m_fSpeed;
		if (vRot.y >= XM_2PI)
			vRot.y = 0.f;
	}
	else
	{
		vRot.y -= fDT * m_fSpeed;
		if (vRot.y <= -XM_2PI)
			vRot.y = 0.f;
	}

	Transform()->SetLocalRot(vRot);
}
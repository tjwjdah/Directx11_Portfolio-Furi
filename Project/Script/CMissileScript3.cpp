#include "pch.h"
#include "CMissileScript3.h"

#include <Engine/CStructuredBuffer.h>
#include <Engine/CRenderMgr.h>
#include "CPlayerScript.h"
CMissileScript3::CMissileScript3()
	: CScript((int)SCRIPT_TYPE::MISSILESCRIPT3)
	, m_vScale(Vec3(0.f, 0.f, 0.f))
	, m_fSpeed(270.f)
	, m_fDeadTime(0.f)
	, m_fDeadTimeMax(5.f)
	, b_bAwake(true)
	, m_pSectorBuffer(nullptr)
{
	m_tSectorInfo.fAngle = 0.f;
	m_tSectorInfo.fDistance = 0.f;
}

CMissileScript3::~CMissileScript3()
{
	SAFE_DELETE(m_pSectorBuffer);
}

void CMissileScript3::update()
{
	if (b_bAwake)
	{
		b_bAwake = false;
		m_pSectorBuffer = new CStructuredBuffer;
		m_pSectorBuffer->Create(sizeof(tSectorInfo), 1, nullptr);
	}

	m_vScale = Transform()->GetLocalScale();

	m_vScale.x += fDT * m_fSpeed;
	m_vScale.z += fDT * m_fSpeed;

	if (m_vScale.x > 5000.f)
	{
		m_tSectorInfo.fDistance += fDT * m_fReduce;
	}

	m_fDeadTime += fDT;
	if (m_fDeadTime >= m_fDeadTimeMax)
	{
		DeleteObject(GetObj());
	}

	Transform()->SetLocalScale(m_vScale);
}

void CMissileScript3::UpdateData()
{
	vector<tSectorInfo> vecSector;
	vecSector.push_back(m_tSectorInfo);

	m_pSectorBuffer->SetData(vecSector.data(), sizeof(tSectorInfo), (UINT)1);
	m_pSectorBuffer->UpdateData(75);
}

void CMissileScript3::ClearData()
{
	//m_pSectorBuffer->Clear();
}

void CMissileScript3::OnCollisionEnter(CGameObject* _pOther)
{
	if (_pOther->GetName() == L"Player")
	{
		CPlayerScript* Player = (CPlayerScript*)_pOther->GetScript(L"CPlayerScript");

		Vec3 vDir = _pOther->Transform()->GetLocalPos() - Transform()->GetLocalPos();
		vDir.Normalize();

		bool ret = ColliderCheck(Transform()->GetLocalPos(), Player->Transform()->GetLocalPos(), Vec3(vDir.x, 0.f, vDir.z), m_tSectorInfo.fAngle);

		if (ret) Player->knockdown(Transform()->GetLocalPos(), true, HitScale::HitBig, 10);
	}
}

void CMissileScript3::OnCollision(CGameObject* _pOther)
{
}


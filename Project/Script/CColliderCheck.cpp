#include "pch.h"
#include "CColliderCheck.h"

#include <Engine\CTransform.h>
#include <Engine\CMeshRender.h>
#include <Engine\CMaterial.h>
#include <Engine\CCollider2D.h>
CColliderCheck::CColliderCheck():
	CScript((int)SCRIPT_TYPE::COLLIDERCHECK)
{
}

CColliderCheck::~CColliderCheck()
{
}

void CColliderCheck::OnCollisionEnter(CGameObject* _pOther)
{

}

void CColliderCheck::OnCollisionExit(CGameObject* _pOther)
{

}

void CColliderCheck::OnCollision(CGameObject* _pOther)
{
	if (ColliderDistanceCheck(Transform()->GetLocalPos(), _pOther->Transform()->GetLocalPos(), Transform()->GetLocalScale(), 0.5f))
	{
		int a = 1;
		Ptr<CMaterial> m_pMtrl = Collider2D()->GetCMtrl();
		m_pMtrl->SetData(SHADER_PARAM::INT_0, &a);
	}
	else
	{




		int a = 0;
		Ptr<CMaterial> m_pMtrl = Collider2D()->GetCMtrl();
		m_pMtrl->SetData(SHADER_PARAM::INT_0, &a);

	}
}


void CColliderCheck::update()
{
}

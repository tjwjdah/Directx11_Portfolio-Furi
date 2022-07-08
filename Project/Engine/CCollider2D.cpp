#include "pch.h"
#include "CCollider2D.h"

#include "CResMgr.h"
#include "CMaterial.h"

#include "CSceneMgr.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CScript.h"


CCollider2D::CCollider2D()
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_vOffsetScale(Vec2(1.f, 1.f))
	, m_vOffsetPos(Vec2(0.f, 0.f))
	, m_Collider2DType(COLLIDER2D_TYPE::CIRCLE)
	, m_iCollisionCount(0)
	, m_Pos(Vec2(0.f, 0.f))
	, m_Scale(Vec2(1.f, 1.f))
{
	SetCollider2DType(m_Collider2DType);
	m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Collider3DMtrl");
}

CCollider2D::CCollider2D(const CCollider2D& _origin)
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_vOffsetScale(_origin.m_vOffsetScale)
	, m_vOffsetPos(_origin.m_vOffsetPos)
	, m_Collider2DType(_origin.m_Collider2DType)
	, m_iCollisionCount(0)
{
	SetCollider2DType(m_Collider2DType);
	m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Collider3DMtrl");
}

CCollider2D::~CCollider2D()
{
}

void CCollider2D::finalupdate()
{
	m_matColWorld = Transform()->GetWorldMat();

	Matrix matColTrans = XMMatrixTranslation(m_vOffsetPos.x, 0.f, m_vOffsetPos.y);
	Matrix matColScale = XMMatrixScaling(m_vOffsetScale.x, 1.f, m_vOffsetScale.y);

	if (COLLIDER2D_TYPE::CIRCLE == m_Collider2DType)
	{
		Vec3 vOriginScale = Transform()->GetWorldScale();

		XMVECTOR vTrans;
		XMVECTOR vScale;
		XMVECTOR vRot;
		XMMatrixDecompose(&vScale, &vRot, &vTrans, m_matColWorld);
		
		if (vOriginScale.x > vOriginScale.z)
		{
			vScale = Vec4(vOriginScale.x, vOriginScale.x, vOriginScale.x, 0.f);
		}
		else
		{
			vScale = Vec4(vOriginScale.z, vOriginScale.z, vOriginScale.z, 0.f);
		}
		//vRot = Vec4(0.f, 0.f, .f, 1.f);
		m_matColWorld = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRot, vTrans);
	}
	m_Pos.x = Transform()->GetWorldPos().x + m_vOffsetPos.x;
	m_Pos.y = Transform()->GetWorldPos().z + m_vOffsetPos.y;
	m_Scale = Vec2(Transform()->GetLocalScale().x * m_vOffsetScale.x, Transform()->GetLocalScale().z * m_vOffsetScale.y);
	m_matColWorld = matColScale * m_matColWorld * matColTrans;
}

void CCollider2D::render()
{
	if (CSceneMgr::GetInst()->GetColShow())
	{
		UpdateData();

		m_pMtrl->UpdateData();

		m_pMesh->UpdateData(0);
		m_pMesh->render(0);
	}
}

void CCollider2D::SetCollider2DType(COLLIDER2D_TYPE _eType)
{
	m_Collider2DType = _eType;

	if (COLLIDER2D_TYPE::RECT == m_Collider2DType)
	{
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh");//RectMesh_C2D");
	}
	else
	{
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh_C2D");
		//int a = 1;
	}
}

void CCollider2D::OnCollisionEnter(CCollider2D* _pOther)
{
	if (0 == m_iCollisionCount)
	{
		m_pMtrl = m_pMtrl->Clone();
	}

	++m_iCollisionCount;

	int a = 1;
	m_pMtrl->SetData(SHADER_PARAM::INT_0, &a);

	// Script 에 알림
	const vector<CScript*>& vecScript = GetObj()->GetScripts();	
	for (size_t i = 0; i < vecScript.size(); ++i)
	{		
		vecScript[i]->OnCollisionEnter(_pOther->GetObj());
	}	
}

void CCollider2D::OnCollision(CCollider2D* _pOther)
{
	// Script 에 알림	
	const vector<CScript*>& vecScript = GetObj()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnCollision(_pOther->GetObj());
	}
}

void CCollider2D::OnCollisionExit(CCollider2D* _pOther)
{
	--m_iCollisionCount;

	if (0 == m_iCollisionCount)
	{		
		m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Collider2DMtrl");
	}

	const vector<CScript*>& vecScript = GetObj()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnCollisionExit(_pOther->GetObj());
	}
}

void CCollider2D::UpdateData()
{
	static const CConstBuffer* pBuffer = CDevice::GetInst()->GetCB(CB_TYPE::TRANSFORM);

	g_transform.matWorld = m_matColWorld;
	g_transform.matWV = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP = g_transform.matWV * g_transform.matProj;

	pBuffer->SetData(&g_transform);
	pBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_VERTEX);
}

void CCollider2D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	fwrite(&m_vOffsetScale, sizeof(Vec2), 1, _pFile);
	fwrite(&m_vOffsetPos, sizeof(Vec2), 1, _pFile);
	fwrite(&m_Collider2DType, sizeof(UINT), 1, _pFile);
}

void CCollider2D::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	fread(&m_vOffsetScale, sizeof(Vec2), 1, _pFile);
	fread(&m_vOffsetPos, sizeof(Vec2), 1, _pFile);
	fread(&m_Collider2DType, sizeof(UINT), 1, _pFile);

	SetCollider2DType(m_Collider2DType);
}
#include "pch.h"
#include "CCollisionMgr.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"

#include "CGameObject.h"
#include "CCollider2D.h"
#include "CTransform.h"

CCollisionMgr::CCollisionMgr()
	: m_arrCheck{}
{
}

CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::update()
{
	for (UINT iRow = 0; iRow < MAX_LAYER; ++iRow)
	{
		for (UINT iCol = iRow; iCol < MAX_LAYER; ++iCol)
		{
			if (m_arrCheck[iRow] & (1 << iCol))
			{
				CollisionGroup(iRow, iCol);				
			}
		}
	}

}

void CCollisionMgr::CollisionGroup(int _iLeft, int _iRight)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	const vector<CGameObject*>& vecLeft = pCurScene->GetLayer(_iLeft)->GetObjects();
	const vector<CGameObject*>& vecRight = pCurScene->GetLayer(_iRight)->GetObjects();

	for (size_t i = 0; i < vecLeft.size(); ++i)
	{
		if (nullptr == vecLeft[i]->Collider2D())
			continue;

		for (size_t j = 0; j < vecRight.size(); ++j)
		{
			if (nullptr == vecRight[j]->Collider2D() || vecLeft[i] == vecRight[j])
			{
				continue;
			}

			// 두 충돌체 조합 아이디 생성
			COLLIDER_PAIR pairid;
			pairid.iLeftID = vecLeft[i]->Collider2D()->GetID();
			pairid.iRightID = vecRight[j]->Collider2D()->GetID();

			map<LONGLONG, bool>::iterator iter = m_mapColPairInfo.find(pairid.iPairID);
			if (m_mapColPairInfo.end() == iter)
			{
				m_mapColPairInfo.insert(make_pair(pairid.iPairID, false));
				iter = m_mapColPairInfo.find(pairid.iPairID);
			}

			// 현재 기준, 충돌 중인지 검사
			if(Collision(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D()))
			{
				// 충돌 했다.
				if (iter->second) // 이전에도 충돌했었다.
				{					
					// 두 오브젝트중 하나라도 삭제 예정상태라면
					if (vecLeft[i]->IsDead() || vecRight[j]->IsDead())
					{
						vecLeft[i]->Collider2D()->OnCollisionExit(vecRight[j]->Collider2D());
						vecRight[j]->Collider2D()->OnCollisionExit(vecLeft[i]->Collider2D());
					}
					else
					{
						vecLeft[i]->Collider2D()->OnCollision(vecRight[j]->Collider2D());
						vecRight[j]->Collider2D()->OnCollision(vecLeft[i]->Collider2D());
					}					
				}
				else // 이전에는 충돌하지 않았다.
				{
					// 두 오브젝트중 하나라도 삭제 예정상태라면
					if (vecLeft[i]->IsDead() || vecRight[j]->IsDead())					
						continue;

					vecLeft[i]->Collider2D()->OnCollisionEnter(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->OnCollisionEnter(vecLeft[i]->Collider2D());
					iter->second = true;
				}
			}
			else // 충돌하지 않는다.
			{				
				if (iter->second) // 이전엔 충돌 했었다.
				{
					vecLeft[i]->Collider2D()->OnCollisionExit(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->OnCollisionExit(vecLeft[i]->Collider2D());
					iter->second = false;
				}
			}
		}
	}
}

bool CCollisionMgr::Collision(CCollider2D* _pLeftCol, CCollider2D* _pRightCol)
{
	if (COLLIDER2D_TYPE::RECT == _pLeftCol->GetCollider2DType()
		&& COLLIDER2D_TYPE::RECT == _pRightCol->GetCollider2DType())
	{
		return CollisionRvsR(_pLeftCol, _pRightCol);
	}

	else if (COLLIDER2D_TYPE::CIRCLE == _pLeftCol->GetCollider2DType()
		&& COLLIDER2D_TYPE::CIRCLE == _pRightCol->GetCollider2DType())
	{
		return CollisionCvsC(_pLeftCol, _pRightCol);
	}

	return false;
}

bool CCollisionMgr::CollisionRvsR(CCollider2D* _pLeftCol, CCollider2D* _pRightCol)
{
	const Matrix& matColLeft = _pLeftCol->GetmatColWorld();
	const Matrix& matColRight = _pRightCol->GetmatColWorld();

	// 0 -- 1
	// |    |
	// 3 -- 2
	static const Vec3 vLocalMesh[4] = { Vec3(-0.5f, 0.5f, 0.f)
		, Vec3(0.5f, 0.5f, 0.f)
		, Vec3(0.5f, -0.5f, 0.f)
		, Vec3(-0.5f, -0.5f, 0.f) };

	// 두 충돌체를 이은 벡터
	Vec2 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColLeft) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColRight);

	// 동시에 투영시킬 벡터
	Vec2 vAxis[4] = {};
		
	vAxis[0] = XMVector3TransformCoord(vLocalMesh[1], matColLeft) - XMVector3TransformCoord(vLocalMesh[0], matColLeft);
	vAxis[1] = XMVector3TransformCoord(vLocalMesh[3], matColLeft) - XMVector3TransformCoord(vLocalMesh[0], matColLeft);
	vAxis[2] = XMVector3TransformCoord(vLocalMesh[1], matColRight) - XMVector3TransformCoord(vLocalMesh[0], matColRight);
	vAxis[3] = XMVector3TransformCoord(vLocalMesh[3], matColRight) - XMVector3TransformCoord(vLocalMesh[0], matColRight);
	
	// 투영 축
	Vec2 vProjAxis[4] = {};
	for (int i = 0; i < 4; ++i)
	{
		vProjAxis[i] = vAxis[i];
		vProjAxis[i].Normalize();
	}

	for (int i = 0; i < 4; ++i)
	{
		float fProjDist = 0.f;
		for (int j = 0; j < 4; ++j)
		{			
			fProjDist += abs(vProjAxis[i].Dot(vAxis[j])) / 2.f;
		}

		if (fProjDist < abs(vProjAxis[i].Dot(vCenter)))
		{
			return false;
		}
	}

	return true;
}

bool CCollisionMgr::CollisionCvsC(CCollider2D* _pLeftCol, CCollider2D* _pRightCol)
{
	/*
	Vec3	vLeftPos = _pLeftCol->GetObj()->Transform()->GetWorldPos() + _pLeftCol->GetvOffsetPos();
	Vec3	vLeftScale = _pLeftCol->GetObj()->Transform()->GetWorldScale() * _pLeftCol->GetvOffsetScale();

	Vec3	vRightPos = _pRightCol->GetObj()->Transform()->GetWorldPos() + _pRightCol->GetvOffsetPos();
	Vec3	vRightScale = _pRightCol->GetObj()->Transform()->GetWorldScale() * _pRightCol->GetvOffsetScale();
	*/
	Vec2	vLeftPos = _pLeftCol->GetPos();
	Vec2	vLeftScale = _pLeftCol->GetScale();

	Vec2	vRightPos = _pRightCol->GetPos();
	Vec2	vRightScale = _pRightCol->GetScale();


	float fDistance = pow((vLeftPos.x - vRightPos.x), 2) + pow((vLeftPos.y - vRightPos.y), 2);
	float fRadius = pow(((vLeftScale.x + vRightScale.x) / 2), 2);

	if (fDistance <= fRadius)
	{
		return true;
	}
	else
		return false;
}


void CCollisionMgr::CollisionCheckClear()
{
	memset(m_arrCheck, 0, sizeof(UINT) * MAX_LAYER);
}

void CCollisionMgr::CollisionCheck(int _iLeft, int _iRight)
{
	int iRow = -1;
	int iCol = -1;

	if ((UINT)_iLeft < (UINT)_iRight)
	{
		iRow = (UINT)_iLeft;
		iCol = (UINT)_iRight;
	}
	else
	{
		iRow = (UINT)_iRight;
		iCol = (UINT)_iLeft;
	}

	if (m_arrCheck[iRow] & (1 << iCol))
	{
		m_arrCheck[iRow] &= ~(1 << iCol);
	}
	else
	{
		m_arrCheck[iRow] |= 1 << iCol;
	}
}

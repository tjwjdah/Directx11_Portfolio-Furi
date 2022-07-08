#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CCollider2D :
    public CComponent
{
private:
    Vec2            m_vOffsetScale;
    Vec2            m_vOffsetPos;

    COLLIDER2D_TYPE m_Collider2DType;

    Ptr<CMesh>      m_pMesh;
    Ptr<CMaterial>  m_pMtrl;

    Matrix          m_matColWorld;

   
    int             m_iCollisionCount;

    Vec2            m_Pos;
    Vec2            m_Scale;
public:
    virtual void finalupdate();
    void render();

    Ptr<CMaterial> GetCMtrl() { return m_pMtrl; }
public:
    // 충돌 진입 프레임 시 호출되는 함수
    virtual void OnCollisionEnter(CCollider2D* _pOther);

    // 충돌 유지 중에 호출되는 함수
    virtual void OnCollision(CCollider2D* _pOther);

    // 충돌이 해제되는 프레임 시 호출되는 함수
    virtual void OnCollisionExit(CCollider2D* _pOther);

private:
    void UpdateData();


public:
    GET_SET(Vec2, vOffsetScale);
    GET_SET(Vec2, vOffsetPos);
    GET(COLLIDER2D_TYPE, Collider2DType);
    void SetCollider2DType(COLLIDER2D_TYPE _eType);
    CLONE(CCollider2D);
    GET(const Matrix&, matColWorld);
    Vec2 GetPos() { return m_Pos; }
    Vec2 GetScale() { return m_Scale; }
    
    Ptr<CMaterial> GetCmaterial() { return m_pMtrl; }
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);

public:
    CCollider2D();
    CCollider2D(const CCollider2D& _origin);
    ~CCollider2D();        
};


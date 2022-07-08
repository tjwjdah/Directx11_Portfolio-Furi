#pragma once
#include "CComponent.h"
#include "CFrustum.h"


class CCollider2D;
class CCamera :
    public CComponent
{
protected:
    CFrustum        m_frustum;

    PROJ_TYPE       m_eProjType;
    Vec2            m_vScale;   // 직교투영 범위 배율
    Vec2            m_vProjRange;

    Matrix          m_matView;
    Matrix          m_matViewInv;

    Matrix          m_matProj;
    Matrix          m_matProjInv;

    tRay            m_tRay; // 카메라에서 마우스 방향을 향하는 직선

    float           m_fFOV;
    float           m_fFar;

    UINT            m_iLayerCheck;

    //vector<CGameObject*>    m_vecDeferred;
    //vector<CGameObject*>    m_vecForward;
    vector<CGameObject*>    m_vecUi;  //////////////////////*********************************************추가
    vector<CGameObject*>    m_vecDeferredParticle;
    vector<CGameObject*>    m_vecParticle;
    vector<CCollider2D*>   m_vecCollider;
    vector<CGameObject*>    m_vecPostEffect;

    // 인스턴싱 버전
    map<ULONG64, vector<tInstObj>>		m_mapInstGroup_D;	// Deferred
    map<ULONG64, vector<tInstObj>>		m_mapInstGroup_F;	// Foward	
    map<INT_PTR, vector<tInstObj>>		m_mapSingleObj;		// Single Object



public:
    virtual void update();
    virtual void finalupdate();

public:
    void SetProjType(PROJ_TYPE _eType) { m_eProjType = _eType; }
    void SetScale(Vec2 _vScale) { m_vScale = _vScale; }
    void SetFar(float _f) { m_fFar = _f; }
    void SetLayerCheck(UINT _iLayerIdx, bool _bAdd);
    void SetLayerAllCheck() { m_iLayerCheck = 0xffffffff; }
    void SetOrthographicProjRange(Vec2 _vRange) { m_vProjRange = _vRange; }

    PROJ_TYPE GetProjType() { return m_eProjType; }
    Vec2 GetScale() { return m_vScale; }
    const tRay& GetRay() { return m_tRay; }

    CFrustum& GetFrustum() { return m_frustum; }

    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetViewInvMat() { return m_matViewInv; }
    const Matrix& GetProjMat() { return m_matProj; }
    const Matrix& GetProjInvMat() { return m_matProjInv; }

public:
    void SortObject();
    void render_deferred();
    void render_forward();
    void render_posteffect();
    void render_collider();
    virtual void render_ui(); /////////*****************************************추가

protected:
    void CalViewMat();
    void CalProjMat();
    void CalRay();

public:
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);
    CLONE(CCamera);

public:
    CCamera();
    ~CCamera();
};


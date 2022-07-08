#pragma once
#include "CEntity.h"
#include "CGameObject.h"

class CTransform;
class CMeshRender;
class CCamera;
class CAnimator2D;
class CAnimator3D;

class CComponent :
    public CEntity
{
private:
    COMPONENT_TYPE  m_eComType;
    CGameObject*    m_pObject;
    bool            m_bAtive;

   
public:
    virtual void awake() {}
    virtual void start() {}
    virtual void update() {};
    virtual void lateupdate() {};
    virtual void finalupdate() = 0;

public:
    void SetActive(bool _b) { m_bAtive = _b; }
    bool IsActive() { return m_bAtive; }
   
public:
    COMPONENT_TYPE GetType() { return m_eComType; }
    CGameObject* GetGameObject() { return m_pObject; };

    CTransform* Transform() { return m_pObject->Transform(); }
    CMeshRender* MeshRender() { return m_pObject->MeshRender(); }
    CCollider2D* Collider2D() { return m_pObject->Collider2D(); }
    CAnimator2D* Animator2D() { return m_pObject->Animator2D(); }
    CAnimator3D* Animator3D() { return m_pObject->Animator3D(); }
    CCamera* Camera() { return m_pObject->Camera(); }
    CLight2D* Light2D() { return m_pObject->Light2D(); }

    CGameObject* GetObj() { return m_pObject; }

private:
    void SetGameObject(CGameObject* _pObject) { m_pObject = _pObject; }
    
public:
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);
    virtual CComponent* Clone() = 0;

public:
    CComponent(COMPONENT_TYPE _eType);
    CComponent(const CComponent& _origin);
    ~CComponent();

    friend class CGameObject;
};


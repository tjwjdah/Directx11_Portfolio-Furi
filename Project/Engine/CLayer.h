#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
private:
    vector<CGameObject*>        m_vecParentObj; // 최상위 부모 오브젝트
    vector<CGameObject*>        m_vecObject;    // 레이어 소속 모든 오브젝트
    int                         m_iLayerIndex;

public:
    void awake();
    void start();

    void update();
    void lateupdate();
    void finalupdate();

public:
    // _pObject : Layer 에 입력되는 Object, _bMoveChild : 자식 오브젝트의 레이어 소속을 같이 변경할지 여부
    void AddObject(CGameObject* _pObject, bool _bMoveChild);
    void ClearObject() { m_vecObject.clear(); }

    // Layer 에 소속된 최상위 부모오브젝트 목록을 가져온다.
    const vector<CGameObject*>& GetParentObj() { return m_vecParentObj; }

    // Layer 에 소속된 모든 오브젝트 목록을 가져온다(finalupdate 이후 유효)
    const vector<CGameObject*>& GetObjects() { return m_vecObject; }

private:
    void RegisterObject(CGameObject* _pObj){m_vecObject.push_back(_pObj);}

protected:
    CLONE_DISABLE(CLayer);
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);

public:
    CLayer(int _iLayerIdx);
    ~CLayer();

    friend class CGameObject;
    friend class CScene;
};


#pragma once
#include "CEntity.h"

class CLayer;
class CGameObject;

class CScene :
    public CEntity
{
private:
    CLayer*     m_arrLayer[MAX_LAYER];
    SCENE_STATE m_eState;

public:
    void awake();
    void start();

    void update();
    void lateupdate();    
    void finalupdate();

public:
    void AddObject(CGameObject* _pObject, int _iLayerIdx);
    CLayer* GetLayer(int _iLayerIdx) 
    { 
        assert(!(_iLayerIdx < 0 || MAX_LAYER <= _iLayerIdx));
        return m_arrLayer[_iLayerIdx]; 
    }

    CGameObject* FindObjectByName(const wstring& _strName);
    SCENE_STATE GetState() { return m_eState; }
    void SetState(SCENE_STATE _eState) { m_eState = _eState; }
 

public:
    CLONE_DISABLE(CScene);
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);

public:
    CScene();
    ~CScene();   
};


#pragma once
#include <Engine\CScript.h>
class CSetPlayerBosPos :
    public CScript
{
private:
    CGameObject* m_Player;
    CGameObject* m_Bos;
public:
    virtual void awake();
    virtual void update();

private:

public:
    void SetPlayer(CGameObject* _Obj) { m_Player = _Obj; }
    void SetBos(CGameObject* _Obj) { m_Bos = _Obj; }

public:
    CLONE(CSetPlayerBosPos);
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);


public:
    CSetPlayerBosPos();
    ~CSetPlayerBosPos();
};


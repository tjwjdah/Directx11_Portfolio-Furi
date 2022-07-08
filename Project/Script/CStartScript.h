#pragma once
#include <Engine\CScript.h>


class CStartScript:
    public CScript
{
private:
    Ptr<CPrefab> but1;
    Ptr<CPrefab> but2;
    Ptr<CPrefab> startlogo;

    CGameObject* button1;
    CGameObject* button2;
    CGameObject* logo;

    float m_fStartTime;
    bool m_bNextScene;
    bool m_bCreateFlag;
    bool m_bIsDead;

public:
    virtual void awake();
    virtual void update();

public:
    CLONE(CStartScript);
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);

public:
    CStartScript();
    ~CStartScript();
};

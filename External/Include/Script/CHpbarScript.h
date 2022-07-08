#pragma once
#include <Engine\CScript.h>

enum class BOSS
{
    LAW,
    WISE,
    FATHER,
    NONE
};

class CPlayerScript;
class CLawScript;
class CWiseScript;
class CHpbarScript :
    public CScript
{
private:
    BOSS m_BossName;

    Ptr<CPrefab> m_hpPrefab;
    Ptr<CPrefab> m_hpPrefab2;
    Ptr<CPrefab> m_lifePrefab;

    CLawScript* m_Law; //위즈 추가해서 이름바꿈
    //위즈추가
    CWiseScript* m_Wise;
    CPlayerScript* m_Player;

    CGameObject* m_hp_player;
    CGameObject* m_hp_boss;

    CGameObject* m_life_player[5];
    CGameObject* m_life_boss[3];

public:
    virtual void awake();
    virtual void update();

    void SetBossName(BOSS _bossname) { m_BossName = _bossname; }
public:
    CLONE(CHpbarScript);
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);


public:
    CHpbarScript();
    ~CHpbarScript();
};


#pragma once
#include <Engine\CScript.h>

class CStart2Script :
    public CScript
{
private:
    Ptr<CPrefab> start2logo;

    CGameObject* logo;

    UINT m_iAniNumber;
    bool m_bMainFlag; 
    bool m_bAniFlag;


    bool m_bStart;
    bool m_bIsDead;
public:
    virtual void awake();
    virtual void update();

public:
    CLONE(CStart2Script);
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);

public:
    CStart2Script();
    ~CStart2Script();
};

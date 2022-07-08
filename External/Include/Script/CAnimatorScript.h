#pragma once
#include <Engine\CScript.h>
class CAnimatorScript :
    public CScript
{
private:
    int m_ChangeAnimClip;
    int m_PreChangeAnimClip;
public:
    virtual void awake();
    virtual void update();

private:
   
public:
    void SetAnimationClipNum(UINT _i);
    void SetAnimationClipNum(const vector<CGameObject*>& _Childs,UINT _i);

public:
    CLONE(CAnimatorScript);
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);

    
public:
    CAnimatorScript();
    ~CAnimatorScript();
};


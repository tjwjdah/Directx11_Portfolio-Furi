#pragma once
#include <Engine\CScript.h>



class CSkyBoxScript :
    public CScript
{
private:
    int         m_iBoxType; // 0 cube, 1 sphere

public:
    virtual void awake();
    virtual void update();



public:
    void SetBoxType(void);
    

public:
    CLONE(CSkyBoxScript);
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);


public:
    CSkyBoxScript();
    ~CSkyBoxScript();
};


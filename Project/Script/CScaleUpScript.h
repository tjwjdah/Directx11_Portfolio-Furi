#pragma once
#include <Engine\CScript.h>

class CPrefab;
class CScaleUpScript :
    public CScript
{
private:
    float m_value;

public:
    virtual void update();
    virtual void awake();

    void Setvalue(float _value) { m_value = _value; }
public:
    CLONE(CScaleUpScript);

public:
    CScaleUpScript();
    ~CScaleUpScript();

    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);
};


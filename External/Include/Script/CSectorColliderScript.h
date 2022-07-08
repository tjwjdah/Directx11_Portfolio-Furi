#pragma once
#include <Engine\CScript.h>

class CPrefab;
class CSectorColliderScript :
    public CScript
{
private:

    bool m_IsInCollision;

    Vec3 m_LawPos;
    Vec3 m_TarPos;

    float m_fStartAng;
    float m_fEndAng;

public:
    virtual void update();
    virtual void awake();

public:
    void SetColliderInfo(Vec3 lawpos, Vec3 tarpos, float startang, float endang)
    {
        m_LawPos = lawpos;
        m_TarPos = tarpos;
        m_fStartAng = startang;
        m_fEndAng = endang;
    }
    bool GetIsInCollision() { return m_IsInCollision; }

public:
    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);

public:
    CLONE(CSectorColliderScript);

public:
    CSectorColliderScript();
    ~CSectorColliderScript();

};


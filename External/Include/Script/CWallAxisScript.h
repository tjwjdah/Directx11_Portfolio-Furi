#pragma once
#include <Engine\CScript.h>

class CWallAxisScript :
    public CScript
{
private:
    float   m_fSpeed;
    bool    m_bClockwise;

public:
    virtual void update();

public:
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }
    void SetClockwise(bool _bClockwise) { m_bClockwise = _bClockwise; }

public:
    CLONE(CWallAxisScript);

public:
    CWallAxisScript();
    ~CWallAxisScript();
};


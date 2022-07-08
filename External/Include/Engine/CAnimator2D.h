#pragma once
#include "CComponent.h"

#include "CTexture.h"

class CAnimation2D;

class CAnimator2D :
    public CComponent
{
private:
    map<wstring, CAnimation2D*> m_mapAnim;
    CAnimation2D*               m_pCurAnim;
    bool                        m_bRepeat;

public:
    virtual void lateupdate();    
    virtual void finalupdate() {};

public:
    void ChangeAnimation(const wstring& _strAnimName);
    void CreateAnimation(const wstring& _strAnimName, Ptr<CTexture> _pAnimTex, Vec2 _vLeftTop, Vec2 _vStep, int _iFrameCount, float _fDuration);
    
    CAnimation2D* GetCurAnim() { return m_pCurAnim; }
    CAnimation2D* FindAnimation(const wstring& _strName);
    void PlayAnimation(const wstring& _strName, bool _bRepeat = true, bool _bInit = true);

    void UpdateData();
    static void Clear();

public:
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);
    CLONE(CAnimator2D);

public:
    CAnimator2D();
    CAnimator2D(const CAnimator2D& _origin);
    ~CAnimator2D();
};


#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"
#include "CParticleUpdateShader.h"

class CStructuredBuffer;


class CParticleSystem :
    public CComponent
{
private:
    CStructuredBuffer* m_pParticleBuffer;

    Ptr<CMesh>                  m_pMesh;
    Ptr<CMaterial>              m_pMtrl;
    Ptr<CParticleUpdateShader>  m_pUpdateShader;

    int                         m_iMaxCount;    // 파티클 최대 수용량
    int                         m_iAliveCount;  // 이번에 활성화 시킬 파티클 개수
    float                       m_fMinLifeTime;
    float                       m_fMaxLifeTime;
    float                       m_fMinSpeed;
    float                       m_fMaxSpeed;
    float                       m_fFrequency;   // 파티클 생성 주기
    float                       m_fAccTime;     // 누적시간

    Vec4                        m_vCreateRange; // 파티클 생성 범위

    Vec4                        m_vStartColor;
    Vec4                        m_vEndColor;
    Vec3                        m_vStartScale;
    Vec3                        m_vEndScale;

    //9/5 추가
    bool                        m_bTest;

    int                         m_iDirection;

    //(22.1.3) 추가 
    int                         m_iShaderIdx;  //test
    Ptr<CTexture>               m_pParticleTex;
    wstring                     m_pParticleName;

    int                         m_iType;

   
public:
    void SetShaderIdx(int _int) { m_iShaderIdx = _int; }//(22.1.3)
    void SetPaticleTex(Ptr<CTexture> _Tex) { m_pParticleTex = _Tex; }//(22.1.3)
    void SetPaticleName(const wstring& _strKey) { m_pParticleName = _strKey; }//(22.1.3)

public:
    virtual void update() {}
    virtual void finalupdate();
    void render();

public:
    void SetMaxParticleCount(int _iMaxCount);
    void SetTest(bool _bTest) { m_bTest = _bTest; }
    void SetDirection(int _Idx) { m_iDirection = _Idx; }
    GET_SET(int, iMaxCount);
    GET_SET(int, iAliveCount);
    GET_SET(float, fMinLifeTime);
    GET_SET(float, fMaxLifeTime);
    GET_SET(float, fMinSpeed);
    GET_SET(float, fMaxSpeed);
    GET_SET(float, fFrequency);
    GET_SET(float, fAccTime);
    GET_SET(Vec4, vCreateRange);
    GET_SET(Vec4, vStartColor);
    GET_SET(Vec4, vEndColor);
    GET_SET(Vec3, vStartScale);
    GET_SET(Vec3, vEndScale);
    GET_SET(int, iType);

    void SetMaterial(Ptr<CMaterial> _Material) { m_pMtrl = _Material; }

    void CreateAnimation(Vec2 _vLeftTop, Vec2 _vStep, int _iFrameCount, float _fDuration);
public:
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);
    CLONE(CParticleSystem);

public:
    CParticleSystem();
    CParticleSystem(const CParticleSystem& _origin);
    ~CParticleSystem();
};

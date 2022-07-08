#include "pch.h"
#include "CParticleSystem.h"

#include "CTransform.h"
#include "CStructuredBuffer.h"
#include "CResMgr.h"
#include "CParticleUpdateShader.h"
#include "CTimeMgr.h"

#include "CAnimation2D.h"
#include "CAnimator2D.h"
CParticleSystem::CParticleSystem()
    : CComponent(COMPONENT_TYPE::PARTICLE)
    , m_iMaxCount(100)
    , m_iAliveCount(0)
    , m_fMinLifeTime(0.1f)
    , m_fMaxLifeTime(0.2f)
    , m_fMinSpeed(20.f)
    , m_fMaxSpeed(40.f)
    , m_fFrequency(0.01f)
    , m_fAccTime(0.f)
    , m_vCreateRange(Vec3(1.f, 1.f, 1.f))
    , m_vStartColor(Vec4(0.87f, 0.224f, 0.0f, 0.8f))
    , m_vEndColor(Vec4(1.f, 0.0f, 0.0f, 0.3f))
    , m_vStartScale(Vec3(100.f, 100.f, 1.f))
    , m_vEndScale(Vec3(1.f, 1.f, 1.f))
    , m_iDirection(1)
    , m_iShaderIdx(0)//(22.1.3)
    , m_iType(0)
{
    m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh");
    m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl");
    m_pUpdateShader = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").Get();

    m_pParticleBuffer = new CStructuredBuffer;
    m_pParticleBuffer->CreateRW(sizeof(tParticle), m_iMaxCount, nullptr);

}

CParticleSystem::CParticleSystem(const CParticleSystem& _origin)
    : CComponent(COMPONENT_TYPE::PARTICLE)
    , m_iMaxCount(_origin.m_iMaxCount) //1000
    , m_iAliveCount(_origin.m_iAliveCount)  //0
    , m_fMinLifeTime(_origin.m_fMinLifeTime) //0.5
    , m_fMaxLifeTime(_origin.m_fMaxLifeTime) //0.5
    , m_fMinSpeed(_origin.m_fMinSpeed)
    , m_fMaxSpeed(_origin.m_fMaxSpeed)
    , m_fFrequency(_origin.m_fFrequency)  //0.1
    , m_fAccTime(_origin.m_fAccTime)
    , m_vCreateRange(_origin.m_vCreateRange) //0.1
    , m_vStartColor(_origin.m_vStartColor)
    , m_vEndColor(_origin.m_vEndColor) //1.f
    , m_vStartScale(_origin.m_vStartScale)//(100.f, 100.f, 1.f))
    , m_vEndScale(_origin.m_vEndScale)//(120.f, 120.f, 1.f))
    //, m_pParticleBuffer(_origin.m_pParticleBuffer)
    , m_iShaderIdx(0)//(22.1.3)
    , m_iType(_origin.m_iType)
{
    m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh");
    m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl");
    m_pUpdateShader = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").Get();

    m_pParticleBuffer = new CStructuredBuffer;
    m_pParticleBuffer->CreateRW(sizeof(tParticle), m_iMaxCount, nullptr);


}
CParticleSystem::~CParticleSystem()
{
    delete m_pParticleBuffer;
}

void CParticleSystem::finalupdate()
{
    m_fAccTime += fDT;
    if (m_fAccTime >= m_fFrequency)
    {
        m_fAccTime = 0.f;
        m_iAliveCount = 2;
    }
    else
    {
        m_iAliveCount = 0;
    }

    Vec3 vPos = Transform()->GetWorldPos();
    if (m_bTest)
        m_iAliveCount = 0;
    m_pUpdateShader->SetMaxParticle(m_iMaxCount);
    m_pUpdateShader->SetAliveCount(m_iAliveCount);
    m_pUpdateShader->SetMinLifeTime(m_fMinLifeTime);
    m_pUpdateShader->SetMaxLifeTime(m_fMaxLifeTime);
    m_pUpdateShader->SetType(m_iType);
    m_pUpdateShader->SetMinSpeed(m_fMinSpeed);
    m_pUpdateShader->SetMaxSpeed(m_fMaxSpeed);
    m_pUpdateShader->SetParticelCreateRange(m_vCreateRange);
    m_pUpdateShader->SetParticleWorldPos(Transform()->GetWorldPos());
    m_pUpdateShader->SetParticleBuffer(m_pParticleBuffer);
    m_pUpdateShader->SetDirection(m_iDirection);
    m_pUpdateShader->SetTestidx(m_iShaderIdx);//(22.1.3)
    m_pUpdateShader->Excute();
}

void CParticleSystem::render()
{
    Transform()->UpdateData();


    if (m_iShaderIdx == 0) // 도현
    {
        // 렌더링에 필요한 데이터 전달
        m_pMtrl->SetData(SHADER_PARAM::VEC4_0, &m_vStartScale);
        m_pMtrl->SetData(SHADER_PARAM::VEC4_1, &m_vEndScale);
        m_pMtrl->SetData(SHADER_PARAM::VEC4_2, &m_vStartColor);
        m_pMtrl->SetData(SHADER_PARAM::VEC4_3, &m_vEndColor);



        Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\particle\\AlphaCircle.png");
        if (nullptr == pTex)
            pTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\particle\\AlphaCircle.png", L"texture\\particle\\AlphaCircle.png");

        m_pMtrl->SetData(SHADER_PARAM::TEX_0, pTex.Get());

        // 리소스 바인딩
        m_pMtrl->UpdateData();

        m_pMesh->UpdateData(0);
        m_pParticleBuffer->UpdateData(100);

        // 렌더
        m_pMesh->render_particle(m_iMaxCount);

        m_pParticleBuffer->Clear();
    }
    else//(22.1.3)
    {
        if (nullptr == m_pParticleTex)
            m_pParticleTex = CResMgr::GetInst()->FindRes<CTexture>(m_pParticleName);
        

        // 렌더링에 필요한 데이터 전달
        if (m_iType==1)
        {
          
          const tAnimFrm& tFrm = GetGameObject()->Animator2D()->GetCurAnim()->GetFrame(0);
          Ptr<CTexture> pAnimTex = GetGameObject()->Animator2D()->GetCurAnim()->GetTex();
          int FrameCount = GetGameObject()->Animator2D()->GetCurAnim()->GetFrameCount();
          m_pMtrl->SetData(SHADER_PARAM::TEX_0, pAnimTex.Get());
          m_pMtrl->SetData(SHADER_PARAM::VEC2_0, &tFrm.vLT);
          m_pMtrl->SetData(SHADER_PARAM::VEC2_1, &tFrm.vStep);
          m_pMtrl->SetData(SHADER_PARAM::INT_2, &FrameCount);
          m_pMtrl->SetData(SHADER_PARAM::FLOAT_3, &tFrm.fDuration);
        }
        else
         m_pMtrl->SetData(SHADER_PARAM::TEX_0, m_pParticleTex.Get());
        
        
        m_pMtrl->SetData(SHADER_PARAM::VEC4_0, &m_vStartScale);
        m_pMtrl->SetData(SHADER_PARAM::VEC4_1, &m_vEndScale);
        m_pMtrl->SetData(SHADER_PARAM::VEC4_2, &m_vStartColor);
        m_pMtrl->SetData(SHADER_PARAM::VEC4_3, &m_vEndColor);

        // 리소스 바인딩
        m_pMtrl->UpdateData();

        m_pMesh->UpdateData(0);
        m_pParticleBuffer->UpdateData(100);

        // 렌더
        m_pMesh->render_particle(m_iMaxCount);

        m_pParticleBuffer->Clear();

    }


}

void CParticleSystem::SetMaxParticleCount(int _iMaxCount)
{
    if (m_iMaxCount < _iMaxCount)
    {
        m_pParticleBuffer->CreateRW(sizeof(tParticle), _iMaxCount, nullptr);
    }

    m_iMaxCount = _iMaxCount;
}


void CParticleSystem::SaveToScene(FILE* _pFile)
{
    CComponent::SaveToScene(_pFile);

    fwrite(&m_iMaxCount, sizeof(int), 1, _pFile);

    fwrite(&m_fMinLifeTime, sizeof(float), 1, _pFile);
    fwrite(&m_fMaxLifeTime, sizeof(float), 1, _pFile);
    fwrite(&m_fMinSpeed, sizeof(float), 1, _pFile);
    fwrite(&m_fMaxSpeed, sizeof(float), 1, _pFile);
    fwrite(&m_fFrequency, sizeof(float), 1, _pFile);
    fwrite(&m_vCreateRange, sizeof(Vec4), 1, _pFile);

    fwrite(&m_vStartColor, sizeof(Vec4), 1, _pFile);
    fwrite(&m_vEndColor, sizeof(Vec4), 1, _pFile);
    fwrite(&m_vStartScale, sizeof(Vec4), 1, _pFile);
    fwrite(&m_vEndScale, sizeof(Vec4), 1, _pFile);
}

void CParticleSystem::LoadFromScene(FILE* _pFile)
{
    CComponent::LoadFromScene(_pFile);

    fread(&m_iMaxCount, sizeof(int), 1, _pFile);
    SetMaxParticleCount(m_iMaxCount);

    fread(&m_fMinLifeTime, sizeof(float), 1, _pFile);
    fread(&m_fMaxLifeTime, sizeof(float), 1, _pFile);
    fread(&m_fMinSpeed, sizeof(float), 1, _pFile);
    fread(&m_fMaxSpeed, sizeof(float), 1, _pFile);
    fread(&m_fFrequency, sizeof(float), 1, _pFile);
    fread(&m_vCreateRange, sizeof(Vec4), 1, _pFile);

    fread(&m_vStartColor, sizeof(Vec4), 1, _pFile);
    fread(&m_vEndColor, sizeof(Vec4), 1, _pFile);
    fread(&m_vStartScale, sizeof(Vec4), 1, _pFile);
    fread(&m_vEndScale, sizeof(Vec4), 1, _pFile);
}
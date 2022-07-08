#pragma once
#include "CEntity.h"

class CStructuredBuffer :
    public CEntity
{
private:
    ComPtr<ID3D11Buffer>                m_pSB;
    ComPtr<ID3D11ShaderResourceView>    m_pSRV;          // 쉐이더 리소스 뷰(리소스 바인딩 용도)
    ComPtr<ID3D11UnorderedAccessView>   m_pUAV;          // RWStructuredbuffer(register u) 바인딩

    UINT                                m_iElementSize;  // 버퍼 요소 사이즈
    UINT                                m_iElementCount; // 버퍼 요소 개수

    int                                 m_iSRVRecentNum;    // 최근 바인딩한 레지스터 번호
    int                                 m_iUAVRecentNum;

public:
    void Create(UINT _iElementSize, UINT _iElementCount, void* _pSysMem);
    void CreateRW(UINT _iElementSize, UINT _iElementCount, void* _pSysMem);
    void UpdateData(UINT _iRegisterNum);
    void UpdateDataRW(UINT _iRegisterNum);

    void Clear();
    void ClearRW();

    void SetData(void* _pData, UINT _iElementSize, UINT _iElementCount);

    int GetElementSize() { return m_iElementSize; }
    int GetElementCount() { return m_iElementCount; }
    int GetBufferSize() { return m_iElementSize * m_iElementCount; }

public:
    CLONE_DISABLE(CStructuredBuffer);
    virtual void SaveToScene(FILE* _pFile) { assert(nullptr); }
    virtual void LoadFromScene(FILE* _pFile) { assert(nullptr); }

public:
    CStructuredBuffer();
    ~CStructuredBuffer();
};


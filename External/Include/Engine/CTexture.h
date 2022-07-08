#pragma once
#include "CRes.h"

#include <DirectXTex\DirectXTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

class CTexture :
    public CRes
{
private:
    ScratchImage                        m_Image;    // SystemMem 에 로드된 데이터 
    ComPtr<ID3D11Texture2D>             m_pTex2D;   // GPU 에 로딩된 텍스쳐 인터페이스

    ComPtr<ID3D11RenderTargetView>      m_pRTV;     // 렌더타켓 용도 설명자 View
    ComPtr<ID3D11DepthStencilView>      m_pDSV;     // 깊이 스텐실 용도 설명자 View
    ComPtr<ID3D11ShaderResourceView>    m_pSRV;     // 쉐이더 리소스 뷰(리소스 바인딩 용도)
    ComPtr<ID3D11UnorderedAccessView>   m_pUAV;     // RWTexture(register u) 바인딩



    D3D11_TEXTURE2D_DESC                m_tDesc;    // GPU Texture2D 정보

public:
    int Create(UINT _iWidth, UINT _iHeight, UINT _eFlag , DXGI_FORMAT _eFormat);
    int Create(ComPtr<ID3D11Texture2D> _pTex2D);
    int CreateArrayTexture(const vector<Ptr<CTexture>>& _vecTex);

    virtual void Save(const wstring& _strRelativePath);
    virtual void Load(const wstring& _strFilePath);

    void UpdateData(UINT _iRegisterNum, UINT _iPipelineStage);
    void UpdateDataRW(UINT _iRegisterNum);

    static void Clear(UINT _iRegisterNum, UINT _iPipelineStage);
    static void ClearRW(UINT _iRegisterNum);

    UINT Width() {return m_tDesc.Width; }
    UINT Height() { return m_tDesc.Height; }

    const D3D11_TEXTURE2D_DESC& GetDesc() const { return m_tDesc; }
    void* GetSysMem() const { return m_Image.GetPixels(); }
    UINT GetRowPitch() const { return (UINT)m_Image.GetImages()->rowPitch; }
    UINT GetSlicePitch()const { return (UINT)m_Image.GetImages()->slicePitch; }

    ComPtr<ID3D11Texture2D> GetTex2D() { return m_pTex2D; }
    ComPtr<ID3D11RenderTargetView> GetRTV() { return m_pRTV; }
    ComPtr<ID3D11DepthStencilView> GetDSV() { return m_pDSV; }
    ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_pSRV; }
    ComPtr<ID3D11UnorderedAccessView> GetUAV() { return m_pUAV; }


public:
    CTexture();
    ~CTexture();
};


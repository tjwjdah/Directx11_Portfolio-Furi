#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer()
	: m_iElementSize(0)
	, m_iElementCount(0)
	, m_iSRVRecentNum(-1)
	, m_iUAVRecentNum(-1)
{

}

CStructuredBuffer::~CStructuredBuffer()
{

}

void CStructuredBuffer::Create(UINT _iElementSize, UINT _iElementCount, void* _pSysMem)
{
	if (nullptr != m_pSB)
	{
		SAFE_RELEASE(m_pSB);
		SAFE_RELEASE(m_pSRV);
		SAFE_RELEASE(m_pUAV);		

		m_iElementSize = 0;
		m_iElementCount = 0;

		m_iSRVRecentNum = -1;
		m_iUAVRecentNum = -1;
	}


	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;

	// GPU 메모리에 Structured Buffer 만들기
	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = m_iElementSize * m_iElementCount; // GPU 에 생성할 구조화 버퍼 메모리 크기(최소단위 ??)
	tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	tBufferDesc.StructureByteStride = m_iElementSize;

	HRESULT hr = S_OK;
	if (nullptr != _pSysMem)
	{
		D3D11_SUBRESOURCE_DATA tSub = {};
		tSub.pSysMem = _pSysMem;
		hr = DEVICE->CreateBuffer(&tBufferDesc, &tSub, m_pSB.GetAddressOf());
	}
	else
	{
		hr = DEVICE->CreateBuffer(&tBufferDesc, nullptr, m_pSB.GetAddressOf());
	}

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Structured Buffer Create Failed!!", L"구조화 버퍼 생성 실패", MB_OK);
		assert(nullptr);
	}

	// Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

	tSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	tSRVDesc.BufferEx.FirstElement = 0;
	tSRVDesc.BufferEx.Flags = 0;
	tSRVDesc.BufferEx.NumElements = m_iElementCount;
	hr = DEVICE->CreateShaderResourceView(m_pSB.Get(), &tSRVDesc, m_pSRV.GetAddressOf());
}

void CStructuredBuffer::CreateRW(UINT _iElementSize, UINT _iElementCount, void* _pSysMem)
{
	if (nullptr != m_pSB)
	{
		m_pSB->Release();
		m_pSRV->Release();
		m_pUAV->Release();

		m_iElementSize = 0;
		m_iElementCount = 0;

		m_iSRVRecentNum = -1;
		m_iUAVRecentNum = -1;
	}


	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;

	// GPU 메모리에 Structured Buffer 만들기
	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = m_iElementSize * m_iElementCount; // GPU 에 생성할 구조화 버퍼 메모리 크기(최소단위 ??)
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.CPUAccessFlags = 0;	
	tBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;	
	tBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	tBufferDesc.StructureByteStride = m_iElementSize;

	HRESULT hr = S_OK;
	if (nullptr != _pSysMem)
	{
		D3D11_SUBRESOURCE_DATA tSub = {};
		tSub.pSysMem = _pSysMem;
		hr = DEVICE->CreateBuffer(&tBufferDesc, &tSub, m_pSB.GetAddressOf());
	}
	else
	{
		hr = DEVICE->CreateBuffer(&tBufferDesc, nullptr, m_pSB.GetAddressOf());
	}
	
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Structured Buffer Create Failed!!", L"구조화 버퍼 생성 실패", MB_OK);
		assert(nullptr);
	}

	// Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

	tSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	tSRVDesc.BufferEx.FirstElement = 0;
	tSRVDesc.BufferEx.Flags = 0;
	tSRVDesc.BufferEx.NumElements = m_iElementCount;
	hr = DEVICE->CreateShaderResourceView(m_pSB.Get(), &tSRVDesc, m_pSRV.GetAddressOf());

	// Unordered Access View
	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};

	tUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	tUAVDesc.Buffer.FirstElement = 0;
	tUAVDesc.Buffer.Flags = 0;
	tUAVDesc.Buffer.NumElements = m_iElementCount;
	hr = DEVICE->CreateUnorderedAccessView(m_pSB.Get(), &tUAVDesc, m_pUAV.GetAddressOf());
}

void CStructuredBuffer::UpdateData(UINT _iRegisterNum)
{
	if (-1 != m_iSRVRecentNum)
	{
		Clear();
	}

	m_iSRVRecentNum = _iRegisterNum;

	CONTEXT->VSSetShaderResources(_iRegisterNum, 1, m_pSRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(_iRegisterNum, 1, m_pSRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(_iRegisterNum, 1, m_pSRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(_iRegisterNum, 1, m_pSRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(_iRegisterNum, 1, m_pSRV.GetAddressOf());
	CONTEXT->CSSetShaderResources(_iRegisterNum, 1, m_pSRV.GetAddressOf());
}

void CStructuredBuffer::UpdateDataRW(UINT _iRegisterNum)
{
	if (-1 != m_iUAVRecentNum)
		ClearRW();

	m_iUAVRecentNum = _iRegisterNum;

	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, m_pUAV.GetAddressOf(), &i);
}

void CStructuredBuffer::Clear()
{
	if (-1 == m_iSRVRecentNum)
		return;

	ComPtr<ID3D11ShaderResourceView> pSRV = nullptr;
	CONTEXT->VSSetShaderResources(m_iSRVRecentNum, 1, pSRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(m_iSRVRecentNum, 1, pSRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(m_iSRVRecentNum, 1, pSRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(m_iSRVRecentNum, 1, pSRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(m_iSRVRecentNum, 1, pSRV.GetAddressOf());
	CONTEXT->CSSetShaderResources(m_iSRVRecentNum, 1, pSRV.GetAddressOf());
	m_iSRVRecentNum = -1;
}

void CStructuredBuffer::ClearRW()
{
	if (-1 == m_iUAVRecentNum)
		return;

	UINT i = -1;
	ComPtr<ID3D11UnorderedAccessView> pUAV = nullptr;
	CONTEXT->CSSetUnorderedAccessViews(m_iUAVRecentNum, 1, pUAV.GetAddressOf(), &i);

	m_iUAVRecentNum = -1;
}

void CStructuredBuffer::SetData(void* _pData, UINT _iElementSize, UINT _iElementCount)
{
	D3D11_MAPPED_SUBRESOURCE tSub = {};	
	CONTEXT->Map(m_pSB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, _pData, (size_t)(_iElementSize * _iElementCount));	
	CONTEXT->Unmap(m_pSB.Get(), 0);		
}

#include "pch.h"
#include "CConstBuffer.h"

#include "CDevice.h"

CConstBuffer::CConstBuffer()
	: m_eCBType(CB_TYPE::END)
	, m_iBufferSize(0)
{
}

CConstBuffer::~CConstBuffer()
{
}

void CConstBuffer::Create(CB_TYPE _eType, UINT _iBufferSize)
{
	m_eCBType = _eType;
	m_iBufferSize = _iBufferSize;

	// GPU 메모리에 Constant Buffer 만들기
	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = m_iBufferSize;					// GPU 에 생성할 상수버퍼 메모리 크기(최소단위 16)
	tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// 수정 가능 버퍼
	tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// 수정 가능 버퍼
	tBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// Constant Buffer 용도	

	HRESULT hr = DEVICE->CreateBuffer(&tBufferDesc, nullptr, m_pCB.GetAddressOf());
}

void CConstBuffer::SetData(void* _pData) const
{
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_pCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, _pData, m_iBufferSize);
	CONTEXT->Unmap(m_pCB.Get(), 0);
}

void CConstBuffer::UpdateData(UINT _iPOV) const
{
	UINT iRegister = (UINT)m_eCBType;

	if((UINT)PIPELINE_STAGE::PS_VERTEX & _iPOV)
		CONTEXT->VSSetConstantBuffers(iRegister, 1, m_pCB.GetAddressOf());
	if ((UINT)PIPELINE_STAGE::PS_HULL & _iPOV)
		CONTEXT->HSSetConstantBuffers(iRegister, 1, m_pCB.GetAddressOf());
	if ((UINT)PIPELINE_STAGE::PS_DOMAIN & _iPOV)
		CONTEXT->DSSetConstantBuffers(iRegister, 1, m_pCB.GetAddressOf());
	if ((UINT)PIPELINE_STAGE::PS_GEOMETRY & _iPOV)
		CONTEXT->GSSetConstantBuffers(iRegister, 1, m_pCB.GetAddressOf());
	if ((UINT)PIPELINE_STAGE::PS_PIXEL & _iPOV)
		CONTEXT->PSSetConstantBuffers(iRegister, 1, m_pCB.GetAddressOf());
	if ((UINT)PIPELINE_STAGE::PS_COMPUTE & _iPOV)
		CONTEXT->CSSetConstantBuffers(iRegister, 1, m_pCB.GetAddressOf());
}
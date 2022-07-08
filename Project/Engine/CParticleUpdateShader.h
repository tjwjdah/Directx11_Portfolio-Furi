#pragma once

#include "CComputeShader.h"

class CStructuredBuffer;

class CParticleUpdateShader
	: public CComputeShader
{
private:
	tMtrlInfo         m_tInfo;
	CStructuredBuffer* m_pParticleBuffer;
	CStructuredBuffer* m_pSharedBuffer;
	int               m_iAliveCount;
	int   m_TestIdx;//test(22.1.3)

public:
	void SetTestidx(int _inx) { m_TestIdx = _inx; } //test(22.1.3)
	void SetParticleBuffer(CStructuredBuffer* _pBuffer) { m_pParticleBuffer = _pBuffer; }
	void SetAliveCount(int _iAliveCount) { m_iAliveCount = _iAliveCount; }
	void SetMaxParticle(int _iMaxCount) { m_tInfo.arrInt[0] = _iMaxCount; }
	void SetMinLifeTime(float _fMinTime) { m_tInfo.arrFloat[0] = _fMinTime; }
	void SetMaxLifeTime(float _fMaxTime) { m_tInfo.arrFloat[1] = _fMaxTime; }
	void SetParticleWorldPos(const Vec3& _vWorldPos) { m_tInfo.arrVec4[0] = _vWorldPos; }
	void SetParticelCreateRange(const Vec4& _vRange) { m_tInfo.arrVec4[1] = _vRange; }
	void SetDirection(int _iIdx) { m_tInfo.arrInt[3] = _iIdx; }
	void SetMinSpeed(float _fMinSpeed) { m_tInfo.arrFloat[2] = _fMinSpeed; }
	void SetMaxSpeed(float _fMaxSpeed) { m_tInfo.arrFloat[3] = _fMaxSpeed; }
	void SetType(int _Type) { m_tInfo.arrInt[1] = _Type; }
public:
	virtual void UpdateData();
	virtual void Clear();

	virtual void Excute();

public:
	CParticleUpdateShader();
	~CParticleUpdateShader();
};

#include "pch.h"
#include "CTimeMgr.h"

#include "CCore.h"
#include "CResMgr.h"

CTimeMgr::CTimeMgr()
	: m_llFrequency{}
	, m_llOldCount{}
	, m_llCurCount{}
{
}

CTimeMgr::~CTimeMgr()
{
}

void CTimeMgr::init()
{
	QueryPerformanceFrequency(&m_llFrequency);
	QueryPerformanceCounter(&m_llOldCount);
	QueryPerformanceCounter(&m_llPrevCount);
}

void CTimeMgr::update()
{
	QueryPerformanceCounter(&m_llCurCount);

	m_dDeltaTime = (double)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (double)m_llFrequency.QuadPart;
	m_dFPS = 1. / m_dDeltaTime;

	m_llPrevCount = m_llCurCount;

	// 1초마다 Frequency 갱신
	if (m_llCurCount.QuadPart - m_llOldCount.QuadPart > m_llFrequency.QuadPart)
	{
		QueryPerformanceFrequency(&m_llFrequency);
		m_llOldCount = m_llCurCount;
	}


	m_dAccTime += m_dDeltaTime;
	if (60. < m_dAccTime)
	{
		CResMgr::GetInst()->ClearCloneRes();
		m_dAccTime = 0.;
	}

	g_global.fDeltaTime = (float)m_dDeltaTime;
	g_global.fAcctime += g_global.fDeltaTime;
}

void CTimeMgr::render()
{
	static int iCallCount = 0;
	++iCallCount;

	static double dTime = 0.;
	dTime += m_dDeltaTime;

	if (1. < dTime)
	{
		wchar_t szDeltaTime[255] = {};
		swprintf(szDeltaTime, 255, L"FPS : %d, DT : %lf ", iCallCount, m_dDeltaTime);
		SetWindowText(CCore::GetInst()->GetWndHwnd(), szDeltaTime);
		dTime = 0.;
		iCallCount = 0;
	}	
}
#pragma once

#include "CSingleton.h"

class CTimeMgr
	: public CSingleton<CTimeMgr>
{
	SINGLE(CTimeMgr);
private:
	LARGE_INTEGER	m_llFrequency;
	LARGE_INTEGER	m_llOldCount;
	LARGE_INTEGER	m_llCurCount;
	LARGE_INTEGER	m_llPrevCount;

	double			m_dDeltaTime; // 1 프레임 간격
	double			m_dFPS;		  // 초당 프레임 회수

	double			m_dAccTime;	  // 리소스매니저용

public:
	double GetDT() { return m_dDeltaTime; }
	float GetfDT() { return (float)m_dDeltaTime; }

public:
	void init();
	void update();
	void render();
};


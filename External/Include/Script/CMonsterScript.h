#pragma once

#include <Engine\CScript.h>

class CMonsterScript
	: public CScript
{
	bool m_bMouseOn;
	Vec2 m_Res;


public:
	bool IsMouseOn() { return m_bMouseOn; }


public:
	virtual	void awake();
	virtual void update();

	//virtual void OnCollisionEnter(CGameObject* _pOther);
	CLONE(CMonsterScript);

public:
	CMonsterScript();
	~CMonsterScript();
};
#pragma once
#include <Engine\CScript.h>

enum class ObjList {
	Start,
	Law,
	Change,
	Wise,
	Hpbar
};
class CPlayerScript;
class CHpbarScript;
class CSecneChangeScript :
    public CScript
{
private:
	int m_iTest;
	bool m_bTest;
	vector<CGameObject*> m_vStartObj;
	vector<CGameObject*> m_vLawObj;
	vector<CGameObject*> m_vChangeObj;
	vector<CGameObject*> m_vWiseObj;
	vector<CGameObject*> m_vHpbarObj;
	

	CPlayerScript*			m_Player;
	CHpbarScript*			m_HpBar;
	CGameObject*			m_Wise;
	CGameObject*			m_Law;
private:
	virtual void update();

public:
	void ChangeObjPushObj(CGameObject* _Obj, ObjList _List);
	void ChangeObjList(ObjList _List);
	void VObjUpRenOnOff(ObjList _List,bool _OnOff);

	void SetPlayer(CGameObject* Obj);
	void SetHpbar(CGameObject* Obj);
	void SetLaw(CGameObject* Obj) { m_Law = Obj; };
	void SetWise(CGameObject* Obj) { m_Wise = Obj; };
public:
	CLONE(CSecneChangeScript);

public:
	CSecneChangeScript();
	~CSecneChangeScript();
};


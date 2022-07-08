#pragma once

#include "CInterface.h"

class CEntity
	: public CInterface
{
private:
	static UINT	g_iGlobalID;

private:
	wstring		m_strName;
	UINT		m_iID;

public:
	void SetName(const wstring& _strName){m_strName = _strName;}
	const wstring& GetName() { return m_strName; }
	UINT GetID() { return m_iID; }

protected:
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	CEntity& operator = (const CEntity& _other) = delete;	

public:
	virtual CEntity* Clone() = 0;

public:
	CEntity();
	CEntity(const CEntity& _origin);
	virtual ~CEntity();
};


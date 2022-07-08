#pragma once

#include "imgui.h"

class GUI
{
private:
	wstring m_strName;
	bool	m_bActive;


public:
	void SetName(const wstring& _strName){m_strName = _strName;}
	const wstring& GetName() { return m_strName; }

	void Activate() { m_bActive = true; }
	void DeActivate() {m_bActive = false;}
	bool IsActive() { return m_bActive; }

public:
	virtual void init() {};
	virtual void update() {};
	virtual void lateupdate() {};
	virtual void render() = 0;

public:
	GUI();
	virtual ~GUI();
};


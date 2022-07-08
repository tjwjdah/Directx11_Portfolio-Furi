#pragma once

#include <Engine\CSingleton.h>

#include "GUI.h"

typedef void (GUI::* EVENT_FUNC_PARAM_1)(const void* _data);
typedef void (GUI::* EVENT_FUNC_PARAM_2)(const void* _data1, const void* _data2);
typedef void (GUI::* EVENT_FUNC_PARAM_3)(const void* _data1, const void* _data2, const void* _data3);
struct tGUIEvent
{
	GUI* pInst;

	virtual void Excute() = 0;

	virtual ~tGUIEvent()
	{}
};

struct tGUIEvent1
	: public tGUIEvent
{
	EVENT_FUNC_PARAM_1  pFunc;
	void* pData;

	virtual void Excute()
	{
		(*pInst.*pFunc)(pData);
	}
};

struct tGUIEvent2
	: public tGUIEvent
{
	EVENT_FUNC_PARAM_2	pFunc;
	void*				pData[2];

	virtual void Excute()
	{
		(*pInst.*pFunc)(pData[0], pData[1]);
	}
};
struct tGUIEvent3
	: public tGUIEvent
{
	EVENT_FUNC_PARAM_3	pFunc;
	void* pData[3];

	virtual void Excute()
	{
		(*pInst.*pFunc)(pData[0], pData[1], pData[2]);
	}
};

class CImGUIMgr
	: public CSingleton<CImGUIMgr>
{
	SINGLE(CImGUIMgr);
private:
	map<wstring, GUI*>	m_mapGUI;

	vector<tGUIEvent*> m_vecEvent;

public:
	void init();
	void progress();

	GUI* FindGUI(const wstring& _strName);
	void AddGUI(const wstring& _strName, GUI* _pGUI);

private:
	void CreateGUI();

public:
	template<typename T>
	void AddCallBackEvent(const T& _event);
};

template<typename T>
inline void CImGUIMgr::AddCallBackEvent(const T& _event)
{
	m_vecEvent.push_back(new T(_event));
}

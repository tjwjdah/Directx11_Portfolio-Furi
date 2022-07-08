#pragma once
#include "CSingleton.h"

#include <queue>
using std::priority_queue;

enum class EVENT_TYPE
{
	CREATE_OBJECT,
	DELETE_OBJECT,
	
	ADD_CHILD,
	DISCONNECT_PARENT,

	SCENE_CHANGE,

	END,
};

struct tEvent
{
	EVENT_TYPE	eEvent;
	DWORD_PTR	wParam;
	DWORD_PTR	lParam;
	UINT	    m_ChildNum; //자식 연달아 상속시킬때 순서
};


class Compare
{
public:
	bool operator() (const tEvent& _left, const tEvent& _right)
	{
		if ((UINT)_left.eEvent == (UINT)_right.eEvent)
		{
			return _left.m_ChildNum > _right.m_ChildNum;  //컴페어로 비교해서 넣어지다보니까 순서꼬이면 자식 레이어가 -1로 들어감 그럴때를 위해서 순서 제대로 하기 위해만듬
		}
		else
		{
			return (UINT)_left.eEvent > (UINT)_right.eEvent;
		}
		
	}
};


class CGameObject;

class CEventMgr :
    public CSingleton<CEventMgr>
{
    SINGLE(CEventMgr);
private:
	priority_queue<tEvent, vector<tEvent>, Compare>	m_event;
	vector<CGameObject*>							m_vecDeadObj;

	bool											m_bEvent; // HierachyGUI 가 알림받을 용도

public:
	void update();
	void AddEvent(const tEvent& _event) { m_event.push(_event); }

	bool HasEvnOcrd() { return m_bEvent; }

private:
	void ExcuteEvent(const tEvent& _event);
};


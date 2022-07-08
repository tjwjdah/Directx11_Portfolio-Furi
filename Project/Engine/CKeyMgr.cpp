#include "pch.h"
#include "CKeyMgr.h"

#include "CCore.h"

UINT g_Key[(UINT)KEY_TYPE::END] =
{
	L'W', // KEY_W, // = 0
	L'S', // KEY_S, // = 1
	L'A', // KEY_A, // = 2
	L'D', // KEY_D, // = 3
	L'Z',
	L'F',
	VK_SPACE,
	VK_RETURN,
	VK_LSHIFT,
	VK_LBUTTON,
	VK_RBUTTON,
	VK_MBUTTON,
	VK_UP,		// KEY_UP,
	VK_DOWN,	// KEY_DOWN,
	VK_LEFT,	// KEY_LEFT,
	VK_RIGHT,	// KEY_RIGHT,			

	VK_NUMPAD0,
	VK_NUMPAD1,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
};


CKeyMgr::CKeyMgr()
	: m_vMousePos{}
	, m_vPrevMousePos{}
{

}

CKeyMgr::~CKeyMgr()
{
	
}

void CKeyMgr::init()
{
	for (UINT i = 0; i < (UINT)KEY_TYPE::END; ++i)
	{
		m_vecKey.push_back(tKeyData{KEY_STATE::NONE, false});
	}
}

void CKeyMgr::update()
{
	HWND hWnd = GetFocus();
	
	if (nullptr == hWnd)
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{			
			if (KEY_STATE::TAP == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::HOLD == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEY_STATE::NONE;
			}						
			m_vecKey[i].bPress = false;
		}
	}
	else
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{
			// 키가 눌려있다
			if (GetAsyncKeyState(g_Key[i]) & 0x8000)
			{
				// 이전에도 눌렸다
				if (m_vecKey[i].bPress)
				{
					// HOLD
					m_vecKey[i].eState = KEY_STATE::HOLD;
				}
				// 이전에는 눌리지 않았다.
				else
				{
					// TAP
					m_vecKey[i].eState = KEY_STATE::TAP;
				}

				m_vecKey[i].bPress = true;
			}
			// 키가 안눌렸다.
			else
			{
				// 이전에는 눌려있었다.
				if (m_vecKey[i].bPress)
				{
					// Away
					m_vecKey[i].eState = KEY_STATE::AWAY;
				}
				else
				{
					// NONE
					m_vecKey[i].eState = KEY_STATE::NONE;
				}

				m_vecKey[i].bPress = false;
			}
		}
	}	

	// 마우스 포지션
	m_vPrevMousePos = m_vMousePos;
	POINT ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(CCore::GetInst()->GetWndHwnd(), &ptMouse);
	m_vMousePos = Vec2((float)ptMouse.x, (float)ptMouse.y);

	// 현재 마우스 좌표와 이전 마우스좌표의 차이값
	m_vMouseDir.x = m_vMousePos.x - m_vPrevMousePos.x;
	m_vMouseDir.y = m_vPrevMousePos.y - m_vMousePos.y;
}

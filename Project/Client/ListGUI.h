#pragma once
#include "GUI.h"

class ListGUI;

//typedef void (GUI::* LIST_SELECT_ITEM)(const ListGUI* _data1, const char* _data2);
typedef void (GUI::* LIST_SELECT_ITEM)(const ListGUI* _data1, const char* _data2, const char* _data3);
class ListGUI :
    public GUI
{
private:
    string            m_strCaption;
    vector<string>    m_vecItem;
    string            m_strSel;
    int               m_iSelIdx;
    bool              m_bOpen;

    // 항목 선택 시, 호출할 함수와 객체
    LIST_SELECT_ITEM  m_pSelectFunc;
    GUI* m_pInst;
    int               m_Setnum;

public:
    virtual void render();

public:
    void SetCaption(const string& _str) { m_strCaption = _str; }
    const string& GetCaption() { return m_strCaption; }
    void AddItem(string _strItem) { m_vecItem.push_back(_strItem); }
    void Clear() { m_vecItem.clear(); m_iSelIdx = -1; }

    bool IsCancle() { return !m_bOpen; }
    const string& GetSelect() { return m_strSel; };

    void SetSelectCallBack(GUI* _Inst, LIST_SELECT_ITEM _pFunc, int _SetNum = 0)
    {
        m_pInst = _Inst;
        m_pSelectFunc = _pFunc;
        m_Setnum = _SetNum;
    }


public:
    ListGUI();
    ~ListGUI();
};


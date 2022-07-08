#pragma once

class TreeCtrl;

struct tData
{
	string				strLabel;
	DWORD_PTR			dwData;
};

struct tTreeItem
{
private:
	TreeCtrl*			m_pOwner;
	tTreeItem*			pParent;
	vector<tTreeItem*>	vecChild;
	tData				data;

	bool				m_bSelected;

public:
	const string& GetLabel() const { return data.strLabel; }
	DWORD_PTR GetData() { return data.dwData; }

	const tTreeItem* GetParent() const { return pParent; }

private:
	void AddChild(tTreeItem* _pChildItem)
	{
		vecChild.push_back(_pChildItem);
		_pChildItem->pParent = this;
	}

	const vector<tTreeItem*>& GetChildItem(){return vecChild;}

	void render();

private:
	tTreeItem();
	~tTreeItem();

	friend class TreeCtrl;
};



enum class DRAG_DROP_MOD
{
	SWAP,
	COPY,
	NONE,
};

// 특정 클래스의 맴버변수 포인터 선언 시, 해당 클래스의 정확한 정보가 필요(헤더)
#include "GUI.h"
typedef void (GUI::*DRAG_DROP_CALLBACK)(const void* _drag, const void* _dropped);
typedef void (GUI::* SEL_CHANGE_CALLBACK)(const void* _SelChangedData);


class TreeCtrl
{
private:
	tTreeItem*			m_pRoot;	
	tTreeItem*			m_pBeginItem;
	tTreeItem*			m_pDroppedItem;
	DRAG_DROP_MOD		m_eMod;

	tTreeItem*			m_pSelectedItem;

	GUI*				m_pCallBackInst;
	DRAG_DROP_CALLBACK	m_pCallBack;

	GUI*				m_pSelChangeInst;
	SEL_CHANGE_CALLBACK m_pSelChangeFunc;
	bool				m_bUseDummyRoot;
	

public:
	tTreeItem* AddItem(tTreeItem* _pParent, const string& _strLabel, DWORD_PTR _dwData);
	tTreeItem* UseDummyRoot(const string& _strDummyLabel);
	void Clear();
	void render();

private:
	void SetBeginItem(tTreeItem* _pBegin) { m_pBeginItem = _pBegin; }
	void SetDroppredItem(tTreeItem* _pDropped) { m_pDroppedItem = _pDropped; }
	void SetSelectedItem(tTreeItem* _pSelected);

public:
	void SetDragDropCallBack(DRAG_DROP_CALLBACK _pFunc, GUI* _pInstance) 
	{
		m_pCallBack = _pFunc; 
		m_pCallBackInst = _pInstance;
	}

	void SetSelChangeCallBack(SEL_CHANGE_CALLBACK _pFunc, GUI* _pInstance)
	{
		m_pSelChangeFunc = _pFunc;
		m_pSelChangeInst = _pInstance;
	}


	const tTreeItem* GetSelectedItem() { return m_pSelectedItem; }


public:
	TreeCtrl();
	~TreeCtrl();

	friend struct tTreeItem;
};


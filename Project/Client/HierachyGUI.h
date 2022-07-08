#pragma once
#include "GUI.h"
#include "TreeCtrl.h"

class CGameObject;

class HierachyGUI :
    public GUI
{
private:
    TreeCtrl    m_Tree;    

public:
    void Reset();
    virtual void update();
    virtual void render();

private:
    void AddGameObject(tTreeItem* _pParentItem, CGameObject* _pObject);

public:
    void DragDropProgress(const tData* _drag, const tData* _dropped);
    void ItemSelectChanged(const tData* _selectData);

public:
    HierachyGUI();
    ~HierachyGUI();
};


#pragma once
#include "GUI.h"
#include "TreeCtrl.h"

class CRes;

class ResGUI :
    public GUI
{
private:
    TreeCtrl    m_Tree;

public:
    void Reset();
    virtual void render();

public:
    void SelectResource(const void* _SelChangedData);

public:
    ResGUI();
    ~ResGUI();
};


#pragma once
#include "GUI.h"


class DebugGUI :
    public GUI
{
private:


public:
    virtual void update();
    virtual void render();

public:
    DebugGUI();
    virtual ~DebugGUI();
};


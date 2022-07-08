#pragma once
#include "GUI.h"
class CCameraAnimaitorScript;

class CCameraAnimGUI :
    public GUI
{
private:
    CCameraAnimaitorScript* m_Camera;
public:
    virtual void init();
    virtual void update();
    virtual void render();

private:
  
public:
    CCameraAnimGUI();
    ~CCameraAnimGUI();
};


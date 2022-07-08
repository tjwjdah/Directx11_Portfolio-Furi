#pragma once
#include "GUI.h"

class CScene;

class MenuGUI :
    public GUI
{
public:
    virtual void update();
    virtual void render();

public:
    void ChangeSceneState(CScene* _pScene, SCENE_STATE _eState);

public:
    MenuGUI();
    ~MenuGUI();
};


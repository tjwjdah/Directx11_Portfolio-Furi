#pragma once
#include "GUI.h"

#include <Engine\Ptr.h>
#include <Engine\CTexture.h>

class CScene;


class BtnGUI :
    public GUI
{
private:
    CTexture*   m_arrIcon[(UINT)SCENE_STATE::END];

public:
    virtual void init();
    virtual void update();
    virtual void render();

private:
    void ChangeSceneState(CScene* _pScene, SCENE_STATE _eState);

public:
    BtnGUI();
    virtual ~BtnGUI();
};


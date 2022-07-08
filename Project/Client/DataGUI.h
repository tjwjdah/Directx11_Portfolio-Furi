#pragma once

class GUI;

#include "ListGUI.h"
#include <Engine\CScript.h>

class DataGUI
{
public:
	static void render_int(const string& _str, void* _pData);	
	static void render_float(const string& _str, void* _pData);
	static void render_Vec2(const string& _str, void* _pData);
	static void render_Vec4(const string& _str, void* _pData);
	static void render_Prefab(const string& _str, void* _pData);
	static bool render_Texture(const string& _str, void* _pData, GUI* _pInst = nullptr, LIST_SELECT_ITEM _pFunc = nullptr);
	static void render_MemFunc(const string& _str, void* _pData, CScript::SCRIPT_MEMFUNC _pMemFunc);


	static void clear();
};




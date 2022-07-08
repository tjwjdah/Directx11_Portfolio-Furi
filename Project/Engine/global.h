#pragma once


#include <Windows.h>


#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")


// Fbx Loader
#include <FBXLoader/fbxsdk.h>

#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif



#include "SimpleMath.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;


// std
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <typeinfo>
#include <random>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
using std::experimental::filesystem::path;



using std::vector;
using std::list;
using std::map;
using std::make_pair;
using std::string;
using std::wstring;


#include "define.h"
#include "struct.h"
#include "extern.h"
#include "Ptr.h"
#include "func.h"

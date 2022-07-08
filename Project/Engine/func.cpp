#include "pch.h"
#include "func.h"


void SaveWString(const wstring& _str, FILE* _pFile)
{
	BYTE b = (BYTE)_str.length();
	fwrite(&b, 1, 1, _pFile);
	fwrite(_str.c_str(), sizeof(wchar_t), (size_t)b, _pFile);
}
void SaveStringg(const string& _str, FILE* _pFile)
{
	BYTE b = (BYTE)_str.length();
	fwrite(&b, 1, 1, _pFile);
	fwrite(_str.c_str(), sizeof(char), (size_t)b, _pFile);
}

void LoadWString(wstring& _str, FILE* _pFile)
{
	BYTE b = 0;
	fread(&b, 1, 1, _pFile);

	wchar_t szBuffer[255] = {};
	fread(szBuffer, sizeof(wchar_t), (size_t)b, _pFile);
	_str = szBuffer;
}
void LoadStringg(string& _str, FILE* _pFile)
{
	BYTE b = 0;
	fread(&b, 1, 1, _pFile);

	char szBuffer[255] = {};
	fread(szBuffer, sizeof(char), (size_t)b, _pFile);
	_str = szBuffer;
}

char g_szChar[256] = {};
char* GetString(wchar_t* _wchar)
{
	wstring wstr = _wchar;
	string str(wstr.begin(), wstr.end());
	strcpy_s(g_szChar, 256, str.c_str());
	return g_szChar;
}

char* GetString(const wstring& _wstr)
{	
	string str(_wstr.begin(), _wstr.end());
	strcpy_s(g_szChar, 256, str.c_str());
	return g_szChar;
}

int GetSizeofFormat(DXGI_FORMAT _eFormat)
{
	int iRetByte = 0;
	switch (_eFormat)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		iRetByte = 128;
		break;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		iRetByte = 96;
		break;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		iRetByte = 64;
		break;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		iRetByte = 16;
		break;
	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		iRetByte = 8;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		iRetByte = 128;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		iRetByte = 64;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		iRetByte = 32;
		break;
		// These are compressed, but bit-size information is unclear.
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_UNKNOWN:
	default:
		return -1;
	}

	return iRetByte / 8;
}

Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat)
{
	Matrix mat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat.m[i][j] = (float)_mat.Get(i, j);
		}
	}
	return mat;
}
bool ColliderCheck(Vec3 _mPos, Vec3 _TarPos, Vec3 _ColDir, float _Angle)
{
	Vec3 TargetDir=   _TarPos- _mPos;
	TargetDir.Normalize();
	float angle = _ColDir.Dot(TargetDir);
	if(angle<0.f)
	angle =6.28f- acosf(angle);
	else
	angle = acosf(angle);
	

	if (_Angle > angle)
		return true;
	else 
		return false;
	/*
	Vec2 m_TargetVec;
	m_TargetVec.x = _TarPos.x - _mPos.x;
	m_TargetVec.y = _TarPos.z - _mPos.z;

	
	m_TargetVec.Normalize();
	float m_TarAngle = atan2f(m_TargetVec.x, m_TargetVec.y);
	if (m_TarAngle < 0)
		m_TarAngle = 2 * 3.14 + m_TarAngle;

	float StartAngle;
	float EndAngle;
	if (_StartAngle < 0)
		StartAngle = 2 * 3.14 + _StartAngle;
	if (_EndAngle >6.28 )
		EndAngle = 6.28f- _EndAngle;

	if (_StartAngle > _EndAngle)
	{
		if (m_TarAngle > StartAngle && m_TarAngle<6.28f || m_TarAngle > 0.f && m_TarAngle < EndAngle)
			return true;
		else
			return false;
	}
	else
	{
		if (m_TarAngle > StartAngle && m_TarAngle < EndAngle)
			return true;
		else
			return false;
	}
	*/

}

bool ColliderDistanceCheck(Vec3 _mPos, Vec3 _TarPos, Vec3 _CScale, float _CollDistance)
{
	//_mPos = 콜리전 포즈
	//_TarPos =콜리전에 들어온 포즈
	//_CScale= 콜리전 스케일
	//_CollDistance= 거리 체크할 비율 0~1사이 0.5를 넣으면 콜리전 원점에서 반까지는 충돌 체크가 안되고 그이상부터 체크가 됌 
	float CDistance = Vec3::Distance(_TarPos, _mPos);
	float Scale;
	if (_CScale.x > _CScale.z)
		Scale = _CScale.x/2.f;
	else
		Scale = _CScale.z/2.f;
	if (CDistance / Scale > _CollDistance)
		return true;
	else
		return false;
}
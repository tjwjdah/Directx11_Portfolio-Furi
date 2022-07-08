#include "pch.h"
#include "CPathMgr.h"

wchar_t CPathMgr::g_szResPath[255] = {};

void CPathMgr::init()
{
	GetCurrentDirectory(255, g_szResPath);
	size_t iLen = wcslen(g_szResPath);

	for (size_t i = iLen - 1; i >= 0; --i)
	{
		if (L'\\' == g_szResPath[i])
		{
			g_szResPath[i] = 0;
			break;
		}		
	}

	wcscat_s(g_szResPath, L"\\bin\\content\\");
}

wstring CPathMgr::GetRelativePath(const wstring& _strPath)
{
	if (-1 == _strPath.find(g_szResPath))
		return nullptr;

	wstring strRelativePath = _strPath.substr(wcslen(g_szResPath), _strPath.length());

	return strRelativePath;
}

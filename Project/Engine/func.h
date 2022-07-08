#pragma once

int Add(int a, int b);

template<typename T1, typename T2>
void Safe_Delete_Map(map<T1, T2>& _map)
{
	typename map<T1, T2>::iterator iter = _map.begin();

	for (; iter != _map.end(); ++iter)
	{
		if (nullptr != iter->second)
			delete iter->second;
	}
	_map.clear();
}


template<typename T>
void Safe_Delete_Vector(vector<T>& _vec)
{
	for (size_t i = 0; i < _vec.size(); ++i)
	{
		if (nullptr != _vec[i])
			delete _vec[i];
	}

	_vec.clear();
}

template<typename T, int iSize>
void Safe_Delete_Array(T(&arrname)[iSize])
{
	for (int i = 0; i < iSize; ++i)
	{
		if (nullptr != arrname[i])
			delete arrname[i];
	}
}


void SaveWString(const wstring& _str, FILE* _pFile);
void LoadWString(wstring& _str, FILE* _pFile);
void SaveStringg(const string& _str, FILE* _pFile);
void LoadStringg(string& _str, FILE* _pFile);

template<typename res>
void SaveResRefInfo(Ptr<res> _pRes, FILE* _pFile)
{
	wstring strKey, strRelativePath;
	bool bExist = false;

	if (nullptr == _pRes)
	{	
		fwrite(&bExist, 1, 1, _pFile);
	}
	else
	{
		bExist = true;
		fwrite(&bExist, 1, 1, _pFile);

		strKey = _pRes->GetKey();
		strRelativePath = _pRes->GetRelativePath();

		SaveWString(strKey, _pFile);
		SaveWString(strRelativePath, _pFile);
	}
}

class CResMgr;

template<typename res>
void LoadResRefInfo(Ptr<res>& _pRes, FILE* _pFile)
{
	wstring strKey, strRelativePath;
	bool bExist = false;

	fread(&bExist, 1, 1, _pFile);

	if (bExist)
	{
		LoadWString(strKey, _pFile);
		LoadWString(strRelativePath, _pFile);

		_pRes = CResMgr::GetInst()->FindRes<res>(strKey);
		if (nullptr == _pRes)
		{
			_pRes = CResMgr::GetInst()->Load<res>(strKey, strRelativePath);
		}
	}
}


char* GetString(wchar_t* _wchar);
char* GetString(const wstring& _wstr);

int GetSizeofFormat(DXGI_FORMAT _eFormat);


Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat);


bool ColliderCheck(Vec3 _mPos, Vec3 _TarPos, Vec3 _ColDir, float _Angle);
bool ColliderDistanceCheck(Vec3 _mPos, Vec3 _TarPos, Vec3 _CScale, float _CollDistance);
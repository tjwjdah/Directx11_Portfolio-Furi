#pragma once
#include "CEntity.h"

#include "Ptr.h"

class CRes :
    public CEntity
{
private:
    wstring         m_strKey;
    wstring         m_strRelativePath;

    int             m_iRefCount;

private:
    void AddRef() { ++m_iRefCount; }
    void SubRef() { --m_iRefCount; }

public:
    void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
    void SetRelativePath(const wstring& _strPath) { m_strRelativePath = _strPath; }

    const wstring& GetKey() { return m_strKey; }
    const wstring& GetRelativePath() { return m_strRelativePath; }

    int GetRefCount() { return m_iRefCount; }

public:
    virtual void Save(const wstring& _strRelativePath) = 0;
    virtual void SaveToScene(FILE* _pFile) {};
    virtual void LoadFromScene(FILE* _pFile) {};

private:
    virtual void Load(const wstring& _strFilePath) = 0;


    CLONE_DISABLE(CRes);

public:
    CRes();
    virtual ~CRes();

    template<typename T>
    friend class Ptr;

    friend class CResMgr;
};


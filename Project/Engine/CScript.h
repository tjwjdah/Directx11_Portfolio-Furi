#pragma once
#include "CComponent.h"

#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CResMgr.h"

#include "CTransform.h"
#include "CMeshRender.h"

#include "CPrefab.h"



enum class SCRIPT_DATA_TYPE
{
    INT,
    FLOAT,
    VEC2,
    VEC4,
    PREFAB,
    MEMFUNC,
};

struct tDataDesc;

class CScript :
    public CComponent
{
private:
    int                 m_iScriptType;
    vector<tDataDesc>   m_vecDesc;

public:
    static void Instantiate(Ptr<CPrefab> _pPrefab, Vec3 _vWorldPos, int _iLayerIdx = 0);
    static void DeleteObject(CGameObject* _pTarget);
    static void AddChild(CGameObject* _pParent, CGameObject* _pChild, UINT __ChildNum = 0);
    static void DisconnenctWithParent(CGameObject* _pTarget);
    
public:
    virtual void finalupdate() final {}
    virtual CScript* Clone() = 0;
    virtual void UpdateData() {};
    virtual void OnCollisionEnter(CGameObject* _pOther) {};
    virtual void OnCollision(CGameObject* _pOther) {};
    virtual void OnCollisionExit(CGameObject* _pOther) {};

protected:
    void AddDesc(const tDataDesc& _desc) { m_vecDesc.push_back(_desc); }

public:
    int GetScriptType() { return m_iScriptType; }
    const vector<tDataDesc>& GetDataDesc() { return m_vecDesc; }

    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile) {};

public:
    CScript(int _iScriptType);
    CScript(const CScript& _origin);
    ~CScript();

public:
    typedef void(CScript::* SCRIPT_MEMFUNC)(void);
};



struct tDataDesc
{
    SCRIPT_DATA_TYPE            eType;
    string                      strName;
    void* pData;
    CScript::SCRIPT_MEMFUNC     pMemFunc;

    tDataDesc(SCRIPT_DATA_TYPE _eType, const string& _strName, void* _pData, CScript::SCRIPT_MEMFUNC _pMemFunc = nullptr)
        : eType(_eType), strName(_strName), pData(_pData), pMemFunc(_pMemFunc)
    {}
};
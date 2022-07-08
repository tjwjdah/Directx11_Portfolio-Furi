#pragma once
#include "CComponent.h"
class CTransform :
    public CComponent
{
private:
    Vec3    m_vLocalPos;
    Vec3    m_vLocalScale;
    Vec3    m_vLocalRot;

    Vec3    m_vLocalDir[(UINT)DIR_TYPE::END];
    Vec3    m_vWorldDir[(UINT)DIR_TYPE::END];

    Matrix  m_matLocalScale;
    Matrix  m_matLocalRot;
    Matrix  m_matWorld;
    Matrix  m_matWorldInv;

   
public:
    virtual void update();
    virtual void finalupdate();

    void UpdateData();

public:
    Vec3 GetLocalPos() { return m_vLocalPos; }
    Vec3 GetLocalScale() { return m_vLocalScale; }
    Vec3 GetLocalRot() { return m_vLocalRot; }
    const Matrix& GetWorldMat() { return m_matWorld; }
    const Matrix& GetWorldInvMat() { return m_matWorldInv; }

    Vec3 GetWorldPos() { return Vec3(m_matWorld._41, m_matWorld._42, m_matWorld._43); };
    Vec3 GetWorldScale();
    //Vec3 GetWorldRot();

    Vec3 GetLocalDir(DIR_TYPE _eType) {return m_vLocalDir[(UINT)_eType];}
    Vec3 GetWorldDir(DIR_TYPE _eType) { return m_vWorldDir[(UINT)_eType]; }


    void SetLocalPos(Vec3 _vPos) { m_vLocalPos = _vPos;}

    void SetLocalPosX(float _x) { m_vLocalPos.x = _x; }
    void SetLocalPosY(float _y) { m_vLocalPos.y = _y; }
    void SetLocalPosZ(float _z) { m_vLocalPos.z = _z; }
    void SetLocalScale(Vec3 _vScale) { m_vLocalScale = _vScale; }
    void SetLocalScaleX(float _x) { m_vLocalScale.x = _x; }
    void SetLocalScaleY(float _y) { m_vLocalScale.y = _y; }
    void SetLocalScaleZ(float _z) { m_vLocalScale.z = _z; }
    void SetLocalRot(Vec3 _vLocalRot) { m_vLocalRot = _vLocalRot; }
    void SetLocalRotX(float _x) { m_vLocalRot.x = _x; }
    void SetLocalRotY(float _y) { m_vLocalRot.y = _y; }
    void SetLocalRotZ(float _z) { m_vLocalRot.z = _z; }
    void SetWorldScale(Vec3 _vScale);

public:
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);
    CLONE(CTransform);

public:
    CTransform();
    ~CTransform();
};


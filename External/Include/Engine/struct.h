#pragma once

#include "define.h"

struct VTX
{
	Vec3 vPos;
	Vec4 vColor;	
	Vec2 vUV;

	Vec3 vTangent;	// 접선
	Vec3 vNormal;	// 법선
	Vec3 vBinormal; // 종법선

	Vec4 vWeights;	// 본 가중치
	Vec4 vIndices;  // 본 인덱스
};

struct tParticle
{
	Vec3	vWorldPos;
	Vec3	vWorldDir;

	float	m_fCurTime;
	float	m_fMaxLife;
	float   m_fSpeed;

	int		iAlive;

private:
	int     iPadding[2];
};

struct tParticleShared
{
	int iAliveCount;
	int iPadding[3];
};

struct tLightColor
{
	Vec4 vDiff; // Diffuse (난반사광 세기)
	Vec4 vSpec; // 반사광 세기
	Vec4 vAmb;  // Ambient (환경광)
};

struct tLightInfo
{
	tLightColor color;  // 광원 색상

	Vec4		vDir;   // 광원의 방향
	Vec4		vWorldPos; // 광원의 위치

	LIGHT_TYPE  eType;	// 광원 타입		
	float		fRange; // 광원의 범위
	float		fAngle; // 광원 각도
	
	int			iPadding;
};

struct tAnim2D
{
	Vec2	vLeftTop;
	Vec2	vStep;
	int		iAnimUse[4];
};

struct tRay
{
	Vec3 vDir;		// 직선의 방향
	Vec3 vPoint;	// 직선위의 한 점
};


// ===========
// Instancing
// ===========
union uInstID
{
	struct {
		UINT iMesh;
		WORD iMtrl;
		WORD iMtrlIdx;
	};
	ULONG64 llID;
};

class CGameObject;

struct tInstObj
{
	CGameObject* pObj;
	UINT			iMtrlIdx;
};

// =============
// Animation 3D
// =============
struct tFrameTrans
{
	Vec4	vTranslate;
	Vec4	vScale;
	Vec4	qRot;
};

struct tMTKeyFrame
{
	double	dTime;
	int		iFrame;
	Vec3	vTranslate;
	Vec3	vScale;
	Vec4	qRot;
};


struct tMTBone
{
	wstring				strBoneName;
	int					iDepth;
	int					iParentIndx;
	Matrix				matOffset;	// Offset 행렬(뼈 -> 루트 까지의 행렬)
	Matrix				matBone;   // 이거 안씀
	vector<tMTKeyFrame>	vecKeyFrame;
};

struct tMTAnimClip
{
	wstring			strAnimName;
	int				iStartFrame;
	int				iEndFrame;
	int				iFrameLength;

	double			dStartTime;
	double			dEndTime;
	double			dTimeLength;
	float			fUpdateTime; // 이거 안씀

	FbxTime::EMode	eMode;
};



// ==============
// 상수버퍼 구조체
// ==============
struct tTransform
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;
	Matrix	matWV;
	Matrix  matWVP;

	Matrix	matWorldInv;
	Matrix	matViewInv;
};

struct tMtrlInfo
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
	Vec4 vEmis;

	int		arrInt[4];
	float	arrFloat[4];
	Vec2	arrVec2[4];
	Vec4	arrVec4[4];
	Matrix	arrMat[4];

	int     arrTexCheck[12];
	int		iAnim3D[4];		// Animation3D 추가정보 전달
};

struct tGlobalValue
{
	Vec2 vResolution;
	Vec2 vNoiseResolution;
	float fDeltaTime;
	float fAcctime;

	UINT iLight2DCount;
	UINT iLight3DCount;
};
struct tMeshRink
{
	//string s_ParentNode;
	//string s_ParentMesh;
	string s_NodeName;
	string s_MeshName;
	//bool   b_ParentMesh;
	//string s_ChildNode;
	//string s_ChildMesh;
	//bool   b_ChildMesh;
	//Vec3   v_ChildPos;
	//Vec3   v_ChildScale;
	//Vec3   v_ChildRot;
};

// nav mesh
struct NavMesh
{
	Vec3 fPointPos[3]; // 정점 위치 정보
	vector<UINT> vecAdjIdx; // 인접 노드 삼각형 idx
};

struct tSectorInfo
{
	float fAngle;
	float fDistance;
	int	  iPadding[2];
};
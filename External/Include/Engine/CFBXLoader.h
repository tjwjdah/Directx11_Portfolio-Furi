#pragma once
#include "global.h"


//===============
// Struct of FBX 
//===============
struct tFbxMaterial
{
	tMtrlInfo	tMtrl;
	wstring     strMtrlName;
	wstring     strDiff;
	wstring		strNormal;
	wstring		strSpec;
	wstring     strEmis;
};

struct tWeightsAndIndices
{
	int		iBoneIdx;
	double	dWeight;
};

struct tContainer
{
	wstring								strName;
	vector<Vec3>						vecPos;
	vector<Vec3>						vecTangent;
	vector<Vec3>						vecBinormal;
	vector<Vec3>						vecNormal;
	vector<Vec2>						vecUV;

	vector<Vec4>						vecIndices;
	vector<Vec4>						vecWeights;

	vector<vector<UINT>>				vecIdx;
	vector<tFbxMaterial>				vecMtrl;

	// Animation °ü·Ã Á¤º¸
	bool								bAnimation;
	vector<vector<tWeightsAndIndices>>	vecWI;

	void Resize(UINT _iSize)
	{
		vecPos.resize(_iSize);
		vecTangent.resize(_iSize);
		vecBinormal.resize(_iSize);
		vecNormal.resize(_iSize);
		vecUV.resize(_iSize);
		vecIndices.resize(_iSize);
		vecWeights.resize(_iSize);
		vecWI.resize(_iSize);
	}
};

struct tKeyFrame
{
	FbxAMatrix  matTransform;
	double		dTime;
};

struct tBone
{
	wstring				strBoneName;
	int					iDepth;			// °èÃþ±¸Á¶ ±íÀÌ
	int					iParentIndx;	// ºÎ¸ð Bone ÀÇ ÀÎµ¦½º
	FbxAMatrix			matOffset;		// Offset Çà·Ä( -> »Ñ¸® -> Local)
	FbxAMatrix			matBone;
	vector<tKeyFrame>	vecKeyFrame;
};

struct tAnimClip
{
	wstring		strName;
	FbxTime		tStartTime;
	FbxTime		tEndTime;
	FbxLongLong	llTimeLength;
	FbxTime::EMode eMode;
};

struct CameraClip {
	wstring		   ClipName;
	bool		   m_bnCamera;
	vector<Vec3>   bnC_Translation;
	vector<Vec3>   bnC_Scali;
	vector<Vec3>   bnC_Rotation;
};

class CMesh;

class CFBXLoader
{
private:
	FbxManager*						m_pManager;
	FbxScene*						m_pScene;
	FbxImporter*					m_pImporter;

	vector<tContainer>				m_vecContainer;	

	// Animation
	vector<vector<tBone*>>			m_vecBone;
	FbxArray<FbxString*>			m_arrAnimName;
	vector<tAnimClip*>				m_vecAnimClip;
	bool							m_CameraAnim;
	vector<CameraClip>              m_CameraData;

public:
	void init();
	void LoadFbx(const wstring& _strPath, LOADFROMFBXTYPE _Type = LOADFROMFBXTYPE::NONE);
	void SaveCameraClip();
public:
	int GetContainerCount() { return (int)m_vecContainer.size(); }	
	const tContainer& GetContainer(int _iIdx) { return m_vecContainer[_iIdx]; }
	vector<tBone*>& GetBones(int i=0) {return m_vecBone[(size_t)i];}
	UINT GetBonesSize() { return m_vecBone.size(); }
	vector<tAnimClip*>& GetAnimClip() { return m_vecAnimClip; }
	
private:
	void LoadMeshDataFromNode(FbxNode* _pRoot);
	void LoadMesh(FbxMesh* _pFbxMesh, const string& Name);
	void LoadMaterial(FbxSurfaceMaterial* _pMtrlSur);

	void GetTangent(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder);
	void GetBinormal(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder);
	void GetNormal(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder);
	void GetUV(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder);

	Vec4 GetMtrlData(FbxSurfaceMaterial * _pSurface, const char* _pMtrlName, const char* _pMtrlFactorName);
	wstring GetMtrlTextureName(FbxSurfaceMaterial * _pSurface, const char* _pMtrlProperty);

	void LoadTexture();
	void CreateMaterial();

	// Animation
	void LoadSkeleton(FbxNode* _pNode);
	void LoadSkeleton_Re(FbxNode* _pNode, int _iDepth, int _iIdx, int _iParentIdx,int _z);
	void LoadAnimationClip();
	void Triangulate(FbxNode* _pNode);

	void LoadAnimationData(FbxMesh* _pMesh, tContainer* _pContainer);
	void LoadWeightsAndIndices(FbxCluster* _pCluster, int _iBoneIdx, tContainer* _pContainer);
	void LoadOffsetMatrix(FbxCluster* _pCluster, const FbxAMatrix& _matNodeTransform, int _iBoneIdx, tContainer* _pContainer, int _i);
	void LoadKeyframeTransform(FbxNode* _pNode, FbxCluster* _pCluster, const FbxAMatrix& _matNodeTransform
		, int _iBoneIdx, tContainer* _pContainer, int _i);

	int FindBoneIndex(string _strBoneName);
	FbxAMatrix GetTransform(FbxNode* _pNode);

	void CheckWeightAndIndices(FbxMesh* _pMesh, tContainer * _pContainer);
public:
	CFBXLoader();
	~CFBXLoader();
};


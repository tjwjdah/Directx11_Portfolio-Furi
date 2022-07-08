#pragma once
#include "CComponent.h"

#include "Ptr.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "CMesh.h"
#include "CMeshRender.h"

class CStructuredBuffer;

class CAnimator3D : public CComponent
{
private:
	vector<const vector<tMTBone>*> m_pVecBones;
	//const vector<tMTBone>* m_pVecBones;
	const vector<tMTAnimClip>* m_pVecClip;

	vector<float>				m_vecClipUpdateTime;
	vector<Matrix>				m_vecFinalBoneMat; // 텍스쳐에 전달할 최종 행렬정보
	int							m_iFrameCount; // 30
	double						m_dCurTime;
	int							m_iCurClip; // 클립 인덱스	

	int							m_iFrameIdx; // 클립의 현재 프레임
	int							m_iNextFrameIdx; // 클립의 다음 프레임
	float						m_fRatio;	// 프레임 사이 비율

	CStructuredBuffer* m_pBoneFinalMatBuffer;  // 특정 프레임의 최종 행렬
	bool						m_bFinalMatUpdate; // 최종행렬 연산 수행여부

	bool m_bFinish;
	bool m_bRepeat;



	float m_fAniSpeed;

	// === change animation ===
	CStructuredBuffer* m_pChangeBoneFinalMatBuffer;  // 특정 프레임의 최종 행렬

	bool m_bBlend;
	float m_fBlendTime;
	int m_iChangeClip; //바뀐 클립 인덱스
	int m_iChangeFrameIdx; //프레임 인데긋


public:

	void ChangeClip(UINT _i, bool _bRepeat = true, float _fAniSpeed =1.0f);
	int ChangeClip(const wstring& _strName, bool _bRepeat, float _fAniSpeed = 1.5f);
	void ChangeAnimation(const wstring& _strAnimName)
	{
		int idx = FindAnimation(_strAnimName); //못찾으면 -1
		if (idx == -1)
		{
			assert(idx != -1); //false 일때 assert
			return;
		}

		m_bBlend = true;
		m_fBlendTime = 0.f;
		m_iChangeClip = idx;
		m_bFinish = false;
		//m_pCurAnim = iter->second;
		//m_pCurAnim->SetFrameIndex(0, true);
	}
	float GetAnimFrmRatio();


	void PlayAnimation(const wstring& _strName, bool _bRepeat = true);//, bool _bInit = true);
	
	int FindAnimation(const wstring& _strAnimName);

	void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones.push_back(_vecBones); m_vecFinalBoneMat.resize(m_pVecBones[0]->size()); }

	void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
	void UpdateData();

	void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }
	UINT GetBoneCount(UINT _i) { return (UINT)m_pVecBones[_i]->size(); }

	bool GetFinish() {return m_bFinish;}
	int GetCurClip() { return m_iCurClip; }
	int GetiFrameIdx() { return m_iFrameIdx; }
	double GetdCurTime() { return m_dCurTime; }

	float GetvecClipUpdateTime() { return m_vecClipUpdateTime[m_iCurClip]; }
	float GetvecClipEndTime(int _idx) { return m_pVecClip->at(_idx).dEndTime; }

	const vector<tMTAnimClip>* GetAnimClip() { return m_pVecClip; }

	CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMatBuffer; }
	//UINT GetBoneCount() { return (UINT)m_pVecBones->size(); }
	void ClearData();




public:
	void ClipNameSave();

private:
	void check_mesh(Ptr<CMesh> _pMesh);


public:
	virtual void awake() {};
	virtual void start() {};
	virtual void update();
	virtual void lateupdate();
	virtual void finalupdate();

	void NormalUpdate();

	void BlendAnimation();

public:
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);
	CAnimator3D* Clone() { return new CAnimator3D(*this); }

public:
	CAnimator3D();
	virtual ~CAnimator3D();
};

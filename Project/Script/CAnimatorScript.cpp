#include "pch.h"
#include "CAnimatorScript.h"
#include <Engine\CAnimator3D.h>

CAnimatorScript::CAnimatorScript() :
	CScript((int)SCRIPT_TYPE::ANIMATORSCRIPT)
	, m_ChangeAnimClip(0)
	, m_PreChangeAnimClip(0)
{
	AddDesc(tDataDesc(SCRIPT_DATA_TYPE::INT, "Int Data", &m_ChangeAnimClip));
}

CAnimatorScript::~CAnimatorScript()
{
}

void CAnimatorScript::awake()
{

}


void CAnimatorScript::update()
{
	if (m_ChangeAnimClip != m_PreChangeAnimClip)
	{
		SetAnimationClipNum(m_ChangeAnimClip);
		m_PreChangeAnimClip = m_ChangeAnimClip;
	}
}

void CAnimatorScript::SetAnimationClipNum(UINT _i)
{
	const vector<CGameObject*>& Childs= GetObj()->GetChild();
	
	for (UINT i = 0; i < Childs.size(); i++)
	{
		if (Childs[i]->Animator3D() != nullptr)
			Childs[i]->Animator3D()->ChangeClip(_i);
		if(Childs[i]->GetChild().size()>0)
			SetAnimationClipNum(Childs[i]->GetChild(),_i);
	}

}

void CAnimatorScript::SetAnimationClipNum(const vector<CGameObject*>& _Childs, UINT _i)
{
	for (UINT i = 0; i < _Childs.size(); i++)
	{
		if (_Childs[i]->Animator3D() != nullptr)
			_Childs[i]->Animator3D()->ChangeClip(_i);
		if (_Childs[i]->GetChild().size() > 0)
			SetAnimationClipNum(_Childs[i]->GetChild(), _i);
	}
}

void CAnimatorScript::SaveToScene(FILE* _pFile)
{
	CScript::SaveToScene(_pFile);

}

void CAnimatorScript::LoadFromScene(FILE* _pFile)
{
	CScript::LoadFromScene(_pFile);
}


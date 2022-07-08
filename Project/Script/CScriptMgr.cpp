#include "pch.h"
#include "CScriptMgr.h"

#include "CAnimatorScript.h"
#include "CBulletScript.h"
#include "CCameraAnimaitorScript.h"
#include "CCameraScript.h"
#include "CColliderCheck.h"
#include "CEffectScript.h"
#include "CHpbarScript.h"
#include "CLawScript.h"
#include "CMissileScript.h"
#include "CMissileScript2.h"
#include "CMissileScript3.h"
#include "CMonsterScript.h"
#include "CNavMeshManager.h"
#include "CPillarScript.h"
#include "CPlayerAttackScript.h"
#include "CPlayerScript.h"
#include "CRadianDirBulletScript.h"
#include "CScaleUpScript.h"
#include "CSecneChangeScript.h"
#include "CSectorColliderScript.h"
#include "CSetPlayerBosPos.h"
#include "CSkyBoxScript.h"
#include "CSlashScript.h"
#include "CStart2Script.h"
#include "CStartScript.h"
#include "CWallAxisScript.h"
#include "CWallScript.h"
#include "CWiseScript.h"
#include "CYoyoScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CAnimatorScript");
	_vec.push_back(L"CBulletScript");
	_vec.push_back(L"CCameraAnimaitorScript");
	_vec.push_back(L"CCameraScript");
	_vec.push_back(L"CColliderCheck");
	_vec.push_back(L"CEffectScript");
	_vec.push_back(L"CHpbarScript");
	_vec.push_back(L"CLawScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CMissileScript2");
	_vec.push_back(L"CMissileScript3");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CNavMeshManager");
	_vec.push_back(L"CPillarScript");
	_vec.push_back(L"CPlayerAttackScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CRadianDirBulletScript");
	_vec.push_back(L"CScaleUpScript");
	_vec.push_back(L"CSecneChangeScript");
	_vec.push_back(L"CSectorColliderScript");
	_vec.push_back(L"CSetPlayerBosPos");
	_vec.push_back(L"CSkyBoxScript");
	_vec.push_back(L"CSlashScript");
	_vec.push_back(L"CStart2Script");
	_vec.push_back(L"CStartScript");
	_vec.push_back(L"CWallAxisScript");
	_vec.push_back(L"CWallScript");
	_vec.push_back(L"CWiseScript");
	_vec.push_back(L"CYoyoScript");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CAnimatorScript" == _strScriptName)
		return new CAnimatorScript;
	if (L"CBulletScript" == _strScriptName)
		return new CBulletScript;
	if (L"CCameraAnimaitorScript" == _strScriptName)
		return new CCameraAnimaitorScript;
	if (L"CCameraScript" == _strScriptName)
		return new CCameraScript;
	if (L"CColliderCheck" == _strScriptName)
		return new CColliderCheck;
	if (L"CEffectScript" == _strScriptName)
		return new CEffectScript;
	if (L"CHpbarScript" == _strScriptName)
		return new CHpbarScript;
	if (L"CLawScript" == _strScriptName)
		return new CLawScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CMissileScript2" == _strScriptName)
		return new CMissileScript2;
	if (L"CMissileScript3" == _strScriptName)
		return new CMissileScript3;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"CNavMeshManager" == _strScriptName)
		return new CNavMeshManager;
	if (L"CPillarScript" == _strScriptName)
		return new CPillarScript;
	if (L"CPlayerAttackScript" == _strScriptName)
		return new CPlayerAttackScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CRadianDirBulletScript" == _strScriptName)
		return new CRadianDirBulletScript;
	if (L"CScaleUpScript" == _strScriptName)
		return new CScaleUpScript;
	if (L"CSecneChangeScript" == _strScriptName)
		return new CSecneChangeScript;
	if (L"CSectorColliderScript" == _strScriptName)
		return new CSectorColliderScript;
	if (L"CSetPlayerBosPos" == _strScriptName)
		return new CSetPlayerBosPos;
	if (L"CSkyBoxScript" == _strScriptName)
		return new CSkyBoxScript;
	if (L"CSlashScript" == _strScriptName)
		return new CSlashScript;
	if (L"CStart2Script" == _strScriptName)
		return new CStart2Script;
	if (L"CStartScript" == _strScriptName)
		return new CStartScript;
	if (L"CWallAxisScript" == _strScriptName)
		return new CWallAxisScript;
	if (L"CWallScript" == _strScriptName)
		return new CWallScript;
	if (L"CWiseScript" == _strScriptName)
		return new CWiseScript;
	if (L"CYoyoScript" == _strScriptName)
		return new CYoyoScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::ANIMATORSCRIPT:
		return new CAnimatorScript;
		break;
	case (UINT)SCRIPT_TYPE::BULLETSCRIPT:
		return new CBulletScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERAANIMAITORSCRIPT:
		return new CCameraAnimaitorScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERASCRIPT:
		return new CCameraScript;
		break;
	case (UINT)SCRIPT_TYPE::COLLIDERCHECK:
		return new CColliderCheck;
		break;
	case (UINT)SCRIPT_TYPE::EFFECTSCRIPT:
		return new CEffectScript;
		break;
	case (UINT)SCRIPT_TYPE::HPBARSCRIPT:
		return new CHpbarScript;
		break;
	case (UINT)SCRIPT_TYPE::LAWSCRIPT:
		return new CLawScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT2:
		return new CMissileScript2;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT3:
		return new CMissileScript3;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERSCRIPT:
		return new CMonsterScript;
		break;
	case (UINT)SCRIPT_TYPE::NAVMESHMANAGER:
		return new CNavMeshManager;
		break;
	case (UINT)SCRIPT_TYPE::PILLARSCRIPT:
		return new CPillarScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERATTACKSCRIPT:
		return new CPlayerAttackScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::RADIANDIRBULLETSCRIPT:
		return new CRadianDirBulletScript;
		break;
	case (UINT)SCRIPT_TYPE::SCALEUPSCRIPT:
		return new CScaleUpScript;
		break;
	case (UINT)SCRIPT_TYPE::SECNECHANGESCRIPT:
		return new CSecneChangeScript;
		break;
	case (UINT)SCRIPT_TYPE::SECTORCOLLIDERSCRIPT:
		return new CSectorColliderScript;
		break;
	case (UINT)SCRIPT_TYPE::SETPLAYERBOSPOS:
		return new CSetPlayerBosPos;
		break;
	case (UINT)SCRIPT_TYPE::SKYBOXSCRIPT:
		return new CSkyBoxScript;
		break;
	case (UINT)SCRIPT_TYPE::SLASHSCRIPT:
		return new CSlashScript;
		break;
	case (UINT)SCRIPT_TYPE::START2SCRIPT:
		return new CStart2Script;
		break;
	case (UINT)SCRIPT_TYPE::STARTSCRIPT:
		return new CStartScript;
		break;
	case (UINT)SCRIPT_TYPE::WALLAXISSCRIPT:
		return new CWallAxisScript;
		break;
	case (UINT)SCRIPT_TYPE::WALLSCRIPT:
		return new CWallScript;
		break;
	case (UINT)SCRIPT_TYPE::WISESCRIPT:
		return new CWiseScript;
		break;
	case (UINT)SCRIPT_TYPE::YOYOSCRIPT:
		return new CYoyoScript;
		break;
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::ANIMATORSCRIPT:
		return L"CAnimatorScript";
		break;

	case SCRIPT_TYPE::BULLETSCRIPT:
		return L"CBulletScript";
		break;

	case SCRIPT_TYPE::CAMERAANIMAITORSCRIPT:
		return L"CCameraAnimaitorScript";
		break;

	case SCRIPT_TYPE::CAMERASCRIPT:
		return L"CCameraScript";
		break;

	case SCRIPT_TYPE::COLLIDERCHECK:
		return L"CColliderCheck";
		break;

	case SCRIPT_TYPE::EFFECTSCRIPT:
		return L"CEffectScript";
		break;

	case SCRIPT_TYPE::HPBARSCRIPT:
		return L"CHpbarScript";
		break;

	case SCRIPT_TYPE::LAWSCRIPT:
		return L"CLawScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT2:
		return L"CMissileScript2";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT3:
		return L"CMissileScript3";
		break;

	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"CMonsterScript";
		break;

	case SCRIPT_TYPE::NAVMESHMANAGER:
		return L"CNavMeshManager";
		break;

	case SCRIPT_TYPE::PILLARSCRIPT:
		return L"CPillarScript";
		break;

	case SCRIPT_TYPE::PLAYERATTACKSCRIPT:
		return L"CPlayerAttackScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::RADIANDIRBULLETSCRIPT:
		return L"CRadianDirBulletScript";
		break;

	case SCRIPT_TYPE::SCALEUPSCRIPT:
		return L"CScaleUpScript";
		break;

	case SCRIPT_TYPE::SECNECHANGESCRIPT:
		return L"CSecneChangeScript";
		break;

	case SCRIPT_TYPE::SECTORCOLLIDERSCRIPT:
		return L"CSectorColliderScript";
		break;

	case SCRIPT_TYPE::SETPLAYERBOSPOS:
		return L"CSetPlayerBosPos";
		break;

	case SCRIPT_TYPE::SKYBOXSCRIPT:
		return L"CSkyBoxScript";
		break;

	case SCRIPT_TYPE::SLASHSCRIPT:
		return L"CSlashScript";
		break;

	case SCRIPT_TYPE::START2SCRIPT:
		return L"CStart2Script";
		break;

	case SCRIPT_TYPE::STARTSCRIPT:
		return L"CStartScript";
		break;

	case SCRIPT_TYPE::WALLAXISSCRIPT:
		return L"CWallAxisScript";
		break;

	case SCRIPT_TYPE::WALLSCRIPT:
		return L"CWallScript";
		break;

	case SCRIPT_TYPE::WISESCRIPT:
		return L"CWiseScript";
		break;

	case SCRIPT_TYPE::YOYOSCRIPT:
		return L"CYoyoScript";
		break;

	}
	return nullptr;
}
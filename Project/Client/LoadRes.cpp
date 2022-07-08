
#include "pch.h"
#include "TestScene.h"

#include <Engine\CScene.h>
#include <Engine\CResMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>
#include <Engine\CMeshRender.h>
#include <Engine\CCamera.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CCollider2D.h>
#include <Engine\CParticleSystem.h>
#include <Engine\CLight2D.h>
#include <Engine\CLight3D.h>
#include <Engine\CTerrain.h>

#include <Engine\CTexture.h>
#include <Engine\cmeshdata.h>
#include <Engine\CPathMgr.h>
#include <Engine\CCollisionMgr.h>
#include <Engine\CCopyShaderCS.h>
#include <Engine\CSceneMgr.h>

#include <Script\CPlayerScript.h>
#include <Script\CMonsterScript.h>
#include <Script\CMissileScript.h>
#include <Script\CSkyBoxScript.h>
#include <Script\CAnimatorScript.h>
#include <Script\CCameraScript.h>

#include <Script\CCameraAnimaitorScript.h>

#include "CSaveLoadMgr.h"


void SceneInit() {
	// 사운드 로딩
	CResMgr::GetInst()->Load<CSound>(L"111", L"sound\\bgm_title_main.wav");      //z
	//background
	CResMgr::GetInst()->Load<CSound>(L"father", L"sound\\background\\father.wav");
	CResMgr::GetInst()->Load<CSound>(L"Law", L"sound\\background\\Law.wav");
	CResMgr::GetInst()->Load<CSound>(L"main", L"sound\\background\\main.wav");
	CResMgr::GetInst()->Load<CSound>(L"wise", L"sound\\background\\wise.wav");

	//Player
	CResMgr::GetInst()->Load<CSound>(L"AttackLight550", L"sound\\Player\\AttackLight550.wav");
	CResMgr::GetInst()->Load<CSound>(L"combohit274", L"sound\\Player\\combohit274.wav");
	CResMgr::GetInst()->Load<CSound>(L"Gun_Shot_V1_05", L"sound\\Player\\Gun_Shot_V1_05.wav");
	CResMgr::GetInst()->Load<CSound>(L"gunshot1", L"sound\\Player\\gunshot1.wav");
	CResMgr::GetInst()->Load<CSound>(L"gunshot2", L"sound\\Player\\gunshot2.wav");
	CResMgr::GetInst()->Load<CSound>(L"hit183", L"sound\\Player\\hit183.wav");
	CResMgr::GetInst()->Load<CSound>(L"HurtLight610", L"sound\\Player\\HurtLight610.wav");
	CResMgr::GetInst()->Load<CSound>(L"Iconic_Shoot", L"sound\\Player\\Iconic_Shoot.wav");
	CResMgr::GetInst()->Load<CSound>(L"laser4", L"sound\\Player\\laser4.wav");
	CResMgr::GetInst()->Load<CSound>(L"laser6", L"sound\\Player\\laser6.wav");
	CResMgr::GetInst()->Load<CSound>(L"laserchargedcast13", L"sound\\Player\\laserchargedcast13.wav");
	CResMgr::GetInst()->Load<CSound>(L"MC_Chargedshot_Shockwave_4", L"sound\\Player\\MC_Chargedshot_Shockwave_4.wav");
	CResMgr::GetInst()->Load<CSound>(L"MC_Shooting_Mechanics_5", L"sound\\Player\\MC_Shooting_Mechanics_5.wav");
	CResMgr::GetInst()->Load<CSound>(L"parry5", L"sound\\Player\\parry5.wav");
	CResMgr::GetInst()->Load<CSound>(L"ParryBullet780", L"sound\\Player\\ParryBullet780.wav");
	CResMgr::GetInst()->Load<CSound>(L"parryhit413", L"sound\\Player\\parryhit413.wav");
	CResMgr::GetInst()->Load<CSound>(L"ProjectileAimingImpact207", L"sound\\Player\\ProjectileAimingImpact207.wav");
	CResMgr::GetInst()->Load<CSound>(L"ProjectileAimingImpact737", L"sound\\Player\\ProjectileAimingImpact737.wav");
	CResMgr::GetInst()->Load<CSound>(L"SlashElectric744", L"sound\\Player\\SlashElectric744.wav");
	CResMgr::GetInst()->Load<CSound>(L"SlashMassiveHit800", L"sound\\Player\\SlashMassiveHit800.wav");
	CResMgr::GetInst()->Load<CSound>(L"sword", L"sound\\Player\\sword.wav");

	CResMgr::GetInst()->Load<CSound>(L"Dash", L"sound\\Player\\Dash.wav");
	CResMgr::GetInst()->Load<CSound>(L"GunCharge", L"sound\\Player\\GunCharge.wav");
	CResMgr::GetInst()->Load<CSound>(L"lawmeleehit1", L"sound\\Player\\lawmeleehit1.wav");
	CResMgr::GetInst()->Load<CSound>(L"lawmeleehit2", L"sound\\Player\\lawmeleehit2.wav");
	CResMgr::GetInst()->Load<CSound>(L"lawmeleehit3", L"sound\\Player\\lawmeleehit3.wav");
	CResMgr::GetInst()->Load<CSound>(L"lawmeleehitkick", L"sound\\Player\\lawmeleehitkick.wav");


	CResMgr::GetInst()->Load<CSound>(L"SwordSlash_V3_SF_01", L"sound\\Player\\SwordSlash_V3_SF//SwordSlash_V3_SF_01.wav");
	CResMgr::GetInst()->Load<CSound>(L"SwordSlash_V3_SF_02", L"sound\\Player\\SwordSlash_V3_SF//SwordSlash_V3_SF_02.wav");
	CResMgr::GetInst()->Load<CSound>(L"SwordSlash_V3_SF_03", L"sound\\Player\\SwordSlash_V3_SF//SwordSlash_V3_SF_03.wav");
	CResMgr::GetInst()->Load<CSound>(L"SwordSlash_V3_SF_04", L"sound\\Player\\SwordSlash_V3_SF//SwordSlash_V3_SF_04.wav");
	CResMgr::GetInst()->Load<CSound>(L"SwordSlash_V3_SF_05", L"sound\\Player\\SwordSlash_V3_SF//SwordSlash_V3_SF_05.wav");
	CResMgr::GetInst()->Load<CSound>(L"SwordSlash_V3_SF_06", L"sound\\Player\\SwordSlash_V3_SF//SwordSlash_V3_SF_06.wav");
	CResMgr::GetInst()->Load<CSound>(L"SwordSlash_V3_Slice_06", L"sound\\Player\\SwordSlash_V3_SF//SwordSlash_V3_Slice_06.wav");
	
	//law
	CResMgr::GetInst()->Load<CSound>(L"rain201", L"sound\\law\\rain201.wav");

	CResMgr::GetInst()->Load<CSound>(L"dash", L"sound\\law\\dash.wav");
	CResMgr::GetInst()->Load<CSound>(L"telpo", L"sound\\law\\telpo.wav");


	CResMgr::GetInst()->Load<CSound>(L"charge_whooch136", L"sound\\law\\charge_whooch136.wav");

	CResMgr::GetInst()->Load<CSound>(L"elecimpact72", L"sound\\law\\elecimpact72.wav");
	CResMgr::GetInst()->Load<CSound>(L"electricimpact277", L"sound\\law\\electricimpact277.wav");
	CResMgr::GetInst()->Load<CSound>(L"grab_elecshock", L"sound\\law\\grab_elecshock.wav");
	CResMgr::GetInst()->Load<CSound>(L"impact_hurt259", L"sound\\law\\impact_hurt259.wav");

	CResMgr::GetInst()->Load<CSound>(L"melee_whoosh168", L"sound\\law\\melee_whoosh168.wav");
	CResMgr::GetInst()->Load<CSound>(L"melee_whoosh181", L"sound\\law\\melee_whoosh181.wav");
	CResMgr::GetInst()->Load<CSound>(L"melee_whoosh184", L"sound\\law\\melee_whoosh184.wav");
	CResMgr::GetInst()->Load<CSound>(L"melee_whoosh191", L"sound\\law\\melee_whoosh191.wav");
	CResMgr::GetInst()->Load<CSound>(L"melee_whoosh200", L"sound\\law\\melee_whoosh200.wav");

	CResMgr::GetInst()->Load<CSound>(L"bullet", L"sound\\law\\bullet.wav");

	CResMgr::GetInst()->Load<CSound>(L"angularwave_cast188", L"sound\\law\\angularwave_cast188.wav");
	CResMgr::GetInst()->Load<CSound>(L"angularwave93", L"sound\\law\\angularwave93.wav");

	CResMgr::GetInst()->Load<CSound>(L"swipeslash", L"sound\\law\\swipeslash.wav");
	CResMgr::GetInst()->Load<CSound>(L"swipeslash_cast121", L"sound\\law\\swipeslash_cast121.wav");
	CResMgr::GetInst()->Load<CSound>(L"swipeslash_prepa120", L"sound\\law\\swipeslash_prepa120.wav");

	CResMgr::GetInst()->Load<CSound>(L"wideslashRL", L"sound\\law\\wideslashRL.wav");

	CResMgr::GetInst()->Load<CSound>(L"yoyo", L"sound\\law\\yoyo.wav");
	CResMgr::GetInst()->Load<CSound>(L"yoyo247", L"sound\\law\\yoyo247.wav");

	//wise
	CResMgr::GetInst()->Load<CSound>(L"WiseWave", L"sound\\wise\\wisegrondwave916.wav");
	CResMgr::GetInst()->Load<CSound>(L"WiseShield", L"sound\\wise\\WiseShieldImpactHigh43.wav");
	CResMgr::GetInst()->Load<CSound>(L"WisePillar", L"sound\\wise\\WiseShieldImpact1003.wav");
	CResMgr::GetInst()->Load<CSound>(L"WiseBullet", L"sound\\wise\\WiseSphereImpact40.wav");
	CResMgr::GetInst()->Load<CSound>(L"WiseAttack", L"sound\\wise\\WiseAttack.wav");
	CResMgr::GetInst()->Load<CSound>(L"WiseTeleport", L"sound\\wise\\WiseTeleport.wav");
	CResMgr::GetInst()->Load<CSound>(L"WiseBullet1", L"sound\\wise\\WiseBullet1.wav");
	CResMgr::GetInst()->Load<CSound>(L"WiseBullet2", L"sound\\wise\\WiseBullet2.wav");
}
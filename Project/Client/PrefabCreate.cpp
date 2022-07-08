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
#include <Script/CLawScript.h>
#include <Script/CBulletScript.h>
#include <Script/CRadianDirBulletScript.h>
#include <Script/CScaleUpScript.h>
#include <Script/CSlashScript.h>
#include <Script\CPlayerAttackScript.h>
#include <Script\CYoyoScript.h>
#include <Script\CSectorColliderScript.h>
#include <Script/CHpbarScript.h>
#include <Script/CMissileScript2.h>
#include <Script/CMissileScript3.h>
#include <Script\CEffectScript.h>
#include "CSaveLoadMgr.h"

void CreateSamplePrefab()
{
	// ===============
	// Player Attack
	// ===============
	CGameObject* pObj = new CGameObject;
	pObj->SetName(L"PlayerAttack");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CPlayerAttackScript);
	pObj->AddComponent(new CCollider2D);
	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);
	Ptr<CPrefab> pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\PlayerAttack.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"PlayerAttack", pPrefab);


	// ===============
	// Attack 1
	// ===============
	pObj = new CGameObject;
	pObj->SetName(L"spreadbullet1");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CParticleSystem);
	pObj->AddComponent(new CRadianDirBulletScript);

	pObj->AddComponent(new CCollider2D);
	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);
	//pObj->Collider2D()->SetColRot(Vec3(0.f, 1.57f, 0.f));

	pObj->Transform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\spreadbullet1.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"spreadbullet1", pPrefab);

	// ===============
	// Attack 2
	// ===============
	pObj = new CGameObject;
	pObj->SetName(L"spreadbullet2");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CParticleSystem);
	pObj->AddComponent(new CBulletScript);

	pObj->AddComponent(new CCollider2D);
	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

	pObj->Transform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\spreadbullet2.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"spreadbullet2", pPrefab);
	
	Ptr<CTexture> pTexture = CResMgr::GetInst()->Load<CTexture>(L"slash02", L"slash02.png");
	// ============
	// Attack3
	//============
	pObj = new CGameObject;
	pObj->SetName(L"sector");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CBulletScript);
	pObj->AddComponent(new CScaleUpScript);
	pObj->AddComponent(new CSectorColliderScript);

	pObj->AddComponent(new CCollider2D);
	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(700.f, 700.f, 700.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f
	/*
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl_slashEffect"), 0);
	*/

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl_slashEffect"), 0);
	Vec3 vColor = Vec3(0.f, 0.f, 1.f);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, pTexture.Get());
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor.x);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor.y);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor.z);
	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\sector.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"sector", pPrefab);

	// ============
	// Attack4
	//============
	pObj = new CGameObject;
	pObj->SetName(L"yoyo");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CYoyoScript);

	pObj->AddComponent(new CCollider2D);
	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);
	//pObj->Collider2D()->SetvOffsetScale(Vec2(0.1f, 0.1f)); //************************************** 수정

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(700.f, 700.f, 700.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\yoyo.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"yoyo", pPrefab);

	// ============
	// Attack5
	//============
	pObj = new CGameObject;
	pObj->SetName(L"slash");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CSlashScript);

	pObj->AddComponent(new CCollider2D);
	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);
	//pObj->Collider2D()->SetvOffsetScale(Vec2(0.1f, 0.1f));

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\slash.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"slash", pPrefab);


	// ============
	// Attack6
	//============
	pObj = new CGameObject;
	pObj->SetName(L"sector2");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CSlashScript);
	pObj->AddComponent(new CSectorColliderScript);

	pObj->AddComponent(new CCollider2D);
	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);
	//pObj->Collider2D()->SetvOffsetScale(Vec2(0.1f, 0.1f));

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\sector2.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"sector2", pPrefab);


	// ==========
	// Attack7 (빨간 장판)
	//===========
	pObj = new CGameObject;
	pObj->SetName(L"slashrtol");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CBulletScript);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(700.f, 700.f, 700.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh_Sector"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl_Effect"), 0);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\slashrtol.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"slashrtol", pPrefab);

	// ============
	// boss hpbar   ***********************************c추가
	//============
	pObj = new CGameObject;
	pObj->SetName(L"hpbar");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	//pObj->AddComponent(new CHpbarScript);
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));  //RectMesh
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"HpbarMtrl"), 0);

	Ptr<CTexture> hpbartex = CResMgr::GetInst()->Load<CTexture>(L"hp_bar", L"texture\\ui\\hp_bar.png");
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, hpbartex.Get());

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));  //x :1.57

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\hpbar.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"hpbar", pPrefab);


	// ============
	// boss hpbar ***********************************c추가
	//============
	pObj = new CGameObject;
	pObj->SetName(L"hpbar2");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	//pObj->AddComponent(new CHpbarScript);
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));  //RectMesh
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"HpbarMtrl2"), 0);

	//Ptr<CTexture> hpbartex = CResMgr::GetInst()->Load<CTexture>(L"hp_bar", L"texture\\ui\\hp_bar.png");
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, hpbartex.Get());

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));  //x :1.57

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\hpbar2.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"hpbar2", pPrefab);


	// ============
	//life_bar ***********************************c추가
	//============
	pObj = new CGameObject;
	pObj->SetName(L"lifebar");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));  //RectMesh
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"lifebarMtrl"), 0);

	Ptr<CTexture> lifebartex = CResMgr::GetInst()->Load<CTexture>(L"life_bar", L"texture\\ui\\life_bar.png");
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, lifebartex.Get());

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));  //x :1.57

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\lifebar.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"lifebar", pPrefab);

	// ============
		// YellowBullet
		// ============
	pObj = new CGameObject;
	pObj->SetName(L"Slash");
	pObj->AddComponent(new CTransform);
	//pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CCollider2D);
	pObj->AddComponent(new CMissileScript);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(50.f, 50.f, 1.f));

	//pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\WiseSlash.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"WiseSlash", pPrefab);

	// ============
// YellowBullet
// ============
	pObj = new CGameObject;
	pObj->SetName(L"Bullet");
	pObj->AddComponent(new CTransform);
	//pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CCollider2D);
	pObj->AddComponent(new CParticleSystem);
	pObj->AddComponent(new CMissileScript);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(50.f, 50.f, 1.f));

	//pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

	pObj->ParticleSystem()->SetvStartColor(Vec4(1.f, 1.f, 0.f, 1.f));
	pObj->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.f, 0.f, 1.f));

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\YellowBullet.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"YellowBullet", pPrefab);

	// ============
	// SpreadBullet
	// ============
	pObj = new CGameObject;
	pObj->SetName(L"SpreadBullet");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CCollider2D);
	pObj->AddComponent(new CParticleSystem);
	pObj->AddComponent(new CMissileScript2);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(50.f, 50.f, 1.f));

	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

	pObj->ParticleSystem()->SetvStartColor(Vec4(0.f, 1.f, 0.f, 1.f));
	pObj->ParticleSystem()->SetvEndColor(Vec4(0.f, 1.f, 0.f, 1.f));

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\SpreadBullet.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"SpreadBullet", pPrefab);

	// ============
	// Fanwise
	// ============
	pObj = new CGameObject;
	pObj->SetName(L"Fanwise");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CCollider2D);
	pObj->AddComponent(new CMissileScript3);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(700.f, 700.f, 700.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));

	pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh_Sector"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl_Effect"), 0);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\Fanwise.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"Fanwise", pPrefab);
	


	// 9/3 c추가 ========= **********
	//시작버튼
	pObj = new CGameObject;
	pObj->SetName(L"startbut");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);

	pObj->AddComponent(new CMonsterScript);

	pObj->Transform()->SetLocalPos(Vec3(-590.f, -100.f, 1.f));
	pObj->Transform()->SetLocalScale(Vec3(660, 170, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"StartBut2DMtrl"), 0);  // ++ 쉐이더 고칠것

	Ptr<CTexture> pstartbut = CResMgr::GetInst()->Load<CTexture>(L"newgamebut", L"texture\\ui\\edit_3.png");
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, pstartbut.Get());

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\startbut.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"startbut", pPrefab);

	//나가기버튼
	pObj = new CGameObject;
	pObj->SetName(L"endbut");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CMonsterScript);

	pObj->Transform()->SetLocalPos(Vec3(-600.f, -170.f, 1.f));
	pObj->Transform()->SetLocalScale(Vec3(700.f, 170.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"EndBut2DMtrl"), 0);  // ++ 쉐이더 고칠것

	Ptr<CTexture> pendbut = CResMgr::GetInst()->Load<CTexture>(L"endbut", L"texture\\ui\\edit_4.png");
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, pendbut.Get());

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\endbut.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"endbut", pPrefab);

	//start 로고
	pObj = new CGameObject;
	pObj->SetName(L"mainlogo");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CAnimator2D);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 10.f));
	pObj->Transform()->SetLocalScale(Vec3(1600.f, 900.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Main2DMtrl"), 0);  // ++ 쉐이더 고칠것

	Ptr<CTexture> pimage1 = CResMgr::GetInst()->Load<CTexture>(L"image1", L"texture\\ui\\image1.png");
	pObj->Animator2D()->CreateAnimation(L"MAIN", pimage1, Vec2(0.f, 0.f), Vec2(1302.f, 733.f), 11, 0.2f);
	pObj->Animator2D()->PlayAnimation(L"MAIN", false);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\mainlogo.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"mainlogo", pPrefab);
	// ******************************

	//start 로고 2222
	pObj = new CGameObject;
	pObj->SetName(L"main2logo");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CAnimator2D);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 10.f));
	pObj->Transform()->SetLocalScale(Vec3(1600.f, 900.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Main2DMtrl"), 0);  // ++ 쉐이더 고칠것

	Ptr<CTexture> pimage2 = CResMgr::GetInst()->Load<CTexture>(L"image2", L"texture\\ui\\image2.png");
	pObj->Animator2D()->CreateAnimation(L"MAIN_1", pimage2, Vec2(0.f, 0.f), Vec2(1300.f, 730.f), 11, 0.2f);
	pObj->Animator2D()->CreateAnimation(L"MAIN_2", pimage2, Vec2(0.f, 730.f), Vec2(1300.f, 730.f), 11, 0.2f);
	pObj->Animator2D()->CreateAnimation(L"MAIN_3", pimage2, Vec2(0.f, 730.f * 2), Vec2(1300.f, 730.f), 11, 0.2f);
	pObj->Animator2D()->CreateAnimation(L"MAIN_4", pimage2, Vec2(0.f, 730.f * 3), Vec2(1300.f, 730.f), 11, 0.2f);
	//pObj->Animator2D()->PlayAnimation(L"MAIN_1", true);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\main2logo.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"main2logo", pPrefab);





	//Effect

	pObj = new CGameObject;
	pObj->SetName(L"HitEffect");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CParticleSystem);
	pObj->AddComponent(new CEffectScript);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 100.f, 1000.f));
	pObj->ParticleSystem()->SetPaticleName(L"Hit");
	pObj->ParticleSystem()->SetShaderIdx(1);
	pObj->ParticleSystem()->SetiMaxCount(100);
	pObj->ParticleSystem()->SetvCreateRange(Vec4(2.f, 2.f, 1.f, 0.f));
	pObj->ParticleSystem()->SetvStartColor(Vec4(1.f, 0.f, 0.0f, 1.f));
	pObj->ParticleSystem()->SetvEndColor(Vec4(0.5f, 0.5f, 0.f, 0.5f));
	pObj->ParticleSystem()->SetfMinSpeed(2);
	pObj->ParticleSystem()->SetfMaxSpeed(5);
	pObj->ParticleSystem()->SetvStartScale(Vec3(200.f, 0.f, 0.f));
	pObj->ParticleSystem()->SetvEndScale(Vec3(30.f, 0.f, 0.f));
	pObj->ParticleSystem()->SetDirection(0);
	pObj->ParticleSystem()->SetfMinLifeTime(0.5f);
	pObj->ParticleSystem()->SetfMaxLifeTime(0.55f);
	pObj->ParticleSystem()->SetfFrequency(0.05f);

	CEffectScript* BulletS = (CEffectScript*)pObj->GetScript(L"CEffectScript");
	BulletS->SetType(EFFECT::NONE);
	BulletS->SetTime(0.5f);
	BulletS->SetStopTime(0.2f);

	pPrefab = new CPrefab(pObj);
	//pPrefab->Save(L"prefab\\HitEffect.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"HitEffect", pPrefab);

	pObj = new CGameObject;
	pObj->SetName(L"ElecEffect");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CParticleSystem);
	pObj->AddComponent(new CEffectScript);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 100.f, 500.f));
	pObj->ParticleSystem()->SetPaticleName(L"eeeeeee");
	pObj->ParticleSystem()->SetShaderIdx(3);

	pObj->ParticleSystem()->SetiMaxCount(10);
	pObj->ParticleSystem()->SetvCreateRange(Vec4(5.f, 30.f, 5.f, 0.f));
	pObj->ParticleSystem()->SetvStartColor(Vec4(0.8f, 0.8f, 0.5f, 0.8f));
	pObj->ParticleSystem()->SetvEndColor(Vec4(1.f, 1.f, 1.f, 1.f));
	pObj->ParticleSystem()->SetfMinSpeed(200);
	pObj->ParticleSystem()->SetfMaxSpeed(200);
	pObj->ParticleSystem()->SetvStartScale(Vec3(150.f, 0.f, 0.f));
	pObj->ParticleSystem()->SetvEndScale(Vec3(100.f, 0.f, 0.f));
	pObj->ParticleSystem()->SetDirection(-1);
	pObj->ParticleSystem()->SetfMinLifeTime(0.1f);
	pObj->ParticleSystem()->SetfMaxLifeTime(0.5f);
	pObj->ParticleSystem()->SetfFrequency(0.1f);

	BulletS = (CEffectScript*)pObj->GetScript(L"CEffectScript");
	BulletS->SetType(EFFECT::NONE);
	BulletS->SetTime(10.f);
	BulletS->SetStopTime(0.7f);

	pPrefab = new CPrefab(pObj);
	//pPrefab->Save(L"prefab\\ElecEffect.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"ElecEffect", pPrefab);

	Ptr<CTexture> slash_circle01 = CResMgr::GetInst()->Load<CTexture>(L"slash_circle01", L"texture\\slash_circle01.png");

	pObj = new CGameObject;
	pObj->SetName(L"slash_circle01");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CEffectScript);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(7.f, 1.f, 7.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f

	BulletS = (CEffectScript*)pObj->GetScript(L"CEffectScript");
	BulletS->SetType(EFFECT::ROT);
	BulletS->SetTime(9999.f);
	BulletS->SetStopTime(9999.f);
	BulletS->SetRotScale(0.25f);
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"slash_circle01"), 0);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, slash_circle01.Get());
	Vec3 vColor2 = Vec3(0.f, 0.f, 1.f);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor2.x);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor2.y);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor2.z);

	Ptr<CTexture> slash_circle02 = CResMgr::GetInst()->Load<CTexture>(L"slash_circle02", L"texture\\slash_circle02.png");

	CGameObject* pObj2 = new CGameObject;
	pObj2->SetName(L"slash_circle02");
	pObj2->AddComponent(new CTransform);
	pObj2->AddComponent(new CMeshRender);
	pObj2->AddComponent(new CEffectScript);
	pObj2->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj2->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObj2->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f
	BulletS = (CEffectScript*)pObj2->GetScript(L"CEffectScript");
	BulletS->SetType(EFFECT::ROT);
	BulletS->SetTime(9999.f);
	BulletS->SetStopTime(9999.f);
	BulletS->SetbRot(true);
	BulletS->SetRotScale(0.25f);
	pObj2->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj2->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"slash_circle02"), 0);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor2.x);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor2.y);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor2.z);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, slash_circle02.Get());
	pObj->AddChild(pObj2);


	CGameObject* pObj3 = new CGameObject;
	pObj3->SetName(L"Particle");
	pObj3->AddComponent(new CTransform);
	pObj3->AddComponent(new CParticleSystem);
	pObj3->Transform()->SetLocalScale(Vec3(1.f, 1.1f, 1.f));
	pObj3->ParticleSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerParticleRenderMtrl"));
	pObj3->ParticleSystem()->SetfMinLifeTime(1.f);
	pObj3->ParticleSystem()->SetfMaxLifeTime(1.f);
	pObj3->ParticleSystem()->SetfMinSpeed(40.f);
	pObj3->ParticleSystem()->SetfMaxSpeed(60.f);
	pObj3->ParticleSystem()->SetDirection(0);
	pObj3->ParticleSystem()->SetiMaxCount(100.f);
	pObj3->ParticleSystem()->SetfFrequency(0.001f);
	pObj3->ParticleSystem()->SetvCreateRange(Vec4(900.f, 100.f, 900.f, 1.f));
	pObj3->ParticleSystem()->SetvEndColor(Vec4(0.0f, 0.2f, 0.7f, 0.8f));
	pObj3->ParticleSystem()->SetvStartColor(Vec4(0.0f, 0.124f, 0.5f, 0.3f));
	pObj3->ParticleSystem()->SetvStartScale(Vec3(10.f, 10.f, 1.f));
	pObj3->ParticleSystem()->SetvEndScale(Vec3(10.f, 10.f, 1.f));

	pObj3->ParticleSystem()->SetDirection(0);
	pObj->AddChild(pObj3);

	pPrefab = new CPrefab(pObj);
	//pPrefab->Save(L"prefab\\slash_circle0.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"slash_circle0", pPrefab);


	CResMgr::GetInst()->Load<CTexture>(L"magic_line", L"texture\\particle//magic_line.png");
	pObj = new CGameObject;
	pObj->SetName(L"magic_line");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CParticleSystem);
	pObj->AddComponent(new CEffectScript);
	pObj->Transform()->SetLocalPos(Vec3(0.3f, 1.f, -0.3f));
	pObj->ParticleSystem()->SetPaticleName(L"magic_line");
	pObj->ParticleSystem()->SetShaderIdx(1);
	BulletS = (CEffectScript*)pObj->GetScript(L"CEffectScript");
	BulletS->SetTime(9999.f);
	BulletS->SetStopTime(9999.f);
	pObj->ParticleSystem()->SetiMaxCount(100);
	pObj->ParticleSystem()->SetvCreateRange(Vec4(250.f, 200.f, 250.f, 1.f));
	pObj->ParticleSystem()->SetvEndColor(Vec4(0.0f, 0.2f, 0.7f, 1.0f));
	pObj->ParticleSystem()->SetvStartColor(Vec4(0.0f, 0.124f, 0.5f, 0.8f));
	pObj->ParticleSystem()->SetfMinSpeed(100);
	pObj->ParticleSystem()->SetfMaxSpeed(100);
	pObj->ParticleSystem()->SetvStartScale(Vec3(10.f, 0.f, 00.f));
	pObj->ParticleSystem()->SetvEndScale(Vec3(20.f, 0.f, 0.f));
	pObj->ParticleSystem()->SetDirection(0);
	pObj->ParticleSystem()->SetfMinLifeTime(0.1f);
	pObj->ParticleSystem()->SetfMaxLifeTime(0.5f);
	pObj->ParticleSystem()->SetfFrequency(0.1f);
	pObj->ParticleSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DegreesParticleRenderMtrl"));


	CResMgr::GetInst()->Load<CTexture>(L"glow1", L"texture\\particle//glow1.png");
	pObj2 = new CGameObject;
	pObj2->SetName(L"Effect");
	pObj2->AddComponent(new CTransform);
	pObj2->AddComponent(new CParticleSystem);
	pObj2->ParticleSystem()->SetPaticleName(L"glow1");
	pObj2->ParticleSystem()->SetShaderIdx(1);
	pObj2->ParticleSystem()->SetiMaxCount(100);
	pObj2->ParticleSystem()->SetvCreateRange(Vec4(2.f, 2.f, 1.f, 0.f));
	pObj2->ParticleSystem()->SetvEndColor(Vec4(0.0f, 0.2f, 0.7f, 1.0f));
	pObj2->ParticleSystem()->SetvStartColor(Vec4(0.0f, 0.124f, 0.5f, 0.8f));
	pObj2->ParticleSystem()->SetfMinSpeed(2);
	pObj2->ParticleSystem()->SetfMaxSpeed(5);
	pObj2->ParticleSystem()->SetvStartScale(Vec3(20.f, 0.f, 0.f));
	pObj2->ParticleSystem()->SetvEndScale(Vec3(100.f, 0.f, 0.f));
	pObj2->ParticleSystem()->SetDirection(0);
	pObj2->ParticleSystem()->SetfMinLifeTime(0.3f);
	pObj2->ParticleSystem()->SetfMaxLifeTime(0.35f);
	pObj2->ParticleSystem()->SetfFrequency(0.05f);
	pObj->AddChild(pObj2);

	pPrefab = new CPrefab(pObj);
	//pPrefab->Save(L"prefab\\magic_line.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"magic_line", pPrefab);


	Ptr<CTexture> ring = CResMgr::GetInst()->Load<CTexture>(L"ring", L"texture\\ring.png");
	// 1번째
	pObj = new CGameObject;
	pObj->SetName(L"ring1");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);

	pObj->Transform()->SetLocalPos(Vec3(00.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(700.f, 700.f, 700.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ring1"), 0);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, ring.Get());

	Vec3 vColor3 = Vec3(0.f, 1.f, 1.f);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor3.x);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor3.y);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor3.z);


	Ptr<CTexture> glow1 = CResMgr::GetInst()->FindRes<CTexture>(L"glow1");

	pObj2 = new CGameObject;
	pObj2->SetName(L"ring2");
	pObj2->AddComponent(new CTransform);
	pObj2->AddComponent(new CMeshRender);
	pObj2->AddComponent(new CEffectScript);
	pObj2->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj2->Transform()->SetLocalScale(Vec3(0.7f, 0.7f, 0.7f));
	pObj2->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f
	BulletS = (CEffectScript*)pObj2->GetScript(L"CEffectScript");
	BulletS->SetType(EFFECT::SCALE);
	BulletS->SetTime(1000.f);
	BulletS->SetbRot(true);
	BulletS->SetAddScale(Vec3(0.5f, 0.5f, 0.5f));
	BulletS->SetScaleTime(1.f);
	BulletS->SetScale(Vec3(0.7f, 0.7f, 0.7f));
	pObj2->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj2->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ring2"), 0);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, glow1.Get());

	Vec3 vColor8 = Vec3(0.f, 0.1f, 0.2647059f);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor8.x);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor8.y);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor8.z);
	pObj->AddChild(pObj2);

	pObj3 = new CGameObject;
	pObj3->SetName(L"Effect");
	pObj3->AddComponent(new CTransform);
	pObj3->AddComponent(new CParticleSystem);
	//pHit->AddComponent(new CEffectScript);
	pObj3->ParticleSystem()->SetPaticleName(L"glow1");
	pObj3->ParticleSystem()->SetiMaxCount(50);
	pObj3->ParticleSystem()->SetvCreateRange(Vec4(800.f, 100.f, 800.f, 1.f));
	pObj3->ParticleSystem()->SetvEndColor(Vec4(0.0f, 0.8705882f, 1.f, 0.2f));
	pObj3->ParticleSystem()->SetvStartColor(Vec4(0.0f, 0.8705882f, 1.f, 0.6f));
	pObj3->ParticleSystem()->SetfMinSpeed(2);
	pObj3->ParticleSystem()->SetfMaxSpeed(3);
	pObj3->ParticleSystem()->SetvStartScale(Vec3(10.f, 0.f, 0.f));
	pObj3->ParticleSystem()->SetvEndScale(Vec3(10.f, 0.f, 0.f));
	pObj3->ParticleSystem()->SetDirection(0);
	pObj3->ParticleSystem()->SetfMinLifeTime(1.0f);
	pObj3->ParticleSystem()->SetfMaxLifeTime(1.0f);
	pObj3->ParticleSystem()->SetfFrequency(0.05f);

	pObj->AddChild(pObj3);

	pPrefab = new CPrefab(pObj);
	//pPrefab->Save(L"prefab\\ring1.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"ring1", pPrefab);



	Ptr<CTexture> portal_circular2 = CResMgr::GetInst()->Load<CTexture>(L"portal_circular2", L"texture\\portal_circular2.png");
	// 1번째
	pObj = new CGameObject;
	pObj->SetName(L"portal_circular2");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CEffectScript);
	pObj->AddComponent(new CAnimator2D);
	pObj->Transform()->SetLocalPos(Vec3(700.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(700.f, 700.f, 700.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"portal_circular"), 0);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, portal_circular2.Get());
	pObj->Animator2D()->CreateAnimation(L"circular", portal_circular2, Vec2(0.f, 0.f), Vec2(512.f, 512.f), 4, 0.05f);
	pObj->Animator2D()->PlayAnimation(L"circular", true);
	Vec3 vColor4 = Vec3(0.f, 0.5f, 1.f);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor4.x);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor4.y);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor4.z);

	BulletS = (CEffectScript*)pObj->GetScript(L"CEffectScript");
	BulletS->SetType(EFFECT::ROT);
	BulletS->SetTime(1000.f);
	BulletS->SetbRot(true);


	pObj2 = new CGameObject;
	pObj2->SetName(L"Effect");
	pObj2->AddComponent(new CTransform);
	pObj2->AddComponent(new CParticleSystem);
	//pHit->AddComponent(new CEffectScript);
	pObj2->ParticleSystem()->SetPaticleName(L"glow1");
	pObj2->ParticleSystem()->SetiMaxCount(100);
	pObj2->ParticleSystem()->SetvCreateRange(Vec4(300.f, 10.f, 300.f, 1.f));
	pObj2->ParticleSystem()->SetvEndColor(Vec4(0.6274509f, 0.8150102f, 1.f, 0.5f));
	pObj2->ParticleSystem()->SetvStartColor(Vec4(0.6274509f, 0.8150102f, 1.f, 1.f));
	pObj2->ParticleSystem()->SetfMinSpeed(2);
	pObj2->ParticleSystem()->SetfMaxSpeed(3);
	pObj2->ParticleSystem()->SetvStartScale(Vec3(100.f, 0.f, 0.f));
	pObj2->ParticleSystem()->SetvEndScale(Vec3(100.f, 0.f, 0.f));
	pObj2->ParticleSystem()->SetDirection(0);
	pObj2->ParticleSystem()->SetfMinLifeTime(1.0f);
	pObj2->ParticleSystem()->SetfMaxLifeTime(1.0f);
	pObj2->ParticleSystem()->SetfFrequency(0.05f);

	pObj->AddChild(pObj2);


	pPrefab = new CPrefab(pObj);
	//pPrefab->Save(L"prefab\\portal_circular2.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"portal_circular2", pPrefab);


	Ptr<CTexture> portal_stripes_blue2 = CResMgr::GetInst()->Load<CTexture>(L"portal_stripes_blue2", L"texture\\portal_stripes_blue2.png");
	// 1번째
	pObj = new CGameObject;
	pObj->SetName(L"portal_stripes_blue2");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CParticleSystem);
	pObj->AddComponent(new CEffectScript);
	pObj->AddComponent(new CAnimator2D);
	pObj->Transform()->SetLocalPos(Vec3(700.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(700.f, 700.f, 700.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f

	pObj->Animator2D()->CreateAnimation(L"circular", portal_stripes_blue2, Vec2(0.f, 0.f), Vec2(512.f, 512.f), 4, 0.4f);
	pObj->Animator2D()->PlayAnimation(L"circular", true);
	pObj->ParticleSystem()->SetiType(1);
	pObj->ParticleSystem()->SetiMaxCount(12);
	pObj->ParticleSystem()->SetvCreateRange(Vec4(10.f, 10.f, 10.f, 1.f));
	pObj->ParticleSystem()->SetvEndColor(Vec4(0.f, 1.f, 1.f, 1.f));
	pObj->ParticleSystem()->SetvStartColor(Vec4(1.f, 1.f, 1.f, 1.f));
	pObj->ParticleSystem()->SetfMinSpeed(1);
	pObj->ParticleSystem()->SetfMaxSpeed(1);
	pObj->ParticleSystem()->SetvStartScale(Vec3(700.f, 0.f, 0.f));
	pObj->ParticleSystem()->SetvEndScale(Vec3(000.f, 0.f, 0.f));
	pObj->ParticleSystem()->SetDirection(0);
	pObj->ParticleSystem()->SetfMinLifeTime(0.4f);
	pObj->ParticleSystem()->SetfMaxLifeTime(0.4f);
	pObj->ParticleSystem()->SetfFrequency(0.2f);
	pObj->ParticleSystem()->SetShaderIdx(1);
	BulletS = (CEffectScript*)pObj->GetScript(L"CEffectScript");
	BulletS->SetType(EFFECT::ROT);
	BulletS->SetTime(1000.f);
	BulletS->SetbRot(true);
	BulletS->SetStopTime(1000.f);
	

	pPrefab = new CPrefab(pObj);
	//pPrefab->Save(L"prefab\\portal_stripes_blue2.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"portal_stripes_blue2", pPrefab);
	

	Ptr<CTexture> portal_ring_twisty2 = CResMgr::GetInst()->Load<CTexture>(L"portal_ring_twisty2", L"texture\\portal_ring_twisty2.png");
	// 1번째
	pObj = new CGameObject;
	pObj->SetName(L"portal_ring_twisty2");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CEffectScript);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(7.f, 1.f, 7.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"portal_ring_twisty2"), 0);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, portal_ring_twisty2.Get());

	Vec3 vColor5 = Vec3(0.f, 0.796f, 1.f);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor5.x);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor5.y);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor5.z);
	BulletS = (CEffectScript*)pObj->GetScript(L"CEffectScript");
	BulletS->SetType(EFFECT::ROT);
	BulletS->SetTime(9999.f);
	BulletS->SetbRot(true);
	BulletS->SetStopTime(9999.f);
	BulletS->SetRotScale(0.25f);

	CResMgr::GetInst()->Load<CTexture>(L"sparkle3", L"texture\\particle//sparkle3.png");
	pObj2 = new CGameObject;
	pObj2->SetName(L"Effect");
	pObj2->AddComponent(new CTransform);
	pObj2->AddComponent(new CParticleSystem);
	//pHit->AddComponent(new CEffectScript);
	pObj2->ParticleSystem()->SetPaticleName(L"sparkle3");
	pObj2->ParticleSystem()->SetShaderIdx(3);
	pObj2->ParticleSystem()->SetiMaxCount(100);
	pObj2->ParticleSystem()->SetvCreateRange(Vec4(700.f, 100.f, 700.f, 0.f));
	pObj2->ParticleSystem()->SetvEndColor(Vec4(0.0f, 0.8344831f, 1.0f, 1.0f));
	pObj2->ParticleSystem()->SetvStartColor(Vec4(1.0f, 1.f, 1.0f, 1.0f));
	pObj2->ParticleSystem()->SetfMinSpeed(100);
	pObj2->ParticleSystem()->SetfMaxSpeed(100);
	pObj2->ParticleSystem()->SetvStartScale(Vec3(15.f, 0.f, 0.f));
	pObj2->ParticleSystem()->SetvEndScale(Vec3(15.f, 0.f, 0.f)); 
	pObj2->ParticleSystem()->SetDirection(0);
	pObj2->ParticleSystem()->SetfMinLifeTime(0.5f);
	pObj2->ParticleSystem()->SetfMaxLifeTime(0.5f);
	pObj2->ParticleSystem()->SetfFrequency(0.05f);

	pObj->AddChild(pObj2);

	pPrefab = new CPrefab(pObj);
	//pPrefab->Save(L"prefab\\portal_ring_twisty2.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"portal_ring_twisty2", pPrefab);



	Ptr<CTexture> portal_ring_twisty = CResMgr::GetInst()->Load<CTexture>(L"portal_ring_twisty", L"texture\\portal_ring_twisty.png");
	// 1번째
	pObj = new CGameObject;
	pObj->SetName(L"portal_ring_twisty");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CEffectScript);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(1000.f, 1.f, 1000.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"portal_ring_twisty"), 0);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, portal_ring_twisty.Get());

	Vec3 vColor6 = Vec3(0.f, 0.796f, 1.f);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor6.x);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor6.y);
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor6.z);
	BulletS = (CEffectScript*)pObj->GetScript(L"CEffectScript");
	BulletS->SetType(EFFECT::ROT);
	BulletS->SetTime(9999.f);
	BulletS->SetbRot(true);
	BulletS->SetStopTime(9999.f);
	BulletS->SetRotScale(0.1f);

	Ptr<CTexture> circle222 = CResMgr::GetInst()->Load<CTexture>(L"circle222", L"texture\\circle222.png");

	pObj2 = new CGameObject;
	pObj2->SetName(L"portal_ring_twisty");
	pObj2->AddComponent(new CTransform);
	pObj2->AddComponent(new CMeshRender);

	pObj2->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj2->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObj2->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));// XM_PI / 2.f

	pObj2->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
	pObj2->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"circle222"), 0);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, circle222.Get());

	Vec3 vColor7 = Vec3(0.f, 0.f, 0.1647059);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_0, &vColor7.x);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_1, &vColor7.y);
	pObj2->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::FLOAT_2, &vColor7.z);
	pObj->AddChild(pObj2);


	pObj3 = new CGameObject;
	pObj3->SetName(L"Effect");
	pObj3->AddComponent(new CTransform);
	pObj3->AddComponent(new CParticleSystem);
	//pHit->AddComponent(new CEffectScript);
	pObj3->ParticleSystem()->SetPaticleName(L"glow1");
	pObj3->ParticleSystem()->SetShaderIdx(3);
	pObj3->ParticleSystem()->SetiMaxCount(100);
	pObj3->ParticleSystem()->SetvCreateRange(Vec4(900.f, 100.f, 900.f, 0.f));
	pObj3->ParticleSystem()->SetvEndColor(Vec4(0.0f, 1.f, 1.0f, 1.0f));
	pObj3->ParticleSystem()->SetvStartColor(Vec4(0.0f, 1.f, 1.0f, 1.0f));
	pObj3->ParticleSystem()->SetfMinSpeed(100);
	pObj3->ParticleSystem()->SetfMaxSpeed(100);
	pObj3->ParticleSystem()->SetvStartScale(Vec3(15.f, 0.f, 0.f));
	pObj3->ParticleSystem()->SetvEndScale(Vec3(8.f, 0.f, 0.f));
	pObj3->ParticleSystem()->SetDirection(0);
	pObj3->ParticleSystem()->SetfMinLifeTime(1.0f);
	pObj3->ParticleSystem()->SetfMaxLifeTime(1.2f);
	pObj3->ParticleSystem()->SetfFrequency(0.05f);

	pObj->AddChild(pObj3);

	pPrefab = new CPrefab(pObj);
	//pPrefab->Save(L"prefab\\portal_ring_twisty.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"portal_ring_twisty", pPrefab);

	// PostEffect
	

	pObj = new CGameObject;
	pObj->SetName(L"PostEffect_ripple");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	
	pObj->Transform()->SetLocalPos(Vec3(-3000.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(20000.f, 20000.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, -0.1f, 0.f));
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_ripple"), 0);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\PostEffect_ripple.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"PostEffect_ripple", pPrefab);

	pObj = new CGameObject;
	pObj->SetName(L"PostEffect_GlichLine");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	
	pObj->Transform()->SetLocalPos(Vec3(-3000.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(20000.f, 20000.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, -0.1f, 0.f));
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_GlichLine"), 0);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\PostEffect_GlichLine.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"PostEffect_GlichLine", pPrefab);

	pObj = new CGameObject;
	pObj->SetName(L"PostEffect_filter");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	
	pObj->Transform()->SetLocalPos(Vec3(-3000.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(20000.f, 20000.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, -0.1f, 0.f));
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_filter"), 0);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\PostEffect_filter.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"PostEffect_filter", pPrefab);

	pObj = new CGameObject;
	pObj->SetName(L"PostEffect_Colorfilter");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	
	pObj->Transform()->SetLocalPos(Vec3(-3000.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(20000.f, 20000.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, -0.1f, 0.f));
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_Colorfilter"), 0);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\PostEffect_Colorfilter.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"PostEffect_Colorfilter", pPrefab);

	pObj = new CGameObject;
	pObj->SetName(L"PostEffect_blur");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	
	pObj->Transform()->SetLocalPos(Vec3(-3000.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(20000.f, 20000.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, -0.1f, 0.f));
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_blur"), 0);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\PostEffect_blur.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"PostEffect_blur", pPrefab);

	pObj = new CGameObject;
	pObj->SetName(L"PostEffect_Vignette");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	
	pObj->Transform()->SetLocalPos(Vec3(-3000.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(20000.f, 20000.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, -0.1f, 0.f));
	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PostEffectMtrl_Vignette"), 0);

	pPrefab = new CPrefab(pObj);
	pPrefab->Save(L"prefab\\PostEffect_Vignette.pref");
	CResMgr::GetInst()->AddRes<CPrefab>(L"PostEffect_Vignette", pPrefab);
}

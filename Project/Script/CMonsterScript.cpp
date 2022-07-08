#include "pch.h"
#include "CMonsterScript.h"
#include "CPlayerScript.h"

CMonsterScript::CMonsterScript()
    : CScript((int)SCRIPT_TYPE::MONSTERSCRIPT)
{
}

CMonsterScript::~CMonsterScript()
{
}
void CMonsterScript::awake()
{

}
void CMonsterScript::update()
{

    Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();
    vMousePos.x = vMousePos.x - 1600 / 2;
    vMousePos.y = -vMousePos.y + 900 / 2 + 60.f; //60 마우스 offset값

    Vec3 vScale = Transform()->GetLocalScale();
    vScale.y = vScale.y - 20.f;
    Vec3 vMyPos = Transform()->GetLocalPos();

    if (vMyPos.x < vMousePos.x && vMousePos.x < vMyPos.x + vScale.x
        && vMyPos.y < vMousePos.y && vMousePos.y < vMyPos.y + vScale.y)
    {
        m_bMouseOn = true;
    }
    else
    {
        m_bMouseOn = false;
    }



}
//
//void CMonsterScript::OnCollisionEnter(CGameObject* _pOther)
//{	
//	CPlayerScript* pScript = dynamic_cast<CPlayerScript*>(_pOther->GetComponent(COMPONENT_TYPE::SCRIPT));
//	if (nullptr != pScript)
//	{
//		DeleteObject(GetObj());
//	}
//}
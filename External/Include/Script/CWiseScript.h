#pragma once
#include <Engine\CScript.h>

class CPrefab;
class CGameObject;
class CCameraAnimaitorScript;

enum class WISE_PHASE
{
	PHASE_1,
	PHASE_2,
	PHASE_3,
	PHASE_4,
	NONE
};

enum class WISE_STATE
{
	IDLE,
	MOVE,
	CHARGE,
	ATTACK,
	SHOOT,
	STUN,
	HIT,
	KNOCKDOWN,
	DIE,
	SPECIAL,
	NONE
};

enum class WISE_MOVE
{
	LEFT,
	FRONT,
	BACK,
	TELEPORT,
	TELEPORT_CENTER,
	TELEPORT_OUT,
	TELEPORT_IN,
	FAR_READY,
	NEAR_READY,
	NONE
};

enum class WISE_ATTACK
{
	TYPE1,
	TYPE2,
	NONE
};

enum class WISE_SHOOT
{
	CIRCLE,
	FANWISE,
	FLOOR,
	NONE
};

enum class WISE_ANIMPHASE
{
	FIRST,
	SECOND,
	THIRD,
	NONE
};

enum class WISE_SPECIAL
{
	FIRST,
	SECOND,
	THIRD,
	FOURTH,
	FIFTH,
	NONE
};

class CWiseScript
	:public CScript
{
private:
	// Objects
	CGameObject* m_pPlayer;

	Ptr<CPrefab>			m_Yellow;	// 노란 총알
	Ptr<CPrefab>			m_Spread;	// 가만히 있다 6방향으로 터지는 총알
	Ptr<CPrefab>			m_Slash;	// 근접공격
	Ptr<CPrefab>			m_Fanwise;	// 부채꼴 투사체

	Ptr<CMeshData>			m_Pillar;

	// enum class
	WISE_PHASE				m_Phase;
	WISE_STATE				m_State;
	WISE_STATE				m_prevState;
	WISE_STATE				m_changeState;
	WISE_MOVE				m_Move;
	WISE_ATTACK				m_Attack;
	WISE_SHOOT				m_Shoot;
	WISE_SPECIAL			m_Special;

	// Status
	Vec3					m_vPos;
	Vec2					m_vDir;
	float					m_fSpeed;
	float					m_fHP;
	int						m_iHpToken;
	int						m_iHpColor;
	int						m_iLifeCount;
	int						m_i3PhaseDead;

	int						m_iPatternRepeat;

	bool					m_bRed;
	float					m_RedTime;
	float					m_RedTimeMax;

	bool					m_bPhaseShoot;
	bool					m_bZoomIn;

	bool					m_bMoveCheck;
	bool					m_bCollision;

	// Projectiles
	float					m_fShootTimeMax;
	float					m_fShootTime;
	float					m_fShootTimeMax2;
	float					m_fShootTime2;
	float					m_fEndTimeMax;
	float					m_fEndTime;

	int						m_iFloorNum;

	// Animations
	CGameObject* m_AnimObj;
	const vector<tMTAnimClip>* m_vecAnimClip;

	WISE_ANIMPHASE			m_AnimPhase;

	float					m_fAnimTimeMax;
	float					m_fAnimTime;

	bool					m_bAnim;

	CCameraAnimaitorScript* m_CameraAnim;
	bool					m_bCameraZoom;

public:
	virtual void awake();
	virtual void update();

private:
	void Idle();
	void Move();
	void Attack();
	void Shoot();
	void Stun();
	void Hit();
	void Knockdown();
	void Die();
	void Special();

	void ForRed();

public:
	void LookAt(Vec3 _TargetPos);

	void ChangeState();		// 애니메이션 끝까지 보고 State변경
	void CheckAnimation();

	void PlayAnimToEnd(const wstring& _strName, bool _Repeat, WISE_STATE _State, float _AddTime = 0.f, bool _bNone = true, bool _bCamera = false);

	void HitMoment();
	void KnockdownMoment();

	void RemovePillar() { m_i3PhaseDead--; };

	float GetfHP() { return m_fHP; }
	int GetLife() { return m_iLifeCount; }
	int GetHpColor() { return m_iHpColor; }

public:
	void SetPatternRepeat(int _Num) { m_iPatternRepeat = _Num; }
	bool GetCollision() { return m_bCollision; }

public:
	virtual void OnCollisionEnter(CGameObject* _pOther);
	//virtual void OnCollision(CGameObject* _pOther);
	CLONE(CWiseScript);

public:
	CWiseScript();
	~CWiseScript();
};

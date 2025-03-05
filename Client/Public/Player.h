#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider_Manager;
class CModel;
class CCollider;
class CHyerarchNode;
class CNavigation;
END

BEGIN(Client)
class CUI_PlayerHp;
class CCamera_Player;
class CSeamLess_Loader;
class CPlayer final : public CGameObject
{
public:
	/* 플레이어의 상태를 구분짓기 위한 열거체 */
	enum PLAYERSTATE {PLAYER_IDLE, PLAYER_WALK, PLAYER_DASH, PLAYER_ROLLING, PLAYER_ATTACK, PLAYER_ATTACK_FINAL, PLAYER_WEAR, PLAYER_DAMAGED, PLAYER_DAMAGED_MAGMA, PLAYER_JUMP, PLAYER_WALL, PLAYER_HANG, PLAYER_SWIM, PLAYER_GRAB, PLAYER_LADDER, PLAYER_PUSH_PULL, PLAYER_CLIMB, PLAYER_CLIMB_CUBE, PLAYER_CLIMB_HiGH, PLAYER_CLIMB_HiGH_END, PLAYER_GETITEM, PLAYER_GETITEM_SHORT, PLAYER_BOKO,  PLAYER_END};
	
	/*하나의 동작이 여러 애니메이션으로 나뉜 경우 상태를 관리하기 위한 열거체 */
	enum PLAYERCLIMBCUBESTATE { CLIMBCUBE_READY, CLIMBCUBE_IDLE, CLIMBCUBE_STAND, CLIMBCUBE_END };
	enum PLAYERCLIMBSTATE { CLIMB_READY, CLIMB_FIANL, CLIMB_END };
	enum PLAYERJUMPSTATE {JUMP_READY, JUMP_ING, JUMP_LAND, JUMP_END };
	enum PLAYERGRABSTATE { GRAB_IDLE, GRAB_READY, GRAB_WALK, GRAB_THROW, GRAB_END };
	enum PLAYERLADDERSTATE { LADDER_READY, LADDER_UP_LEFT, LADDER_UP_RIGHT, LADDER_FINISH, LADDER_END};
	enum PLAYERITEMSTATE { ITEM_OPEN, ITEM_OPEN_SHORT, ITEM_GET, ITEM_END };
	enum PLAYERBOKOSTATE { BOKO_IDLE, BOKO_ATTACK, BOKO_THROW, BOKO_END };
	//플레이어가 바라보는 방향을 관리하기 위한 열거체 
	enum DIR { DIR_FRONT, DIR_BACK, DIR_LEFT, DIR_RIGHT, DIR_1, DIR_7, DIR_9, DIR_3, DIR_END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public: /* Get Function */
	virtual _vector Get_Position() const override
	{
		return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	}

	_vector Get_Look() const
	{
		return m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	}

	_bool Get_Attention() const
	{
		return m_bAttention;
	}

	PLAYERSTATE Get_State() const
	{
		return m_PlayerState;
	}

	_bool Get_ItemState() const
	{
		return m_bWearItem;
	}

	_vector Get_GrapPoint() const
	{
		return XMLoadFloat4(&m_vGrapPoint);
	}

	_float Get_CoolDown() const
	{
		return m_fCooldown;
	}

	const _uint& Get_NumKey() const
	{
		return m_iNumKey;
	}

	_bool Get_UnderCollision() const
	{
		return m_bUnderCollision;
	}

	_uint Get_RoomId() const
	{
		return m_iNextRoomID;
	}

public: /* Set Function */
	void Set_Axis(_fvector Axis_Front, _fvector Axis_Side)
	{
		XMStoreFloat4(&m_Axis_Front, Axis_Front);
		XMStoreFloat4(&m_Axis_Side, Axis_Side);
	}

	void Set_Collision(_bool bResult)
	{
		m_bCollision = bResult;
	}

	void Set_UnderCollision(_bool bResult)
	{
		m_bUnderCollision = bResult;
	}

	void Set_UnderObjectSizeY(_float fSizeY)
	{
		m_fSizeOfCubeY = fSizeY;
	}

	void Set_NumUnderObject(_uint iNum)
	{
		m_iNumUnderObject = iNum;
	}

	void Set_State(PLAYERSTATE eState)
	{
		m_PlayerState = eState;
	}

	void Set_CoolDown()
	{
		m_fCooldown = 0.f;
	}

	void Set_NumKey(_int iSumNum)
	{
		m_iNumKey += iSumNum;
	}

	void Set_OpenBox(_bool bResult)
	{
		m_bCanOpenBox = bResult;
	}

	void Set_OpenBox_Short(_bool bResult)
	{
		m_bCanOpenBoxShort = bResult;
	}

	void Find_RealCell(_float3 vPos);

	void Set_UnderMagRock(_bool bResult)
	{
		m_bUnderMagRock = bResult;
	}

public:
	void Check_Front(_double TimeDelta, _bool bResult, _float fFirstX, _float fFirstZ);
	void Jump_Reset()
	{
		m_bJump = false;
		m_fJumpTimeAcc = 0.f;
		m_fJumpPower = 0.f;
	}

public:
	HRESULT SetUp_Components(); 
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_PlayerUI();
	HRESULT SetUp_OnTerrain();

private: 
	void Set_HpUI();
	void Input_Key(_float fTimeDelta);
	void Input_Key_Attention(_float fTimeDelta);
	void Pattern_Jump(_float fTimeDelta);
	void Pattern_Hang(_float fTimeDelta);
	void Pattern_Swim(_float fTimeDelta);
	void Pattern_PushOrPull(_float fTimeDelta);
	void Pattern_climb(_float fTimeDelta);
	void Pattern_Climb_Cube(_float fTimeDelta);
	void Pattern_Climb_High(_float fTimeDelta);
	void Pattern_Grab_Throw(_float fTimeDelta);
	void Pattern_Damaged_Magma(_float fTimeDelta);
	void Pattern_Ladder(_float fTimeDelta);
	void Pattern_GetItem(_float fTimeDelta);
	void Pattern_Boko(_float fTimeDelta);

private: // Component
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CCollider*						m_pSPHERE = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;

private:
	_float4x4 m_PivotMatrix;
	_float4x4 m_SocketMatrix;

private:
	_uint m_iNumKey = 0; // 플레이어가 가지고 있는 열쇠의 개수
	PLAYERSTATE m_ePlayerPreState = PLAYER_END;

private: // PlayerHp
	_uint m_iPlayerHp = 12;
	_int m_iPlayerHpAcc = 0;
	vector<CUI_PlayerHp*> m_PlayerHps;
	typedef vector<CUI_PlayerHp*> PLAYERHPS;

private: // 애니메이션 제어를 위한 변수들
	_bool m_bBodyAnimFinished = false;
	_bool m_bWaistAnimFinished = false;
	_float m_fBodyAnimSpeed = 1.f; // 상체 애니메이션들의 실행 속도를 통제하는 변수
	_float m_fWaistAnimSpeed = 1.f; // 하체 애니메이션들의 실행 속도를 통제하는 변수
	_bool m_bPlayAnim = true;

private:
	_bool m_bAttention = false; // 플레이어가 주목을 하고있는지를 판단하기 위한 변수
	PLAYERSTATE m_PlayerState = PLAYER_END; // 플레이어의 현 동작의 상태를 부여하는 변수
	_float m_fTickAcc = 0.f; // Tick 다중 호출 방지용 변수
	_uint m_iAttackAcc = 0; // 공격의 콤보 횟수를 설정하기 위한 변수
	_uint m_iRollingAcc = 0; // 하나의 애니메이션이 여러 개로 나뉜 경우를 위한 변수
	DIR m_eDir = DIR_END;
	_bool m_bWearItem = true; // 현재 검과 방패를 장착한 상태인지 확인하기 위한 변수 
	_bool m_bDamaged = false; // 플레이어가 데미지를 입은 상태인지 확인하기 위한 변수
	_float m_fPatternAcc = 0.f;
	_uint m_iPatternCount = 0;
	_float m_fChangeAnimCount = 0.f; 

private: // 플레이어의 점프 상태를 컨트롤 하기 위한 변수
	_bool m_bJump = false; 
	_float	m_fJumpTimeAcc = 0.f;
	_float	m_fJumpPower = 0.f;
	_float m_fCurrentPosY = 0.f;

private: // 플레이어의 수영 상태를 컨트롤 하기 위한 변수
	_bool m_bSwim = false;

private: // 플레이어가 지형을 오르는 상태를 컨트롤하기 위한 변수들
	_bool m_bClimb = false;
	PLAYERCLIMBSTATE m_eState_Climb = CLIMB_END;

private: // 플레이어가 벽에 붙어서 이동하는 상태를 컨트롤하기 위한 변수들
	_bool m_bSneak = false;

private: // 플레이어가 큐브에 충돌한 상태를 컨트롤하기 위한 변수들
	PLAYERCLIMBCUBESTATE m_eSTATE_CUBE = CLIMBCUBE_END;
	_bool m_bCollision = false;
	_bool m_bUnderCollision = false;
	_bool m_bTargetFront = false;
	_float m_fSizeOfCubeY = 0.f;
	_uint m_iNumUnderObject = 0;
	_float m_fClimbTimeAcc = 0.f; // Climb상태에서 앞으로 살짝 이동시켜주기 위한 변수
	_bool m_bUnderMagRock = false;
	_float m_fCubeJumpTimeAcc = 0.f;

private: // 플레이어가 오브젝트를 들고 던지는 상태를 컨트롤하기 위한 변수들
	PLAYERGRABSTATE m_eSTATE_GRAB = GRAB_END;
	_float4 m_vGrapPoint;

private: // 플레이어가 사다리를 오르는 상태를 컨트롤하기 위한 변수들
	PLAYERLADDERSTATE m_eState_Ladder = LADDER_END;

private: // 플레이어가 상자에서 아이템을 얻는 상태를 컨트롤하기 위한 변수들
	PLAYERITEMSTATE m_eState_GetItem = ITEM_END;
	_bool m_bCanOpenBox = false;
	_bool m_bCanOpenBoxShort = false;
	_float m_fGetItemAnimAcc = 0.f;

private: // 플레이어가 Boko를 들고있는 상태를 컨트롤하기 위한 변수들 
	PLAYERBOKOSTATE m_eState_Boko = BOKO_END;

private:
	_float m_fCooldown = 0.f;
	_uint m_iNextRoomID = 1;

private: // 이동 축에 대한 변수들
	_float4 m_Axis_Front;
	_float4 m_Axis_Side;

private: /*포인터 객체 변수들 */
	CGameObject* m_pTarget = nullptr;
	CSeamLess_Loader* m_pSeamLess_Loader = nullptr;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
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
	/* �÷��̾��� ���¸� �������� ���� ����ü */
	enum PLAYERSTATE {PLAYER_IDLE, PLAYER_WALK, PLAYER_DASH, PLAYER_ROLLING, PLAYER_ATTACK, PLAYER_ATTACK_FINAL, PLAYER_WEAR, PLAYER_DAMAGED, PLAYER_DAMAGED_MAGMA, PLAYER_JUMP, PLAYER_WALL, PLAYER_HANG, PLAYER_SWIM, PLAYER_GRAB, PLAYER_LADDER, PLAYER_PUSH_PULL, PLAYER_CLIMB, PLAYER_CLIMB_CUBE, PLAYER_CLIMB_HiGH, PLAYER_CLIMB_HiGH_END, PLAYER_GETITEM, PLAYER_GETITEM_SHORT, PLAYER_BOKO,  PLAYER_END};
	
	/*�ϳ��� ������ ���� �ִϸ��̼����� ���� ��� ���¸� �����ϱ� ���� ����ü */
	enum PLAYERCLIMBCUBESTATE { CLIMBCUBE_READY, CLIMBCUBE_IDLE, CLIMBCUBE_STAND, CLIMBCUBE_END };
	enum PLAYERCLIMBSTATE { CLIMB_READY, CLIMB_FIANL, CLIMB_END };
	enum PLAYERJUMPSTATE {JUMP_READY, JUMP_ING, JUMP_LAND, JUMP_END };
	enum PLAYERGRABSTATE { GRAB_IDLE, GRAB_READY, GRAB_WALK, GRAB_THROW, GRAB_END };
	enum PLAYERLADDERSTATE { LADDER_READY, LADDER_UP_LEFT, LADDER_UP_RIGHT, LADDER_FINISH, LADDER_END};
	enum PLAYERITEMSTATE { ITEM_OPEN, ITEM_OPEN_SHORT, ITEM_GET, ITEM_END };
	enum PLAYERBOKOSTATE { BOKO_IDLE, BOKO_ATTACK, BOKO_THROW, BOKO_END };
	//�÷��̾ �ٶ󺸴� ������ �����ϱ� ���� ����ü 
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
	_uint m_iNumKey = 0; // �÷��̾ ������ �ִ� ������ ����
	PLAYERSTATE m_ePlayerPreState = PLAYER_END;

private: // PlayerHp
	_uint m_iPlayerHp = 12;
	_int m_iPlayerHpAcc = 0;
	vector<CUI_PlayerHp*> m_PlayerHps;
	typedef vector<CUI_PlayerHp*> PLAYERHPS;

private: // �ִϸ��̼� ��� ���� ������
	_bool m_bBodyAnimFinished = false;
	_bool m_bWaistAnimFinished = false;
	_float m_fBodyAnimSpeed = 1.f; // ��ü �ִϸ��̼ǵ��� ���� �ӵ��� �����ϴ� ����
	_float m_fWaistAnimSpeed = 1.f; // ��ü �ִϸ��̼ǵ��� ���� �ӵ��� �����ϴ� ����
	_bool m_bPlayAnim = true;

private:
	_bool m_bAttention = false; // �÷��̾ �ָ��� �ϰ��ִ����� �Ǵ��ϱ� ���� ����
	PLAYERSTATE m_PlayerState = PLAYER_END; // �÷��̾��� �� ������ ���¸� �ο��ϴ� ����
	_float m_fTickAcc = 0.f; // Tick ���� ȣ�� ������ ����
	_uint m_iAttackAcc = 0; // ������ �޺� Ƚ���� �����ϱ� ���� ����
	_uint m_iRollingAcc = 0; // �ϳ��� �ִϸ��̼��� ���� ���� ���� ��츦 ���� ����
	DIR m_eDir = DIR_END;
	_bool m_bWearItem = true; // ���� �˰� ���и� ������ �������� Ȯ���ϱ� ���� ���� 
	_bool m_bDamaged = false; // �÷��̾ �������� ���� �������� Ȯ���ϱ� ���� ����
	_float m_fPatternAcc = 0.f;
	_uint m_iPatternCount = 0;
	_float m_fChangeAnimCount = 0.f; 

private: // �÷��̾��� ���� ���¸� ��Ʈ�� �ϱ� ���� ����
	_bool m_bJump = false; 
	_float	m_fJumpTimeAcc = 0.f;
	_float	m_fJumpPower = 0.f;
	_float m_fCurrentPosY = 0.f;

private: // �÷��̾��� ���� ���¸� ��Ʈ�� �ϱ� ���� ����
	_bool m_bSwim = false;

private: // �÷��̾ ������ ������ ���¸� ��Ʈ���ϱ� ���� ������
	_bool m_bClimb = false;
	PLAYERCLIMBSTATE m_eState_Climb = CLIMB_END;

private: // �÷��̾ ���� �پ �̵��ϴ� ���¸� ��Ʈ���ϱ� ���� ������
	_bool m_bSneak = false;

private: // �÷��̾ ť�꿡 �浹�� ���¸� ��Ʈ���ϱ� ���� ������
	PLAYERCLIMBCUBESTATE m_eSTATE_CUBE = CLIMBCUBE_END;
	_bool m_bCollision = false;
	_bool m_bUnderCollision = false;
	_bool m_bTargetFront = false;
	_float m_fSizeOfCubeY = 0.f;
	_uint m_iNumUnderObject = 0;
	_float m_fClimbTimeAcc = 0.f; // Climb���¿��� ������ ��¦ �̵������ֱ� ���� ����
	_bool m_bUnderMagRock = false;
	_float m_fCubeJumpTimeAcc = 0.f;

private: // �÷��̾ ������Ʈ�� ��� ������ ���¸� ��Ʈ���ϱ� ���� ������
	PLAYERGRABSTATE m_eSTATE_GRAB = GRAB_END;
	_float4 m_vGrapPoint;

private: // �÷��̾ ��ٸ��� ������ ���¸� ��Ʈ���ϱ� ���� ������
	PLAYERLADDERSTATE m_eState_Ladder = LADDER_END;

private: // �÷��̾ ���ڿ��� �������� ��� ���¸� ��Ʈ���ϱ� ���� ������
	PLAYERITEMSTATE m_eState_GetItem = ITEM_END;
	_bool m_bCanOpenBox = false;
	_bool m_bCanOpenBoxShort = false;
	_float m_fGetItemAnimAcc = 0.f;

private: // �÷��̾ Boko�� ����ִ� ���¸� ��Ʈ���ϱ� ���� ������ 
	PLAYERBOKOSTATE m_eState_Boko = BOKO_END;

private:
	_float m_fCooldown = 0.f;
	_uint m_iNextRoomID = 1;

private: // �̵� �࿡ ���� ������
	_float4 m_Axis_Front;
	_float4 m_Axis_Side;

private: /*������ ��ü ������ */
	CGameObject* m_pTarget = nullptr;
	CSeamLess_Loader* m_pSeamLess_Loader = nullptr;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
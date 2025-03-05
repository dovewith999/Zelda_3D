#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CNavigation;
class CCollider_Manager;
END

BEGIN(Client)
class CPlayer;
class CItem_Boko;
class CItem_Blade;
class CMonster final : public CGameObject
{
public :
	enum MONSTERSTATE {STATE_IDLE, STATE_ATTACK, STATE_FIND, STATE_GUARD, STATE_DAMAGED, STATE_DEAD, STATE_END};
	enum ATTACKSTATE { ATTACK_READY, ATTACK_ING, ATTACK_FINAL, ATTACK_END };
private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual _vector Get_Position() const override
	{
		return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	}

	_float Get_Distance() const;

	
private: /* Component */
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CCollider*						m_pSPHERE = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;


private:/* 객체 */
	CPlayer* m_pPlayer = nullptr;
	CItem_Blade* m_pItem_Blade = nullptr;

private:
	MONSTERSTATE m_eState = STATE_END; // 몬스터의 상태를 관리하기 위한 변수
	ATTACKSTATE m_eAttack = ATTACK_END; // 공격 애니메이션 관이를 위한 단계 변수
	_bool m_bAnimFinished = false;
	_uint m_iHp = 3;
	_float m_fDmgTimeAcc = 0.f;
	_float m_fAttackCoolDown = 0.f;
	_float m_fDeadTimeAcc = 0.f;

private:
	void Pattern_Idle(_float fTimeDelta);
	void Pattern_Attack(_float fTimeDelta);
	void Pattern_Find_Player(_float fTimeDelta);
	void Pattern_Guard(_float fTimeDelta);
	void Pattern_Damaged(_float fTimeDelta);
	void Pattern_Dead(_float fTimeDelta);


public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_OnTerrain();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


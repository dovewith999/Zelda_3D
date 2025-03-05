#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CCollider_Manager;
class CNavigation;
END

BEGIN(Client)
class CPlayer;
class CItem_Blade;
class CBossMonster final : public CGameObject
{
public:
	enum BOSSSTATE {BOSS_APPEAR, BOSS_WALK, BOSS_ATTACK, BOSS_DEFENCE, BOSS_CHACE, BOSS_GROGGY, BOSS_DEAD, BOSS_END};

private:
	CBossMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBossMonster(const CBossMonster& rhs);
	virtual ~CBossMonster() = default;

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

public:
	void Pattern_Appear(_float fTimeDelta);
	void Pattern_Walk(_float fTimeDelta);
	void Pattern_Attaick(_float fTimeDelta);
	void Pattern_Defence(_float fTimeDelta);
	void Pattern_Groggy(_float fTimeDelta);
	void Pattern_Dead(_float fTimeDelta);

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CCollider*						m_pSPHERE = nullptr;
	CCollider*						m_pOBB_AttackRange = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;

private:
	CPlayer*						m_pPlayer = nullptr;
	CItem_Blade*					m_pPlayer_Blade = nullptr;

private:
	_bool							m_bAttack = false;
	_bool							m_bAnimFinished = false;
	_uint							m_iCombo = 0;
	_float							m_fAttackCoolDown = 0.f;
	_float							m_fTimeAcc = 0.f;
	_float							m_fAnimSpeed = 1.f;

private:
	BOSSSTATE m_eBossState = BOSS_END;

public:
	static CBossMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


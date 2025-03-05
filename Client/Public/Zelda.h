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
class CZelda final : public CGameObject
{
public:
	enum ZELDASTATE { STATE_IDLE, STATE_WALK, STATE_ATTACK, STATE_DAMAGE, STATE_END };

private:
	CZelda(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CZelda(const CZelda& rhs);
	virtual ~CZelda() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_float Get_Distance();

public:
	void Set_Boss(_bool bResult)
	{
		m_bBoss = bResult;
	}

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CCollider*						m_pSPHERE = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;

private:
	CPlayer*						m_pPlayer = nullptr;	

private:
	_bool							m_bMove = false;
	_bool							m_bBoss = false;

private:
	ZELDASTATE m_eState = STATE_END;
	_float m_fTimeAcc = 0.f;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_OnTerrain();
public:
	static CZelda* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


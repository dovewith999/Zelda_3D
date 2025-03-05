#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CCollider;
class CCollider_Manager;
END

BEGIN(Client)
class CPlayer;
class CObject_WaterPot final : public CGameObject
{
public:
	enum BOMBSTATE { STATE_IDLE, STATE_GRAB, STATE_THROW, STATE_DEAD, STATE_END };

private:
	CObject_WaterPot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_WaterPot(const CObject_WaterPot& rhs);
	virtual ~CObject_WaterPot() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	void Move_GrapPoint(_float fTimeDelta);
	void Throw(_float fTimeDelta);
	HRESULT SetUp_OnTerrain();

private:
	_float m_fTimeAcc = 0.f;
	_float m_fBoomTimeAcc = 0.f;
	_float	m_fJumpTimeAcc = 0.f;
	_float	m_fJumpPower = 0.f;
	_float m_fCurrentPosY = 0.f;
	BOMBSTATE m_eState = STATE_END;


private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CPlayer*						m_pPlayer = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;


public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_WaterPot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& pTransform);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END



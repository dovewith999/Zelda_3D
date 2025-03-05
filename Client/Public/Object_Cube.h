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
class CObject_Cube final : public CGameObject
{
private:
	CObject_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_Cube(const CObject_Cube& rhs);
	virtual ~CObject_Cube() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	_bool Check_Front();

private:
	_bool m_bCollision = false;
	_bool m_bUnderCollision = false;
	_float m_fFirstX = 0.f;
	_float m_fFirstZ = 0.f;
	_float m_fTimeAcc = 0.f;
	_float m_fCellY = 0.f;
	_float m_fJumpTimeAcc = 0.f;
	_float m_fJumpFallTime = 0.f;
	_float	m_fJumpPower = 4.f;

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CPlayer*						m_pPlayer = nullptr;
	CTransform*						m_pPlayer_Transform = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_OnTerrain();

public:
	static CObject_Cube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END






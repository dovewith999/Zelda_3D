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
class CObject_Door final : public CGameObject
{
private:
	CObject_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_Door(const CObject_Door& rhs);
	virtual ~CObject_Door() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_float Get_Distance() const;

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CPlayer*						m_pPlayer = nullptr;

private:
	_bool m_bLock = false; 
	_bool m_bOpen = false;
	_float m_fTimeAcc = 0.f;
	_float m_fOpen = 0.f;
	_float m_fOpenTimeAcc = 0.f;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END






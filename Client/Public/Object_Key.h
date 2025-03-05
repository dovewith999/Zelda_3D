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
class CObject_Key final : public CGameObject
{
	
private:
	CObject_Key(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_Key(const CObject_Key& rhs);
	virtual ~CObject_Key() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pAABB = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CPlayer*						m_pPlayer = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_Key* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END






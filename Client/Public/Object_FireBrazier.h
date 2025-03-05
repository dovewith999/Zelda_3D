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
class CObject_FireBrazier final : public CGameObject
{
private:
	CObject_FireBrazier(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_FireBrazier(const CObject_FireBrazier& rhs);
	virtual ~CObject_FireBrazier() override = default;

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
	CCollider*						m_pSPHERE = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;

private:
	_float m_fTimeAcc = 0.f;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_FireBrazier* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& pTransform);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override; 
};
END


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
class CItem_Blade;
class CObject_WoodPlanks final : public CGameObject
{
private:
	CObject_WoodPlanks(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_WoodPlanks(const CObject_WoodPlanks& rhs);
	virtual ~CObject_WoodPlanks() override = default;

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
	CPlayer*						m_pPlayer = nullptr;
	CCollider*						m_pOBB = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CItem_Blade*					m_pItem_Blade = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_WoodPlanks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& pTransform);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END




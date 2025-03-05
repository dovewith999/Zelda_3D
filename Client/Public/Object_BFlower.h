#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)
class CObject_BFlower final : public CGameObject
{
private:
	CObject_BFlower(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_BFlower(const CObject_BFlower& rhs);
	virtual ~CObject_BFlower() override = default;

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


public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_BFlower* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& pTransform);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
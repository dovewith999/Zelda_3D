#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CZelda;
class CTrigger_Boss final : public CGameObject
{

private:
	explicit CTrigger_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTrigger_Boss(const CTrigger_Boss& rhs);
	virtual ~CTrigger_Boss() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_Pos(_float4 vPos)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	}

private: /* Component */
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;

private:/*포인터 변수들*/
	CZelda* m_pZelda = nullptr;

private:
	_float m_fTimeAcc = 0.f;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CTrigger_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


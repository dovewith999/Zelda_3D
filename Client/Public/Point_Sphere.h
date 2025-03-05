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
class CPoint_Sphere final : public CGameObject
{

private:
	explicit CPoint_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPoint_Sphere(const CPoint_Sphere& rhs);
	virtual ~CPoint_Sphere() override = default;

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

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pSPHERE = nullptr;


private:

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CPoint_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

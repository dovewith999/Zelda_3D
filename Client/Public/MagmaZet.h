#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
class CCollider_Manager;
END

BEGIN(Client)
class CMagmaZet final : public CGameObject
{
public:
	CMagmaZet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMagmaZet(const CMagmaZet& rhs);
	virtual ~CMagmaZet() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC &pTransformDesc) override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CCollider* m_pOBB = nullptr;
	CCollider_Manager* m_pCollider_Manager = nullptr;

private:
	_float m_fUpDownAcc = 26.f;
	_float m_fTimeAcc = 0.f;
	_bool m_bUp = false;
	_bool m_bDown = true;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CMagmaZet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC &pTransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


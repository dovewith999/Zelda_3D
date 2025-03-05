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
class CObject_MagRock final : public CGameObject
{
private:
	CObject_MagRock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_MagRock(const CObject_MagRock& rhs);
	virtual ~CObject_MagRock() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_float Get_Distance();

private: /* Component */
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;

private:
	CPlayer*						m_pPlayer = nullptr;

private:
	_float m_fUpDown = 0.f;
	_float m_fFallTimeAcc = 0.f;
public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_MagRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& pTransform);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


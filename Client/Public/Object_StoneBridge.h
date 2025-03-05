#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)
class CPlayer;
class CObject_StoneBridge final : public CGameObject
{
private:
	CObject_StoneBridge(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_StoneBridge(const CObject_StoneBridge& rhs);
	virtual ~CObject_StoneBridge() override = default;

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
	CCollider*						m_pOBB = nullptr;

private:
	CPlayer*						m_pPlayer = nullptr;

private:
	_float m_fFallingCntAcc = 0.f;
	_bool m_bFalling = false;
	_float	m_fJumpPower = 3.f;
	_float m_fFallDistance = 0.f;


private:
	_float Get_Distance();

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_StoneBridge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& pTransform);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END



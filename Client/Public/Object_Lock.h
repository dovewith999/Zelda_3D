#pragma once
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CCollider_Manager;
END

BEGIN(Client)
class CPlayer;

class CObject_Lock final : public CGameObject
{
public:
	enum LOCKSTATE { STATE_IDLE, STATE_UNLOCK, STATE_END };

private:
	CObject_Lock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_Lock(const CObject_Lock& rhs);
	virtual ~CObject_Lock() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_float Get_Distance() const;

private: /* Component */
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CPlayer*						m_pPlayer = nullptr;
	CCollider*						m_pOBB = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;

private:
	LOCKSTATE m_eState = STATE_END;
	_bool m_bIsAnimFinished = false;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_Lock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END






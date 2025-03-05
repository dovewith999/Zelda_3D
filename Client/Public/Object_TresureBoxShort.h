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
class CObject_TresureBoxShort final : public CGameObject
{
	enum STATE {
		STATE_IDLE, STATE_OPEN_SHORT, STATE_OPENIDLE, STATE_END
	};

private:
	CObject_TresureBoxShort(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CObject_TresureBoxShort(const CObject_TresureBoxShort& rhs);
	virtual ~CObject_TresureBoxShort() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_float Get_Instance();

private:
	_float m_fTimeAcc = 0.f;

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CCollider*						m_pTarget_Collider = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	CPlayer*						m_pPlayer = nullptr;

private:
	STATE m_eState = STATE_END;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CObject_TresureBoxShort* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


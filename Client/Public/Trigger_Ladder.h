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
class CPlayer;
class CUI_BlackOut;
class CTrigger_Ladder final : public CGameObject
{

private:
	explicit CTrigger_Ladder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTrigger_Ladder(const CTrigger_Ladder& rhs);
	virtual ~CTrigger_Ladder() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_float Get_Distance() const;

public:
	void Set_Pos(_float4 vPos)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	}

public:
	void Light_Off();

private: /* Component */
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pOBB = nullptr;

private:/*포인터 변수들*/
	CPlayer* m_pPlayer = nullptr;
	CUI_BlackOut* m_pUI_BlackOut = nullptr;

private: //암전 테스트용 변수
	_float vColor = 1.f;
	_bool m_bLightOff = false;

private:
	_float m_fTimeAcc = 0.f;
	_float m_fPlayerTimeAcc = 0.f;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CTrigger_Ladder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


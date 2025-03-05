#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CShaderl;
class CRenderer;
class CVIBuffer_Rect;
class CNavigation;
END

BEGIN(Client)
class CNavigation_Assist final : public CGameObject
{
public:
	CNavigation_Assist(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNavigation_Assist(const CNavigation_Assist& rhs);
	virtual ~CNavigation_Assist() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTexture*						m_pTextureCom =  nullptr ;
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CVIBuffer_Rect*					m_pVIBufferCom = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;

private:
	_uint					m_iPassIndex = 0;

private:
	_float m_fTimeAcc = 0.f;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CNavigation_Assist* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


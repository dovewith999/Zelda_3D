#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI_LogoBackGround final : public CUIObject
{
protected:
	explicit CUI_LogoBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_LogoBackGround(const CUI_LogoBackGround& rhs);
	virtual ~CUI_LogoBackGround() override = default;


public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc) override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

private:
	CTexture*						m_pTextureCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CVIBuffer_Rect*					m_pVIBufferCom = nullptr;

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;

public:
	HRESULT SetUp_Components();

public:
	static CUI_LogoBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


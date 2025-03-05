#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CPlayer;
class CUI_BlackOut final : public CUIObject
{
protected:
	explicit CUI_BlackOut(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_BlackOut(const CUI_BlackOut& rhs);
	virtual ~CUI_BlackOut() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& tagTransform) override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

public:
	_float Get_Alpha()
	{
		return m_fAlpha;
	}

public:
	void Set_Alpha(_float fSumAlpha)
	{
		m_fAlpha += fSumAlpha;
	}

private:
	CTexture*						m_pTextureCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CVIBuffer_Rect*					m_pVIBufferCom = nullptr;

private:
	_float				m_fSizeX, m_fSizeY;
	_float2				m_vPos;
	_float4x4			m_ProjMatrix;
	_float				m_fAlpha = 1.f;

private:
	_bool m_bIsAttention = false;
	CPlayer* m_pPlayer = nullptr;

public:
	HRESULT SetUp_Components();

public:
	static CUI_BlackOut* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


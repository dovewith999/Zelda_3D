#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END
BEGIN(Client)

class CUI_PlayerHp final : public CUIObject
{
protected:
	explicit CUI_PlayerHp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_PlayerHp(const CUI_PlayerHp& rhs);
	virtual ~CUI_PlayerHp() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& tagTransform) override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

public:
	void Set_TextureNum(_uint iTextureNum)
	{
		m_iTextureNum = iTextureNum;
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

private:
	_uint m_iTextureNum = 3;

public:
	HRESULT SetUp_Components();

public:
	static CUI_PlayerHp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


#include "stdafx.h"
#include "..\Public\UI_TitleLogo.h"
#include "GameInstance.h"

CUI_TitleLogo::CUI_TitleLogo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUIObject(pDevice, pDeviceContext)
{
}

CUI_TitleLogo::CUI_TitleLogo(const CUI_TitleLogo & rhs)
	: CUIObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CUI_TitleLogo::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_TitleLogo::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 400.f;
	m_fSizeY = 100.f;
	m_fX = 750.f;
	m_fY = 380.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));

	return S_OK;
}

void CUI_TitleLogo::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (2.f <= m_fTimeAcc)
	{
		m_fBarFront += 0.1f;

		if (0.2f < m_fBarFront)
		{
			m_fBarBack += 0.1f;
		}

		if (3.f <= m_fBarBack)
		{
			m_fTimeAcc = 0.f;
			m_fBarFront = 0.f;
			m_fBarBack = 0.f;
		}
	}
}

void CUI_TitleLogo::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pTransformCom->Set_Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - (g_iWinCX >> 1), -m_fY + (g_iWinCY >> 1), 0.f, 1.f));

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_UI, this);
}

HRESULT CUI_TitleLogo::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_fBarFront", &m_fBarFront, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fBarBack", &m_fBarBack, sizeof(_float));

	if (FAILED(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_TitleLogo::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_TitleLogo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CUI_TitleLogo * CUI_TitleLogo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CUI_TitleLogo* pInstance = new CUI_TitleLogo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Error : Failed Create UI_TitleLogo"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_TitleLogo::Clone(void * pArg)
{
	CUI_TitleLogo* pInstance = new CUI_TitleLogo(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Error : Failed Cloned UI_TitleLogo"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_TitleLogo::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}

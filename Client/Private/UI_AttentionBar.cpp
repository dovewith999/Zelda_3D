#include "stdafx.h"
#include "..\Public\UI_AttentionBar.h"
#include "GameInstance.h"
#include "Player.h"

CUI_AttentionBar::CUI_AttentionBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUIObject(pDevice, pDeviceContext)
{
}

CUI_AttentionBar::CUI_AttentionBar(const CUI_AttentionBar & rhs)
	: CUIObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CUI_AttentionBar::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AttentionBar::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinCX;
	m_fSizeY = 50.f;
	if (nullptr == pArg)
	{
		m_vPos.x = g_iWinCX >> 1;
		m_vPos.y = 250.f;
	}
	else
	{
		m_vPos = *(_float2*)pArg;
	}
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f)));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	if (static_cast<_uint>(LEVEL_DUNGEON) == pGameInstance->Get_NextLevelID())
	{
		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, L"Layer_Player"));
	}

	else if (static_cast<_uint>(LEVEL_DUNGEON_FINAL) == pGameInstance->Get_NextLevelID())
	{
		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON_FINAL, L"Layer_Player"));
	}

	Safe_AddRef(m_pPlayer);

	Safe_Release(pGameInstance);
	return S_OK;
}

void CUI_AttentionBar::Tick(_float fTimeDelta)
{
	m_bIsAttention = m_pPlayer->Get_Attention();

	if (m_bIsAttention)
	{
		if (25.f > m_vPos.y)
		{
			m_vPos.y += 5.f;
		}
		else if (695.f < m_vPos.y)
		{
			m_vPos.y -= 5.f;
		}
	}

	else
	{
		if (25.f >= m_vPos.y && -25.f < m_vPos.y)
		{
			m_vPos.y -= 5.f;
		}

		else if (695.f <= m_vPos.y && 745.f > m_vPos.y)
		{
			m_vPos.y += 5.f;
		}
	}
}

void CUI_AttentionBar::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pTransformCom->Set_Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.x - (g_iWinCX >> 1), -m_vPos.y + (g_iWinCY >> 1), 0.f, 1.f));

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_UI, this);
}

HRESULT CUI_AttentionBar::Render()
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

	if (FAILED(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_AttentionBar::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Texture_UI_AttentionBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CUI_AttentionBar * CUI_AttentionBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CUI_AttentionBar* pInstance = new CUI_AttentionBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Error : Failed Create UI_AttentionBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_AttentionBar::Clone(void * pArg)
{
	CUI_AttentionBar* pInstance = new CUI_AttentionBar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Error : Failed Cloned UI_AttentionBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_AttentionBar::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
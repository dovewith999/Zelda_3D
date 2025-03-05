#include "stdafx.h"
#include "..\Public\Navigation_Assist.h"
#include "GameInstance.h"

CNavigation_Assist::CNavigation_Assist(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CNavigation_Assist::CNavigation_Assist(const CNavigation_Assist & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNavigation_Assist::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation_Assist::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-400.f, 0.f, -400.f, 1.f));

	return S_OK;
}

void CNavigation_Assist::Tick(_float fTimeDelta)
{
}

void CNavigation_Assist::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	_float4 vPos;

	m_fTimeAcc += fTimeDelta;

	if (0.3f < m_fTimeAcc && GetKeyState(VK_LBUTTON) & 0x8000)
	{
		if (pGameInstance->Picking(m_pVIBufferCom, m_pTransformCom, &vPos))
		{
			if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Point_Sphere"), TEXT("Prototype_GameObject_Point_Sphere"), &vPos)))
			{
				return;
			}
			m_fTimeAcc = 0.f;
		}
	}

	Safe_Release(pGameInstance);
}

HRESULT CNavigation_Assist::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

//#ifdef _DEBUG
//	m_pNavigationCom->Render();
//#endif // _DEBUG

	return S_OK;
}

HRESULT CNavigation_Assist::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Navigation*/
	if (FAILED(__super::Add_Components(LEVEL_TOOL, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation_Assist::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CNavigation_Assist * CNavigation_Assist::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CNavigation_Assist* pInstance = new CNavigation_Assist(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Error : Failed Created Navigation_Assist"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNavigation_Assist::Clone(void * pArg)
{
	CNavigation_Assist* pInstance = new CNavigation_Assist(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Error : Failed Cloned Navigation_Assist"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation_Assist::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}

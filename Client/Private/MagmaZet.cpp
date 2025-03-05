#include "stdafx.h"
#include "..\Public\MagmaZet.h"
#include "GameInstance.h"
#include "Collider_Manager.h"

CMagmaZet::CMagmaZet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMagmaZet::CMagmaZet(const CMagmaZet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMagmaZet::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & pTransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(pTransformDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMagmaZet::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		return E_FAIL;
	}


	if (FAILED(SetUp_Components()))
	{
		return E_FAIL;
	}

	if (nullptr != pArg)
	{

		_matrix WorldMatrix = *(_matrix*)pArg;

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, WorldMatrix.r[0]);
		m_pTransformCom->Set_State(CTransform::STATE_UP, WorldMatrix.r[1]);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, WorldMatrix.r[2]);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMatrix.r[3]);
	}

	return S_OK;
}

void CMagmaZet::Tick(_float fTimeDelta)
{
	m_pTransformCom->Turn(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)), fTimeDelta);

	if (m_bUp)
	{
		if (26.f > m_fUpDownAcc)
		{
			m_fUpDownAcc += 0.5f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.5f);//XMVectorSet(0.f, 0.5, 0.f, 0.f));
		}

		else if (26.f <= m_fUpDownAcc)
		{
			m_fTimeAcc += fTimeDelta;

			if (7.f < m_fTimeAcc)
			{
				m_bDown = true;
				m_bUp = false;
				m_fTimeAcc = 0.f;
			}
		}
	}

	else if(m_bDown)
	{
		if (10.f < m_fUpDownAcc)
		{
			m_fUpDownAcc -= 0.5f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.5f);//XMVectorSet(0.f, 0.5, 0.f, 0.f));
		}
		else if(10.f >= m_fUpDownAcc)
		{
			m_fTimeAcc += fTimeDelta;

			if (7.f < m_fTimeAcc)
			{
				m_bDown = false;
				m_bUp = true;
				m_fTimeAcc = 0.f;
			}
		}
	}

	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMagmaZet::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
	{
		return;
	}
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	if (true == pGameInstance->isIn_FrustumInWorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f));
	{
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
	}
	Safe_Release(pGameInstance);
	m_pCollider_Manager->Add_Collider(CCollider_Manager::OBJECT_MAGZET, m_pOBB);
}

HRESULT CMagmaZet::Render()
{

	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_Material_OnShader(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	//m_pOBB->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CMagmaZet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider_Manager */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Manager"), TEXT("Com_Collider_Manager"), (CComponent**)&m_pCollider_Manager)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_MagmaZet"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale =  _float3(3.f, 15.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, 7.5f, 0.f);

	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	return S_OK;

}

HRESULT CMagmaZet::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionFloat4(), sizeof(_float4));

	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMagmaZet * CMagmaZet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & pTransformDesc)
{
	CMagmaZet* pInstance = new CMagmaZet(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pTransformDesc)))
	{
		MSGBOX(TEXT("Error : Failed Created CMagmaZet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMagmaZet::Clone(void * pArg)
{
	CMagmaZet* pInstance = new CMagmaZet(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Error : Failed Cloned CMagmaZet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagmaZet::Free()
{
	__super::Free();

	Safe_Release(m_pOBB);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderCom);

}

#include "stdafx.h"
#include "..\Public\Item_Sheath.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Model.h"

CItem_Sheath::CItem_Sheath(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CItem_Sheath::CItem_Sheath(const CItem_Sheath & rhs)
	: CGameObject(rhs)

{

}

HRESULT CItem_Sheath::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Sheath::NativeConstruct(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	memcpy(&m_ItemDesc, pArg, sizeof(ITEMDESC));



	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(SetUp_BonePtr()))
		return E_FAIL;

	
	m_pTransformCom->Rotation(XMVectorSet(-1.f, -1.f, 0.f, 0.f), XMConvertToRadians(180.0f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	m_pTransformCom->Set_Scaled(XMVectorSet(0.004f, 0.004f, 0.004f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CItem_Sheath::Tick(_float fTimeDelta)
{
}

void CItem_Sheath::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CItem_Sheath::Render()
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

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CItem_Sheath::SetUp_BonePtr()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CModel*		pTargetModelCom = dynamic_cast<CModel*>(pGameInstance->Get_ComponentPtr(m_ItemDesc.eLevelID, m_ItemDesc.pLayerTag, m_ItemDesc.pComTag, m_ItemDesc.iIndex));
	if (nullptr == pTargetModelCom)
		return E_FAIL;

	m_PivotMatrix = pTargetModelCom->Get_PivotMatrix();

	m_pBone = pTargetModelCom->Find_HierarchyNode(m_ItemDesc.pBoneName);
	if (nullptr == m_pBone)
	{
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CItem_Sheath::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Model_Item_Sheath"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Sheath::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		SocketMatrix;

	XMStoreFloat4x4(&SocketMatrix, XMMatrixTranspose(m_pBone->Get_OffSetMatrix()) * m_pBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_PivotMatrix));

	XMStoreFloat3((_float3*)&SocketMatrix.m[0], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[0])));
	XMStoreFloat3((_float3*)&SocketMatrix.m[1], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[1])));
	XMStoreFloat3((_float3*)&SocketMatrix.m[2], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[2])));

	CTransform*		pTargetTransform = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(m_ItemDesc.eLevelID, m_ItemDesc.pLayerTag, CGameObject::m_pTransformTag, m_ItemDesc.iIndex));
	if (nullptr == pTargetTransform)
	{
		return E_FAIL;
	}


	XMStoreFloat4x4(&SocketMatrix, XMMatrixTranspose(XMLoadFloat4x4(&SocketMatrix) * pTargetTransform->Get_WorldMatrix()));

	m_pShaderCom->Set_RawValue("g_SocketMatrix", &SocketMatrix, sizeof(_float4x4));

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


CItem_Sheath * CItem_Sheath::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CItem_Sheath*		pInstance = new CItem_Sheath(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CItem_Sheath"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CItem_Sheath::Clone(void * pArg)
{
	CItem_Sheath*		pInstance = new CItem_Sheath(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CItem_Sheath"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_Sheath::Free()
{
	__super::Free();



	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

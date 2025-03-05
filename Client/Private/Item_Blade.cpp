#include "stdafx.h"
#include "..\Public\Item_Blade.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Model.h"
#include "Player.h"

CItem_Blade::CItem_Blade(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CItem_Blade::CItem_Blade(const CItem_Blade & rhs)
	: CGameObject(rhs)

{

}

HRESULT CItem_Blade::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Blade::NativeConstruct(void * pArg)
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

	//m_pTransformCom->Set_Scaled(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.0f));
	m_pTransformCom->Set_Scaled(XMVectorSet(0.008f, 0.008f, 0.008f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.0f, 0.f, 0.f, 1.f));

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

void CItem_Blade::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scaled(XMVectorSet(0.008f, 0.008f, 0.008f, 1.f));

	if (!m_pPlayer->Get_ItemState())
	{
		m_ItemDesc.pBoneName = "cl_cl_podA                     ";

		SetUp_BonePtr();
	}
	else
	{
		m_ItemDesc.pBoneName = "cl_cl_LhandA                   ";
		SetUp_BonePtr();
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		SocketMatrix;

	XMStoreFloat4x4(&SocketMatrix, m_pBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_PivotMatrix));

	XMStoreFloat3((_float3*)&SocketMatrix.m[0], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[0])));
	XMStoreFloat3((_float3*)&SocketMatrix.m[1], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[1])));
	XMStoreFloat3((_float3*)&SocketMatrix.m[2], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[2])));

	CTransform*		pTargetTransform = (CTransform*)pGameInstance->Get_ComponentPtr(m_ItemDesc.eLevelID, m_ItemDesc.pLayerTag, CGameObject::m_pTransformTag, m_ItemDesc.iIndex);
	if (nullptr == pTargetTransform)
		return;


	m_pOBB->Update(XMLoadFloat4x4(&SocketMatrix) * pTargetTransform->Get_WorldMatrix());

	RELEASE_INSTANCE(CGameInstance);

}

void CItem_Blade::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_pPlayer->Get_ItemState())
	{
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CItem_Blade::Render()
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

#ifdef _DEBUG
	//m_pOBB->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CItem_Blade::SetUp_BonePtr()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CModel*		pTargetModelCom = (CModel*)pGameInstance->Get_ComponentPtr(m_ItemDesc.eLevelID, m_ItemDesc.pLayerTag, m_ItemDesc.pComTag, m_ItemDesc.iIndex);
	if (nullptr == pTargetModelCom)
		return E_FAIL;

	m_PivotMatrix = pTargetModelCom->Get_PivotMatrix();

	m_pBone = pTargetModelCom->Find_HierarchyNode(m_ItemDesc.pBoneName);
	if (nullptr == m_pBone)
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CItem_Blade::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Model_Item_Blade"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_AABB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(0.1f, 1.f, 0.1f);
	ColliderDesc.vPosition = _float3(0.025f, 0.5f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, -0.8f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Blade::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		SocketMatrix;

	XMStoreFloat4x4(&SocketMatrix, XMMatrixTranspose(m_pBone->Get_OffSetMatrix()) * m_pBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_PivotMatrix));

	XMStoreFloat3((_float3*)&SocketMatrix.m[0], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[0])));
	XMStoreFloat3((_float3*)&SocketMatrix.m[1], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[1])));
	XMStoreFloat3((_float3*)&SocketMatrix.m[2], XMVector3Normalize(XMLoadFloat3((_float3*)&SocketMatrix.m[2])));

	CTransform*		pTargetTransform = (CTransform*)pGameInstance->Get_ComponentPtr(m_ItemDesc.eLevelID, m_ItemDesc.pLayerTag, CGameObject::m_pTransformTag, m_ItemDesc.iIndex);
	if (nullptr == pTargetTransform)
		return E_FAIL;


	XMStoreFloat4x4(&SocketMatrix, XMMatrixTranspose(XMLoadFloat4x4(&SocketMatrix) * pTargetTransform->Get_WorldMatrix()));

	// SocketMatrix = 뼈의 오프셋 * 뼈의 컴바인드 * 모델의 피벗 * 모델을가진 객체의 월드 
	// SocketMatrix 현재 이 스워드 객체의 부모행렬로 사용하였다. (vPosition * g_WorldMatrix * g_SocketMatrix * g_View * g_Proj)

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


CItem_Blade * CItem_Blade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CItem_Blade*		pInstance = new CItem_Blade(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CItem_Blade"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CItem_Blade::Clone(void * pArg)
{
	CItem_Blade*		pInstance = new CItem_Blade(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CItem_Blade"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_Blade::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pOBB);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}


#include "stdafx.h"
#include "..\Public\Trigger_Boss.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Model.h"
#include "Zelda.h"

CTrigger_Boss::CTrigger_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTrigger_Boss::CTrigger_Boss(const CTrigger_Boss & rhs)
	: CGameObject(rhs)

{

}

HRESULT CTrigger_Boss::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger_Boss::NativeConstruct(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	if (nullptr != pArg)
	{
		_matrix WorldMatrix = *(_matrix*)pArg;

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, WorldMatrix.r[0]);
		m_pTransformCom->Set_State(CTransform::STATE_UP, WorldMatrix.r[1]);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, WorldMatrix.r[2]);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMatrix.r[3]);
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pZelda = dynamic_cast<CZelda*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_NPC")));

	Safe_AddRef(m_pZelda);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CTrigger_Boss::Tick(_float fTimeDelta)
{
	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
}

void CTrigger_Boss::Late_Tick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (0 == m_fTimeAcc)
	{
		if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON_FINAL, TEXT("Layer_NPC"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
		{
			if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON_FINAL, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_BossMonster"))))
				return;
			m_pZelda->Set_Boss(true);
			m_fTimeAcc += fTimeDelta;
			m_bRelease = true;
		
		}
	}
	
	Safe_Release(pGameInstance);
}

HRESULT CTrigger_Boss::Render()
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

		m_pShaderCom->Begin(0);

		//m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pOBB->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CTrigger_Boss::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Item_Bow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_AABB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger_Boss::SetUp_ConstantTable()
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


CTrigger_Boss * CTrigger_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CTrigger_Boss*		pInstance = new CTrigger_Boss(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CTrigger_Boss"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTrigger_Boss::Clone(void * pArg)
{
	CTrigger_Boss*		pInstance = new CTrigger_Boss(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CTrigger_Boss"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrigger_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pZelda);
	Safe_Release(m_pOBB);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
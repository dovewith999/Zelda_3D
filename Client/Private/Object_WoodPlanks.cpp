#include "stdafx.h"
#include "..\Public\Object_WoodPlanks.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item_Blade.h"

CObject_WoodPlanks::CObject_WoodPlanks(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CObject_WoodPlanks::CObject_WoodPlanks(const CObject_WoodPlanks & rhs)
	: CGameObject(rhs)
{

}

HRESULT CObject_WoodPlanks::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_WoodPlanks::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		//_vector vPos = *(_vector*)pArg;

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		_matrix WorldMatrix = *(_matrix*)pArg;

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, WorldMatrix.r[0]);
		m_pTransformCom->Set_State(CTransform::STATE_UP, WorldMatrix.r[1]);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, WorldMatrix.r[2]);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMatrix.r[3]);
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_Player")));
	Safe_AddRef(m_pPlayer);

	m_pItem_Blade = dynamic_cast<CItem_Blade*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_Item_Blade")));
	Safe_AddRef(m_pItem_Blade);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CObject_WoodPlanks::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scaled(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());

	//if (m_pOBB->Get_Collision())
	//{
	//	/* For.Com_Model*/
	//	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_WoodPlanks_Break"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//		return;
	//}
	
	if (m_pOBB->Get_Collision_OBB())
	{
		m_bRelease = true;
	}
}

void CObject_WoodPlanks::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
	{
		m_pPlayer->Check_Front(static_cast<_double>(fTimeDelta), true, 0.f, 0.f);
	}

	if (CPlayer::PLAYERSTATE::PLAYER_ATTACK == m_pPlayer->Get_State() || CPlayer::PLAYERSTATE::PLAYER_ATTACK_FINAL == m_pPlayer->Get_State())
	{
		if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Item_Blade"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
		{
			m_bRelease = true;

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_WoodPlanks_Broken"), &m_pTransformCom->Get_WorldMatrix())))
			{
				return;
			}

			pGameInstance->PlaySoundW(L"WoodPlanks.wav", SOUND_EFFECT, 0.2f);

			Safe_Release(pGameInstance);
		}
	}

	Safe_Release(pGameInstance);
}

HRESULT CObject_WoodPlanks::Render()
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

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	//m_pOBB->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CObject_WoodPlanks::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_WoodPlanks"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(2.f, 1.5f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_WoodPlanks::SetUp_ConstantTable()
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


CObject_WoodPlanks * CObject_WoodPlanks::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CObject_WoodPlanks*		pInstance = new CObject_WoodPlanks(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CObject_WoodPlanks"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CObject_WoodPlanks::Clone(void * pArg)
{
	CObject_WoodPlanks*		pInstance = new CObject_WoodPlanks(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CObject_WoodPlanks"));
		Safe_Release(pInstance);
	}
	return pInstance;

}

void CObject_WoodPlanks::Free()
{
	__super::Free();

	Safe_Release(m_pItem_Blade);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pOBB);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
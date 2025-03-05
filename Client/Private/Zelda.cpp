#include "stdafx.h"
#include "..\Public\Zelda.h"
#include "GameInstance.h"
#include "Player.h"

CZelda::CZelda(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CZelda::CZelda(const CZelda & rhs)
	: CGameObject(rhs)
{

}

HRESULT CZelda::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CZelda::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-15.4f, 14.f, 32.39f, 1.f));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Player")));

	Safe_AddRef(m_pPlayer);

	RELEASE_INSTANCE(CGameInstance);

	m_pModelCom->SetUp_Animation(6);

	return S_OK;
}

void CZelda::Tick(_float fTimeDelta)
{

	if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}

	else if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}

	else if (GetKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}

	if (FAILED(SetUp_OnTerrain()))
	{
		return;
	}

	if (m_bMove)
	{
		m_pTransformCom->LookAt(m_pPlayer->Get_Position());


		if (3.f < Get_Distance())
		{
			m_pTransformCom->Go_Straight(static_cast<_double>(fTimeDelta), m_pNavigationCom);		
			m_pModelCom->SetUp_Animation(10);
		}

		else
		{
			m_pModelCom->SetUp_Animation(6);
		}
	}

	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSPHERE->Update(m_pTransformCom->Get_WorldMatrix());

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CZelda::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_SPHERE"))), CCollider::TYPE_SPHERE))
	//{
	//	m_pCollider_Manager->Add_CollisionObject(CCollider_Manager::OBJECT_BOSS, this);
	//}

	if (3.f > Get_Distance())
	{
		if (!m_bMove)
		{
			pGameInstance->PlaySoundW(L"Final.wav", SOUND_MONSTER, 0.2f);
			m_bMove = true;
		}
	}

	if (m_bMove)
	{
		if (2.f >= m_fTimeAcc)
		{
			m_fTimeAcc += fTimeDelta;
		}

		else if (2.f < m_fTimeAcc)
		{
			if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON_FINAL, TEXT("Layer_UI_End"), TEXT("Prototype_GameObject_UI_End"))))
			{
				return;
			}
		}
	}

	Safe_Release(pGameInstance);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CZelda::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_BoneMatrices_OnShader(m_pShaderCom, "g_Bones", i);

		m_pModelCom->SetUp_Material_OnShader(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	//m_pOBB->Render();
	//m_pSPHERE->Render();
	//m_pNavigationCom->Render();
#endif // _DEBUG


	return S_OK;
}

_float CZelda::Get_Distance()
{
	return XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
}

HRESULT CZelda::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider_Manager */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Manager"), TEXT("Com_Collider_Manager"), (CComponent**)&m_pCollider_Manager)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Zelda"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDER_DESC ColliderDesc;

	/* For.Com_Collider_OBB*/
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;


	/* For.Com_Collider_SPHERE*/
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(8.f, 8.f, 8.f);
	ColliderDesc.vPosition = _float3(0.f, 0.0f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"), (CComponent**)&m_pSPHERE, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation*/
	CNavigation::NAVIGATIONDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 74;

	if (FAILED(__super::Add_Components(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Navigation_Dungeon_Final"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CZelda::SetUp_ConstantTable()
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


HRESULT CZelda::SetUp_OnTerrain()
{
	_vector vPosition = XMVectorSetW(m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f);

	_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(vPosition));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;	
}


CZelda * CZelda::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CZelda*		pInstance = new CZelda(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CZelda"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CZelda::Clone(void * pArg)
{
	CZelda*		pInstance = new CZelda(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CZelda"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CZelda::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pSPHERE);
	Safe_Release(m_pOBB);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}


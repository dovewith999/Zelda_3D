#include "stdafx.h"
#include "..\Public\Object_Door.h"
#include "GameInstance.h"
#include "Player.h"

CObject_Door::CObject_Door(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CObject_Door::CObject_Door(const CObject_Door & rhs)
	: CGameObject(rhs)
{

}

HRESULT CObject_Door::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Door::NativeConstruct(void * pArg)
{
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

	if (LEVEL_DUNGEON == pGameInstance->Get_NextLevelID())
	{
		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_Player")));
		Safe_AddRef(m_pPlayer);
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CObject_Door::Tick(_float fTimeDelta)
{
	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());

	//m_pTransformCom->Set_Scaled(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	if (m_bOpen && 2.8f > m_fOpen)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.03f, 0.f, 0.f));
		m_fOpen += 0.03f;
	}

	else if (!m_bOpen && 0.f < m_fOpen)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMVectorSet(0.f, 0.03f, 0.f, 0.f));
		m_fOpen -= 0.03f;
	}

	if (2.8f <= m_fOpen)
	{
		m_fOpenTimeAcc += fTimeDelta;
	}

}

void CObject_Door::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->isIn_FrustumInWorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
	}

	m_pCollider_Manager->Add_Collider(CCollider_Manager::OBJECT_DOOR, m_pOBB);

	if (m_pOBB->Get_Collision())
	{
		m_bLock = true;
	}

	else
	{
		m_bLock = false;
	}

	if (LEVEL_DUNGEON == pGameInstance->Get_NextLevelID())
	{
		if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
		{

			m_pPlayer->Check_Front(static_cast<_double>(fTimeDelta), true, 0.f, 0.f);

			if (!m_bLock)
			{
				if (!m_bOpen)
				{
					CGameInstance* pGameInstance = CGameInstance::Get_Instance();
					Safe_AddRef(pGameInstance);

					pGameInstance->PlaySoundW(L"Door_Open.wav", SOUND_EFFECT, 0.2f);

					Safe_Release(pGameInstance);
					m_bOpen = true;
				}

				if (0.f == m_fTimeAcc && 0.f != m_pPlayer->Get_CoolDown())
				{
					m_pPlayer->Set_CoolDown();
					m_fTimeAcc += fTimeDelta;
				}
			}
		}

		else
		{
		}

		if (5.f < m_fOpenTimeAcc)
		{
			m_bOpen = false;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CObject_Door::Render()
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

_float CObject_Door::Get_Distance() const
{
	return _float();
}

HRESULT CObject_Door::SetUp_Components()
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

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (LEVEL_DUNGEON == pGameInstance->Get_NextLevelID())
	{
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Door"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	else if (LEVEL_DUNGEON_FINAL == pGameInstance->Get_NextLevelID())
	{
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Object_Door"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	else if (LEVEL_TOOL == pGameInstance->Get_NextLevelID())
	{
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_Door"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	
	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CObject_Door::SetUp_ConstantTable()
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


CObject_Door * CObject_Door::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CObject_Door*		pInstance = new CObject_Door(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CObject_Door"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CObject_Door::Clone(void * pArg)
{
	CObject_Door*		pInstance = new CObject_Door(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CObject_Door"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Door::Free()
{
	__super::Free();

	if (nullptr != m_pPlayer)
	{
		Safe_Release(m_pPlayer);
	}

	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pOBB);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
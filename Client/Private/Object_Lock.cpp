#include "stdafx.h"
#include "..\Public\Object_Lock.h"
#include "GameInstance.h"
#include "Player.h"
#include "Collider.h"
#include "Collider_Manager.h"

CObject_Lock::CObject_Lock(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CObject_Lock::CObject_Lock(const CObject_Lock & rhs)
	: CGameObject(rhs)
{

}

HRESULT CObject_Lock::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Lock::NativeConstruct(void * pArg)
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

	m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_Player")));

	Safe_Release(pGameInstance);

	m_pModelCom->SetUp_Animation(0);

	return S_OK;
}

void CObject_Lock::Tick(_float fTimeDelta)
{

	switch (m_eState)
	{
	case STATE_IDLE:
		m_pModelCom->SetUp_Animation(0);
		break;
	case STATE_UNLOCK:
		m_pModelCom->SetUp_Animation(1);
		m_bIsAnimFinished = m_pModelCom->Get_AnimationState();	
		break;
	default:
		break;
	}

	m_pModelCom->Play_Animation_Loop(fTimeDelta);

	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
}

void CObject_Lock::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (true == pGameInstance->isIn_FrustumInWorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
	}

	if (!m_bIsAnimFinished)
	{
		if (3.f > Get_Distance())
		{
			m_pCollider_Manager->Add_Collider(CCollider_Manager::OBJECT_LOCK, m_pOBB);
		}
	}
	
	if (!m_bIsAnimFinished)
	{
		if (3.f > Get_Distance())
		{
			if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
			{
				if (0 < m_pPlayer->Get_NumKey())
				{
					m_eState = STATE_UNLOCK;
					m_pPlayer->Set_NumKey(-1);
				}
			}
		}
	}

	Safe_Release(pGameInstance);



}

HRESULT CObject_Lock::Render()
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
#endif // _DEBUG



	return S_OK;
}

_float CObject_Lock::Get_Distance() const
{
	return XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
}

HRESULT CObject_Lock::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Lock"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Lock::SetUp_ConstantTable()
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


CObject_Lock * CObject_Lock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CObject_Lock*		pInstance = new CObject_Lock(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CObject_Lock"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CObject_Lock::Clone(void * pArg)
{
	CObject_Lock*		pInstance = new CObject_Lock(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CObject_Lock"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Lock::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pOBB);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

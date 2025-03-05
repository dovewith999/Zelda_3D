#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item_Boko.h"
#include "Item_Blade.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{

}

HRESULT CMonster::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::NativeConstruct(void * pArg)
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
	m_pItem_Blade = dynamic_cast<CItem_Blade*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_Item_Blade")));
	Safe_AddRef(m_pItem_Blade);
	Safe_AddRef(m_pPlayer);

	Safe_Release(pGameInstance);

	//SetUp_Weapon();

	//현재 위치한 포지션에 해당하는 셀을 찾는 코드
	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vPos;
	XMStoreFloat3(&vPos, vPosition);

	m_pNavigationCom->Find_RealCell(&vPos);

	m_pModelCom->SetUp_Animation(32);

	m_eState = STATE_IDLE;

	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{

	m_pTransformCom->Set_Scaled(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	m_bAnimFinished = m_pModelCom->Get_AnimationState();

	m_fDmgTimeAcc += fTimeDelta;

	if (STATE_DEAD != m_eState)
	{
		if (FAILED(SetUp_OnTerrain()))
		{
			return;
		}
	}

	if (60 > m_pNavigationCom->Get_NumCurrentCell())
	{

	}

	else
	{
		switch (m_eState)
		{
		case Client::CMonster::STATE_IDLE:
			Pattern_Idle(fTimeDelta);
			break;

		case Client::CMonster::STATE_ATTACK:
			Pattern_Attack(fTimeDelta);
			break;

		case Client::CMonster::STATE_FIND:
			Pattern_Find_Player(fTimeDelta);
			break;

		case Client::CMonster::STATE_GUARD:
			Pattern_Guard(fTimeDelta);
			break;

		case Client::CMonster::STATE_DAMAGED:
			Pattern_Damaged(fTimeDelta);
			break;

		case Client::CMonster::STATE_DEAD:
			Pattern_Dead(fTimeDelta);
			break;
		default:
			break;
		}
	}

	if (STATE_ATTACK != m_eState)
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}

	else
	{
		m_pModelCom->Play_Animation_Loop(fTimeDelta);
	}

	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSPHERE->Update(m_pTransformCom->Get_WorldMatrix());


}

void CMonster::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (10.f >= Get_Distance())
	{
		if (STATE_IDLE == m_eState)
		{
			m_eState = STATE_FIND;
		}
	}

	if (5.f > Get_Distance())
	{

		if (STATE_DEAD != m_eState)
		{
			m_pCollider_Manager->Add_CollisionObject(CCollider_Manager::OBJECT_MONSTER, this);
		}

		if (0.5f < m_fDmgTimeAcc && m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Item_Blade"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
		{
			if (STATE_DEAD != m_eState)
			{
				m_eState = STATE_DAMAGED;
			}
		}
	}

	Safe_Release(pGameInstance);
}

HRESULT CMonster::Render()
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

	/*m_pOBB->Render();
	m_pSPHERE->Render();*/
	//m_pNavigationCom->Render();

#endif // _DEBUG


	return S_OK;
}

_float CMonster::Get_Distance() const
{
	return XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
}

void CMonster::Pattern_Idle(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(39);
}

void CMonster::Pattern_Attack(_float fTimeDelta)
{
	if (ATTACK_END == m_eAttack)
	{
		m_eAttack = ATTACK_READY;
	}

	if (ATTACK_READY == m_eAttack)
	{
		m_pModelCom->SetUp_Animation(1);

		if (m_bAnimFinished)
		{
			m_eAttack = ATTACK_ING;
		}
	}

	else if (ATTACK_ING == m_eAttack)
	{
		m_pModelCom->SetUp_Animation(2);

		if (m_bAnimFinished)
		{
			m_eAttack = ATTACK_FINAL;
		}
	}
	
	else if (ATTACK_FINAL == m_eAttack)
	{
		m_pModelCom->SetUp_Animation(3);
		
		if (m_bAnimFinished)
		{
			m_fAttackCoolDown = 0.f;
			m_eState = STATE_GUARD;
			m_eAttack = ATTACK_END;
			
		}
	}
}

void CMonster::Pattern_Find_Player(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(4);

	if (m_bAnimFinished)
	{
		m_eState = STATE_GUARD;
	}

}

void CMonster::Pattern_Guard(_float fTimeDelta)
{
	m_pTransformCom->LookAt(m_pPlayer->Get_Position());

	if (3.f >= Get_Distance())
	{
		m_fAttackCoolDown += fTimeDelta;

		if (5.f < m_fAttackCoolDown)
		{
			m_eState = STATE_ATTACK;
		}
		else
		{
			m_pModelCom->SetUp_Animation(41); // Side Step
			//_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
			//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			//_vector vDir = m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		}

		//if (0 <= XMVectorGetX(XMVector3Dot(vUp, XMVector3Cross(vDir, vLook))))
		//{
		//	m_pTransformCom->Go_Right(fTimeDelta, m_pNavigationCom);

		//	//m_pTransformCom->Go_Left(fTimeDelta, m_pNavigationCom);
		//}

		//else
		//{
		//	//m_pTransformCom->Go_Right(fTimeDelta, m_pNavigationCom);

		//	m_pTransformCom->Go_Left(fTimeDelta, m_pNavigationCom);
		//}
	}

	else
	{
		m_pModelCom->SetUp_Animation(31);
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}

}

void CMonster::Pattern_Damaged(_float fTimeDelta)
{
	if (0.5f < m_fDmgTimeAcc)
	{
		--m_iHp;
		m_fDmgTimeAcc = 0.f;
		m_pModelCom->SetUp_Animation(8);
	}

	if (m_bAnimFinished)
	{
		m_eState = STATE_GUARD;
		m_fDmgTimeAcc = 0.f;
	}

	if (0 >= m_iHp)
	{
		m_eState = STATE_DEAD;


	}
}

void CMonster::Pattern_Dead(_float fTimeDelta)
{
	m_fDeadTimeAcc += fTimeDelta;
	m_pModelCom->SetUp_Animation(38);
	m_pTransformCom->Go_Axis(static_cast<_double>(fTimeDelta), XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pNavigationCom);
	if (2.f < m_fDeadTimeAcc)
	{
		m_bRelease = true;
	}
	
}

HRESULT CMonster::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Boko"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;
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
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CMonster::SetUp_ConstantTable()
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

HRESULT CMonster::SetUp_OnTerrain()
{
	_vector vPosition = XMVectorSetW(m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f);

	_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(vPosition));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}


CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CMonster*		pInstance = new CMonster(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonster::Clone(void * pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : Monster"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pItem_Blade);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pSPHERE);
	Safe_Release(m_pOBB);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

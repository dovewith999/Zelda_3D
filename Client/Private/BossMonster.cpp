#include "stdafx.h"
#include "..\Public\BossMonster.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item_Blade.h"

CBossMonster::CBossMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBossMonster::CBossMonster(const CBossMonster & rhs)
	: CGameObject(rhs)
{

}

HRESULT CBossMonster::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossMonster::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);
	m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Player")));
	Safe_AddRef(m_pPlayer);

	m_pPlayer_Blade = dynamic_cast<CItem_Blade*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Item_Blade")));
	Safe_AddRef(m_pPlayer_Blade);

	Safe_Release(pGameInstance);
	

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-32.f, 24.f, 28.f, 1.f));

	m_eBossState = BOSS_APPEAR;

	m_pModelCom->SetUp_Animation(30);

	return S_OK;
}

void CBossMonster::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	m_fAttackCoolDown += fTimeDelta;

	m_bAnimFinished = m_pModelCom->Get_AnimationState();

	switch (m_eBossState)
	{
	case BOSS_APPEAR:
		Pattern_Appear(fTimeDelta);
		break;
	case BOSS_WALK:
		Pattern_Walk(fTimeDelta);
		break;
	case BOSS_ATTACK:
		Pattern_Attaick(fTimeDelta);
		break;
	case BOSS_DEFENCE:
		Pattern_Defence(fTimeDelta);
		break;
	case BOSS_GROGGY:
		Pattern_Groggy(fTimeDelta);
		break;
	case BOSS_DEAD:
		Pattern_Dead(fTimeDelta);
		break;
	default:
		break;
	}

	if (m_bAttack)
	{
		m_eBossState = BOSS_ATTACK;

	}

	/*else if (!m_bAttack && BOSS_GROGGY != m_eBossState)
	{
		m_eBossState = BOSS_WALK;

		m_pTransformCom->LookAt(m_pPlayer->Get_Position());

		m_pTransformCom->Go_Straight(fTimeDelta);

		m_pModelCom->SetUp_Animation(52);
	}

	if (BOSS_GROGGY != m_eBossState && GetKeyState('P') & 0x8000)
	{
		m_eBossState = BOSS_GROGGY;
		m_pModelCom->SetUp_Animation(13);
	}

	if (BOSS_GROGGY == m_eBossState && m_bAnimFinished)
	{
		m_pModelCom->SetUp_Animation(14);
	}*/

	m_pModelCom->Play_Animation(fTimeDelta * m_fAnimSpeed);

	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
	m_pOBB_AttackRange->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSPHERE->Update(m_pTransformCom->Get_WorldMatrix());
}

void CBossMonster::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);
	
	if (1.f < m_fAttackCoolDown)
	{
		m_fAttackCoolDown = 0.f;
		m_bAttack = m_pOBB_AttackRange->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON_FINAL, TEXT("Layer_Player"), TEXT("Com_OBB"))), CCollider::TYPE_OBB);
	}

	if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON_FINAL, TEXT("Layer_Player"), TEXT("Com_SPHERE"))), CCollider::TYPE_SPHERE))
	{
		m_pCollider_Manager->Add_CollisionObject(CCollider_Manager::OBJECT_BOSS, this);
	}

	Safe_Release(pGameInstance);

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBossMonster::Render()
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
	m_pOBB->Render();
	m_pOBB_AttackRange->Render();
	//m_pSPHERE->Render();
#endif // _DEBUG

	return S_OK;
}

void CBossMonster::Pattern_Appear(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	_vector vPos;
	vPos = XMVectorSet(-32.f, 14.f, 28.f, 1.f);

	if (16.f < XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{


	}

	else if (14.f == XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		m_pModelCom->SetUp_Animation(29);
		if (m_bAnimFinished)
		{
			m_eBossState = BOSS_WALK;
		}
	}
	
	//else
	//{
	//	m_pModelCom->SetUp_Animation(28);
	//}




	_double Ratio = static_cast<_double>(m_fTimeAcc / 5.f);

	_float3 vCurrentPos;
	XMStoreFloat3(&vCurrentPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_vector vPosition;

	vPosition = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vPos, static_cast<_float>(Ratio));
	vPosition = XMVectorSetW(vPosition, 1);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CBossMonster::Pattern_Walk(_float fTimeDelta)
{

	m_eBossState = BOSS_WALK;

	m_pTransformCom->LookAt(m_pPlayer->Get_Position());

	m_pTransformCom->Go_Straight(fTimeDelta);

	m_pModelCom->SetUp_Animation(52);
	
}

void CBossMonster::Pattern_Attaick(_float fTimeDelta)
{
	
	m_eBossState = BOSS_ATTACK;
	m_pModelCom->SetUp_Animation(m_iCombo);
	m_fAnimSpeed = 1.5f;
	if (m_bAnimFinished)
	{
		m_iCombo++;
	}

	if (2 < m_iCombo)
	{
		m_iCombo = 0;

		m_eBossState = BOSS_WALK;
	}
}

void CBossMonster::Pattern_Defence(_float fTimeDelta)
{
}

void CBossMonster::Pattern_Groggy(_float fTimeDelta)
{
}

void CBossMonster::Pattern_Dead(_float fTimeDelta)
{
}

HRESULT CBossMonster::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Gnd"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(2.f, 3.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 1.5f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Collider_OBB_AttackRange*/
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 2.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB_AttackRange"), (CComponent**)&m_pOBB_AttackRange, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Collider_SPHERE*/
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(7.f, 7.f, 7.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"), (CComponent**)&m_pSPHERE, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation*/
	CNavigation::NAVIGATIONDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Components(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Navigation_Dungeon_Final"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBossMonster::SetUp_ConstantTable()
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


CBossMonster * CBossMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CBossMonster*		pInstance = new CBossMonster(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : BossMonster"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossMonster::Clone(void * pArg)
{
	CBossMonster*		pInstance = new CBossMonster(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : BossMonster"));
		Safe_Release(pInstance);
	} 
	return pInstance;
}

void CBossMonster::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pOBB_AttackRange);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayer_Blade);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pSPHERE);
	Safe_Release(m_pOBB);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}


#include "stdafx.h"
#include "..\Public\Object_Bomb.h"
#include "GameInstance.h"
#include "Player.h"
#include "Collider_Manager.h"
#include "Item_Blade.h"

CObject_Bomb::CObject_Bomb(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CObject_Bomb::CObject_Bomb(const CObject_Bomb & rhs)
	: CGameObject(rhs)
{

}

HRESULT CObject_Bomb::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Bomb::NativeConstruct(void * pArg)
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

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_BFlower"), &WorldMatrix)))
		{
			return E_FAIL;
		}

		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_Player")));
		Safe_AddRef(m_pPlayer);

		m_pItem_Blade = dynamic_cast<CItem_Blade*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_Item_Blade")));
		Safe_AddRef(m_pItem_Blade);

		Safe_Release(pGameInstance);
	}

	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vPos;
	XMStoreFloat3(&vPos, vPosition);

	m_pNavigationCom->Find_RealCell(&vPos);

	m_eState = STATE_IDLE;

	return S_OK;
}

void CObject_Bomb::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scaled(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	switch (m_eState)
	{
	case Client::CObject_Bomb::STATE_IDLE:
		//if (FAILED(SetUp_OnTerrain()))
		//{
		//	return;
		//}
		if (STATE_THROW == m_ePreState)
		{
			//m_fBoomTimeAcc += fTimeDelta;

			//if (3.f < m_fBoomTimeAcc)
			//{
			//	m_pCollider_Manager->Add_Collider(CCollider_Manager::OBJECT_BOMB, m_pSPHERE);
			//	m_bRelease = true;
			//}

			m_eState = STATE_WARNING;

		}
		break;
	case Client::CObject_Bomb::STATE_GRAB:
		if (0.5f >= m_fTimeAcc)
		{
			Move_GrapPoint(fTimeDelta);

		}

		else
		{
			_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


			_float3 vPos;
			XMStoreFloat3(&vPos, vPosition);

			if (m_pNavigationCom->Move_OnNavigation(&vPos))
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayer->Get_GrapPoint());
			}

		}
		break;
	case Client::CObject_Bomb::STATE_THROW:
		Throw(fTimeDelta);
		break;
	case STATE_WARNING:
		Warning(fTimeDelta);
		break;
	default:
		break;
	}

	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSPHERE->Update(m_pTransformCom->Get_WorldMatrix());
}

void CObject_Bomb::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (true == pGameInstance->isIn_FrustumInWorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
	}


	if (2.f > Get_Instance())
	{
		m_pCollider_Manager->Add_CollisionObject(CCollider_Manager::OBJECT_MONSTER, this);
	}
	//m_pCollider_Manager->Add_Collider(CCollider_Manager::OBJECT_BOMB, m_pSPHERE);

	if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
	{
		if (GetKeyState('G') & 0x8000)
		{
			m_eState = STATE_GRAB;
			m_pPlayer->Set_State(CPlayer::PLAYERSTATE::PLAYER_GRAB);
		}
	}

	if (STATE_GRAB == m_eState && GetKeyState(VK_LBUTTON) & 0x8000)
	{
		m_fJumpPower = 6.f;
		m_fCurrentPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_eState = STATE_THROW;
	}

	if (CPlayer::PLAYERSTATE::PLAYER_ATTACK == m_pPlayer->Get_State() || CPlayer::PLAYERSTATE::PLAYER_ATTACK_FINAL == m_pPlayer->Get_State())
	{
		if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Item_Blade"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
		{
			m_eState = STATE_WARNING;
		}
	}

	Safe_Release(pGameInstance);

}

HRESULT CObject_Bomb::Render()
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

		m_pShaderCom->Begin(m_iNumPass);

		m_pModelCom->Render(i);
	}

// 자꾸 실행 도중에 m_pDeviceContext가 Null이라는 에러가 남 일단 호출스택보니 여기에서 넘어가서 주석처리해놓음
//#ifdef _DEBUG
//	m_pOBB->Render();
//	m_pSPHERE->Render();
//#endif // _DEBUG

	return S_OK;
}

void CObject_Bomb::Move_GrapPoint(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	_vector vPosition;

	_double Ratio = static_cast<_double>(m_fTimeAcc / 0.5);

	vPosition = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pPlayer->Get_GrapPoint(), static_cast<_float>(Ratio));
	vPosition = XMVectorSetW(vPosition, 1);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CObject_Bomb::Throw(_float fTimeDelta)
{
	m_fJumpTimeAcc += fTimeDelta;

	_float fCellPosY = XMVectorGetY(XMVectorSetW(m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f));

	_float fY = m_fCurrentPosY + ((m_fJumpPower * m_fJumpTimeAcc) - ((9.8f * m_fJumpTimeAcc * m_fJumpTimeAcc) * 0.5f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), fY));

	_float fMyPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (fMyPosY > fCellPosY)
	{

		m_pTransformCom->Go_Axis(static_cast<_double>(fTimeDelta), m_pPlayer->Get_Look(), m_pNavigationCom);

	}

	else if (fMyPosY <= fCellPosY)
	{
		m_ePreState = STATE_THROW;
		m_eState = STATE_IDLE;
		m_fJumpTimeAcc = 0.f;
		m_fJumpPower = 0.f;
	}
}

void CObject_Bomb::Warning(_float fTimeDelta)
{
	m_fBoomTimeAcc += fTimeDelta;

	m_iNumPass = 3;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->PlaySoundW(L"Bomb_Fuse.wav", SOUND_ITEM, 0.2f);

	

	if (3.f < m_fBoomTimeAcc)
	{

		m_pCollider_Manager->Add_Collider(CCollider_Manager::OBJECT_BOMB, m_pSPHERE);
		m_bRelease = true;

	}

	Safe_Release(pGameInstance);

}

HRESULT CObject_Bomb::SetUp_OnTerrain()
{

	_vector vPosition = XMVectorSetW(m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f);

	_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(vPosition));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;

}

_float CObject_Bomb::Get_Instance()
{
	return XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
}

HRESULT CObject_Bomb::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Bomb"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.25f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Collider_SPHERE*/
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
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

HRESULT CObject_Bomb::SetUp_ConstantTable()
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


CObject_Bomb * CObject_Bomb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CObject_Bomb*		pInstance = new CObject_Bomb(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CObject_Bomb"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CObject_Bomb::Clone(void * pArg)
{
	CObject_Bomb*		pInstance = new CObject_Bomb(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CObject_Bomb"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Bomb::Free()
{
	__super::Free();

	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pItem_Blade);
	Safe_Release(m_pSPHERE);
	Safe_Release(m_pOBB);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
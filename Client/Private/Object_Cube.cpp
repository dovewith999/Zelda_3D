#include "stdafx.h"
#include "..\Public\Object_Cube.h"
#include "GameInstance.h"
#include "Player.h"
#include "Cell.h"

CObject_Cube::CObject_Cube(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CObject_Cube::CObject_Cube(const CObject_Cube & rhs)
	: CGameObject(rhs)
{

}

HRESULT CObject_Cube::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Cube::NativeConstruct(void * pArg)
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

	m_pPlayer_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), CGameObject::m_pTransformTag, 0));

	Safe_AddRef(m_pPlayer);
	Safe_AddRef(m_pPlayer_Transform);

	Safe_Release(pGameInstance);

	//현재 위치한 포지션에 해당하는 셀을 찾는 코드
	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vPos;
	XMStoreFloat3(&vPos, vPosition);

	m_pNavigationCom->Find_RealCell(&vPos);

	m_fCellY = XMVectorGetY(XMVectorSetW(m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f));


	return S_OK;
}

void CObject_Cube::Tick(_float fTimeDelta)
{
	if (!m_bUnderCollision && m_bCollision && CPlayer::PLAYER_PUSH_PULL == m_pPlayer->Get_State()/* && CCell::OPTION_OBJECT == m_pNavigationCom->Get_CellOption()*/)
	{
		if (GetKeyState('W') & 0x8000)
		{
			m_pTransformCom->Go_Axis(static_cast<_double>(fTimeDelta), XMVector3Normalize(m_pPlayer->Get_Look()));
			m_pPlayer_Transform->Go_Straight(fTimeDelta);
		}

		if (GetKeyState('S') & 0x8000)
		{
			m_pTransformCom->Go_Axis(static_cast<_double>(fTimeDelta), XMVector3Normalize(m_pPlayer->Get_Look()) * -1.f);
			m_pPlayer_Transform->Go_Backward(fTimeDelta);
		}
	}

	if (9 == m_pPlayer->Get_RoomId() /*|| 8 == m_pPlayer->Get_RoomId()*/)
	{
		if (!m_pOBB->Get_Collision_OBB())
		{
			if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) > m_fCellY)
			{
				m_fJumpTimeAcc += fTimeDelta;

				_float fY = ((m_fJumpPower * m_fJumpTimeAcc) - ((9.8f * m_fJumpTimeAcc * m_fJumpTimeAcc) * 0.5f));

				if (0.f >= fY)
				{
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, fY, 0.f, 0.f));
				}
			}

			else if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) <= m_fCellY)
			{
				m_fJumpTimeAcc = 0.f;
				if (FAILED(SetUp_OnTerrain()))
				{
					return;
				}
			}

		}
	}

	else if (m_pOBB->Get_Collision_OBB())
	{

	}

	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
}

void CObject_Cube::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->isIn_FrustumInWorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.5f))
	{
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
	}

	//if (9 == m_pPlayer->Get_RoomId() /*|| 8 == m_pPlayer->Get_RoomId()*/)
	//{
	//	m_pCollider_Manager->Add_Collider(CCollider_Manager::OBJECT_CUBE, m_pOBB);
	//}

	_float fDistance = fabs(XMVectorGetY(m_pPlayer->Get_Position()) - XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
	{
		m_pCollider_Manager->Add_CollisionObject(CCollider_Manager::OBJECT_NORMAL, this);
		m_pPlayer->Set_Collision(true);
		m_pPlayer->Set_UnderObjectSizeY(1.5f);
		if (0.f == m_fTimeAcc)
		{
			m_fFirstX = XMVectorGetX(m_pPlayer_Transform->Get_State(CTransform::STATE_POSITION));
			m_fFirstZ = XMVectorGetZ(m_pPlayer_Transform->Get_State(CTransform::STATE_POSITION));
			m_bCollision = true;
			m_fTimeAcc += fTimeDelta;
		}

		if (XMVectorGetY(m_pPlayer_Transform->Get_State(CTransform::STATE_POSITION)) < XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + 1.f)
		{
			m_pPlayer->Check_Front(static_cast<_double>(fTimeDelta), Check_Front(), m_fFirstX, m_fFirstZ);
		}

		if (1.f <= fDistance)
		{
			//m_pPlayer->Set_UnderCollision(true);
			m_bUnderCollision = true;
		}
	}

	else
	{
		m_fTimeAcc = 0.f;
		if (CPlayer::PLAYER_PUSH_PULL != m_pPlayer->Get_State())
		{
			m_bCollision = false;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CObject_Cube::Render()
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

_bool CObject_Cube::Check_Front()
{

	_vector vPosDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pPlayer_Transform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = XMVector3Normalize(m_pPlayer_Transform->Get_State(CTransform::STATE_LOOK));

	_vector vFrontCos = XMVector3Dot(vPosDir, vPlayerLook);

	if (0.f <= XMVectorGetX(vFrontCos))
	{
		return true;
	}

	else
	{
		return false;
	}
	

}

HRESULT CObject_Cube::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Cube"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(1.6f, 1.5f, 1.6f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;


	/* For.Com_Navigation*/
	CNavigation::NAVIGATIONDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CObject_Cube::SetUp_ConstantTable()
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

HRESULT CObject_Cube::SetUp_OnTerrain()
{
	_vector vPosition = XMVectorSetW(m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f);

	m_fCellY = XMVectorGetY(vPosition);

	_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(vPosition));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}


CObject_Cube * CObject_Cube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CObject_Cube*		pInstance = new CObject_Cube(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CObject_Cube"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CObject_Cube::Clone(void * pArg)
{
	CObject_Cube*		pInstance = new CObject_Cube(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CObject_Cube"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Cube::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer_Transform);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pOBB);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
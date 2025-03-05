#include "stdafx.h"
#include "..\Public\Object_WaterPot.h"
#include "GameInstance.h"
#include "Player.h"
#include "Collider_Manager.h"

CObject_WaterPot::CObject_WaterPot(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CObject_WaterPot::CObject_WaterPot(const CObject_WaterPot & rhs)
	: CGameObject(rhs)
{

}

HRESULT CObject_WaterPot::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_WaterPot::NativeConstruct(void * pArg)
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

		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_Player")));
		Safe_AddRef(m_pPlayer);

		Safe_Release(pGameInstance);
	}

	//현재 위치한 포지션에 해당하는 셀을 찾는 코드
	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vPos;
	XMStoreFloat3(&vPos, vPosition);

	m_pNavigationCom->Find_RealCell(&vPos);

	m_eState = STATE_IDLE;

	return S_OK;
}

void CObject_WaterPot::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scaled(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	switch (m_eState)
	{
	case Client::CObject_WaterPot::STATE_IDLE:
		if (FAILED(SetUp_OnTerrain()))
		{
			return;
		}
		break;
	case Client::CObject_WaterPot::STATE_GRAB:
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
	case Client::CObject_WaterPot::STATE_THROW:
		Throw(fTimeDelta);
		break;
	default:
		break;
	}

	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
	
}

void CObject_WaterPot::Late_Tick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (true == pGameInstance->isIn_FrustumInWorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.5f))
	{
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);
	}

	if (4 == m_pPlayer->Get_RoomId() || 14 == m_pPlayer->Get_RoomId())
	{
		m_pCollider_Manager->Add_Collider(CCollider_Manager::OBJECT_WATERPOT, m_pOBB);
	}

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
		m_fJumpPower = 4.f;
		m_fCurrentPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_eState = STATE_THROW;
	}


	if (m_pOBB->Get_Collision_OBB())
	{
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_MagRock"), &WorldMatrix)))
		{
			Safe_Release(pGameInstance);

			return;
		}

		m_bRelease = true;
	}


	Safe_Release(pGameInstance);

}

HRESULT CObject_WaterPot::Render()
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



	return S_OK;
}

void CObject_WaterPot::Move_GrapPoint(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	_vector vPosition;

	_double Ratio = static_cast<_double>(m_fTimeAcc / 0.5);

	vPosition = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pPlayer->Get_GrapPoint(), static_cast<_float>(Ratio));
	vPosition = XMVectorSetW(vPosition, 1);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CObject_WaterPot::Throw(_float fTimeDelta)
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
		m_eState = STATE_IDLE;
		m_fJumpTimeAcc = 0.f;
		m_fJumpPower = 0.f;
	}
}

HRESULT CObject_WaterPot::SetUp_OnTerrain()
{
	_vector vPosition = XMVectorSetW(m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f);

	_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(vPosition));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

HRESULT CObject_WaterPot::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_WaterPot"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
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

HRESULT CObject_WaterPot::SetUp_ConstantTable()
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


CObject_WaterPot * CObject_WaterPot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CObject_WaterPot*		pInstance = new CObject_WaterPot(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CObject_WaterPot"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CObject_WaterPot::Clone(void * pArg)
{
	CObject_WaterPot*		pInstance = new CObject_WaterPot(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CObject_WaterPot"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_WaterPot::Free()
{
	__super::Free();


	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pOBB);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
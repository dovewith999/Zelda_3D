#include "stdafx.h"
#include "..\Public\Camera_Player.h"
#include "GameInstance.h"
#include "Player.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Player::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Player::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	//if (FAILED(SetUp_Component()))
	//{
	//	return E_FAIL;
	//}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (static_cast<_uint>(LEVEL_DUNGEON) == pGameInstance->Get_NextLevelID())
	{
		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, L"Layer_Player"));

		m_pPlayer_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), CGameObject::m_pTransformTag, 0));
	}

	else if (static_cast<_uint>(LEVEL_DUNGEON_FINAL) == pGameInstance->Get_NextLevelID())
	{
		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON_FINAL, L"Layer_Player"));

		m_pPlayer_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON_FINAL, TEXT("Layer_Player"), CGameObject::m_pTransformTag, 0));
	}

	Safe_AddRef(m_pPlayer);
	Safe_AddRef(m_pPlayer_Transform);

	Safe_Release(pGameInstance);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayer->Get_Position() + XMLoadFloat4(&_float4(0.f, 2.f, -5.f, 0.f)));
	return S_OK;
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);
	
	m_pTransformCom->LookAt(m_pPlayer->Get_Position());

	m_pPlayer->Set_Axis(XMVectorSet(XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 0.f, XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 0.f), m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(m_pPlayer_Transform->Get_State(CTransform::STATE_POSITION)) + 2.f));

	//if (5.0 <= fabs(XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))))
	//{
	//	m_pTransformCom->Go_Straight(fTimeDelta);
	//}

	//else if (2.0 >= fabs(XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))))
	//{
	//	m_pTransformCom->Go_Backward(fTimeDelta);
	//}

	switch (m_pPlayer->Get_State())
	{
	case CPlayer::PLAYERSTATE::PLAYER_LADDER:
		Action_Ladder(fTimeDelta);
		break;
	case CPlayer::PLAYERSTATE::PLAYER_GETITEM:
		Action_GetItem(fTimeDelta);
		break;
	case CPlayer::PLAYERSTATE::PLAYER_GETITEM_SHORT:
		Action_GetItem(fTimeDelta);
		break;

	default:
		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));

		if (m_pPlayer->Get_Attention())
		{
			if (0.5f >= m_fTimeAcc)
			{
				Move_Lerp_Camera(fTimeDelta);
			}

			else
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayer_Transform->Back_Pos());
			}

		}

		else
		{
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pPlayer_Transform->Get_State(CTransform::STATE_POSITION), XMVectorGetY(m_pPlayer_Transform->Get_State(CTransform::STATE_POSITION)) + 2.f));

			if (0.f != m_fTimeAcc)
			{
				m_fTimeAcc = 0.f;
			}

		//	Move_Lerp_Camera(fTimeDelta);


			if (6.f <= XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
			{
				m_pTransformCom->Go_Axis(fTimeDelta, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));
			}

			else if (2.f >= XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
			{
				m_pTransformCom->Go_Axis(fTimeDelta, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));
			}
		}

		break;
	}

	if (FAILED(__super::Bind_TransformMatrices()))
		return;

	Safe_Release(pGameInstance);

}

void CCamera_Player::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Player::Render()
{
	return S_OK;
}

void CCamera_Player::Move_Lerp_Camera(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	_double Ratio = static_cast<_double>(m_fTimeAcc / 0.5);

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	/*if (false == m_pNavigationCom->Move_OnNavigation(&vPos))
	{
		m_pTransformCom->Go_Straight(static_cast<_double>(fTimeDelta), m_pNavigationCom);
	}

	else	
	{ */
		_vector vPosition;

		vPosition = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pPlayer_Transform->Back_Pos(), static_cast<_float>(Ratio));
		vPosition = XMVectorSetW(vPosition, 1);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	//}


}

void CCamera_Player::Action_Ladder(_float fTimeDelta)
{
	m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(1.5f, XMConvertToRadians(60.f)));
	if (LADDER_END == m_eState_Ladder)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayer_Transform->Back_Pos());
		m_eState_Ladder = LADDER_READY;
	}

	else
	{
		if (GetKeyState('W') & 0x8000)
		{
			m_pTransformCom->Go_Axis(static_cast<_double>(fTimeDelta), XMVectorSet(0.f, 0.8f, 0.f, 0.f));
		}
	}

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayer_Transform->Back_Pos());
}

void CCamera_Player::Action_GetItem(_float fTimeDelta)
{
	Move_Lerp_Camera(fTimeDelta);
}

HRESULT CCamera_Player::SetUp_Component()
{
	/* For.Com_Navigation*/
	CNavigation::NAVIGATIONDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

CCamera_Player * CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CCamera_Player*	pInstance = new CCamera_Player(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Player::Clone(void * pArg)
{
	CCamera_Player*	pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Created : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayer_Transform);
	
}

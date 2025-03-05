#include "stdafx.h"
#include "..\Public\Trigger_Ladder.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Model.h"
#include "Player.h"
#include "UI_BlackOut.h"

CTrigger_Ladder::CTrigger_Ladder(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTrigger_Ladder::CTrigger_Ladder(const CTrigger_Ladder & rhs)
	: CGameObject(rhs)

{

}

HRESULT CTrigger_Ladder::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger_Ladder::NativeConstruct(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

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
	Safe_AddRef(m_pPlayer);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_UI_BlackOut"), TEXT("Prototype_GameObject_UI_BlackOut"))))
	{
		Safe_Release(pGameInstance);

		return E_FAIL;
	}

	m_pUI_BlackOut = dynamic_cast<CUI_BlackOut*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_UI_BlackOut")));
	Safe_AddRef(m_pUI_BlackOut);

	Safe_Release(pGameInstance);
	return S_OK;
}

void CTrigger_Ladder::Tick(_float fTimeDelta)
{
	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
}

void CTrigger_Ladder::Late_Tick(_float fTimeDelta)
{
	
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (3.f > Get_Distance())
	{

		if (CPlayer::PLAYERSTATE::PLAYER_LADDER != m_pPlayer->Get_State())
		{
			if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
			{
				//if (3.f < m_fTimeAcc)
				//{
				//	if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < XMVectorGetY(m_pPlayer->Get_Position()))
				//	{
				//		//m_pPlayer->Set_State(CPlayer::PLAYERSTATE::PLAYER_LADDER);
				//		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_UI_BlackOut"), TEXT("Prototype_GameObject_UI_BlackOut"))))
				//		{
				//			Safe_Release(pGameInstance);

				//			return;
				//		}
				//		m_fTimeAcc = 0.f;
				//	}
				//}

				m_bLightOff = true;
			}
		}
	//	else if (CPlayer::PLAYERSTATE::PLAYER_LADDER == m_pPlayer->Get_State())
	//	{
	//		if ( m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
	//		{
	//			if (3.f < m_fTimeAcc)
	//			{
	//				if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) > XMVectorGetY(m_pPlayer->Get_Position()))
	//				{
	//					m_pPlayer->Set_State(CPlayer::PLAYERSTATE::PLAYER_IDLE);
	//					m_fTimeAcc = 0.f;
	//				}

	//				m_bLightOff = false;
	//			}
	//		}
	//	}
	}

	if (m_bLightOff)
	{
		if (1.f > m_pUI_BlackOut->Get_Alpha())
		{
			m_pUI_BlackOut->Set_Alpha(0.1f);
		}

		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc > 3.f)
		{
			m_bLightOff = false;
		}
	}

	else if (!m_bLightOff)
	{
		if (0.f < m_pUI_BlackOut->Get_Alpha())
		{
			m_pUI_BlackOut->Set_Alpha(-0.1f);
		}

		m_fTimeAcc = 0.f;
	}

	
	if (9 == m_pPlayer->Get_RoomId())
	{
		if (0.f == m_fPlayerTimeAcc)
		{
			m_bLightOff = true;
			m_fPlayerTimeAcc += fTimeDelta;
			m_pPlayer->Set_Position(XMVectorSet(-14.5f, 24.0f, 111.64f, 1.f));
			_float3 vPos = { -14.5f, 24.0f, 111.64f };
			m_pPlayer->Find_RealCell(vPos);
			m_pPlayer->Set_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
		}
	}
	
	//Light_Off();

	Safe_Release(pGameInstance);
}

HRESULT CTrigger_Ladder::Render()
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

		//m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	//m_pOBB->Render();
#endif // _DEBUG


	return S_OK;
}

_float CTrigger_Ladder::Get_Distance() const
{
	return XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
}

void CTrigger_Ladder::Light_Off()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_bLightOff)
	{
		if (0.f < vColor)
		{
			vColor -= 0.1f;
			pGameInstance->Set_LightDiffuse(0, _float4(vColor, vColor, vColor, vColor));
		}

	}

	else if(!m_bLightOff)
	{

		if (1.f >= vColor)
		{
			vColor += 0.1f;
			pGameInstance->Set_LightDiffuse(0, _float4(vColor, vColor, vColor, vColor));
		}

	}

	Safe_Release(pGameInstance);


}


HRESULT CTrigger_Ladder::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Bomb"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_AABB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger_Ladder::SetUp_ConstantTable()
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


CTrigger_Ladder * CTrigger_Ladder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CTrigger_Ladder*		pInstance = new CTrigger_Ladder(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CTrigger_Ladder"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTrigger_Ladder::Clone(void * pArg)
{
	CTrigger_Ladder*		pInstance = new CTrigger_Ladder(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CTrigger_Ladder"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrigger_Ladder::Free()
{
	__super::Free();

	Safe_Release(m_pUI_BlackOut);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pOBB);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

#include "stdafx.h"
#include "..\Public\Magma.h"
#include "GameInstance.h"
#include "Player.h"
CMagma::CMagma(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMagma::CMagma(const CMagma & rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CMagma::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMagma::NativeConstruct(void * pArg)
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
	Safe_AddRef(m_pPlayer);

	Safe_Release(pGameInstance);
	return S_OK;
}

void CMagma::Tick(_float fTimeDelta)
{
	m_pOBB->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMagma::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONLIGHT, this);
	if (4 == m_pPlayer->Get_RoomId() || 14 == m_pPlayer->Get_RoomId())
	{
		m_pCollider_Manager->Add_Collider(CCollider_Manager::OBJECT_MAGMA, m_pOBB);
	}
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//if (3.f > Get_Distance())
	//{
	//	if (m_pOBB->Collision_OBB(dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Com_OBB"))), CCollider::TYPE_OBB))
	//	{
	//		//m_pPlayer->Set_State(CPlayer::PLAYERSTATE::PLAYER_DAMAGED_MAGMA);
	//	}
	//}

	Safe_Release(pGameInstance);

}

HRESULT CMagma::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));

	if (FAILED(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

#ifdef _DEBUG
	//m_pOBB->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CMagma::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider_Manager */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Manager"), TEXT("Com_Collider_Manager"), (CComponent**)&m_pCollider_Manager)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_Magma"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(m_pTransformCom->Get_Scaled().x, m_pTransformCom->Get_Scaled().y, 0.f);//   _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

_float CMagma::Get_Distance() const
{
	return XMVectorGetX(XMVector3Length(m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
}

CMagma * CMagma::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CMagma* pInstance = new CMagma(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Error : Failed Create Magma"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMagma::Clone(void * pArg)
{
	CMagma* pInstance = new CMagma(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Error : Failed Cloned Magma"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMagma::Free()
{
	__super::Free();

	Safe_Release(m_pOBB);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}

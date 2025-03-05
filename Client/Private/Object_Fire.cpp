#include "stdafx.h"
#include "..\Public\Object_Fire.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Player.h"

CObject_Fire::CObject_Fire(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CObject_Fire::CObject_Fire(const CObject_Fire & rhs)
	: CGameObject(rhs)
{
}

HRESULT CObject_Fire::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Fire::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.0f, 0.f, 0.f, 1.f));

	if (nullptr != pArg)
	{
		_vector vPos = *(_vector*)pArg;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		//_matrix WorldMatrix = *(_matrix*)pArg;
		//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, WorldMatrix.r[0]);
		//m_pTransformCom->Set_State(CTransform::STATE_UP, WorldMatrix.r[1]);
		//m_pTransformCom->Set_State(CTransform::STATE_LOOK, WorldMatrix.r[2]);
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMatrix.r[3]);
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pCamera_Player = dynamic_cast<CCamera_Player*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_Camera")));
	Safe_AddRef(m_pCamera_Player);

	Safe_Release(pGameInstance);
	return S_OK;
}

void CObject_Fire::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (0.1f < m_fTimeAcc)
	{
		if (5 == m_iTextureNum)
		{
			m_iTextureNum = 0;
			m_fTimeAcc = 0.f;
		}

		else
		{
			++m_iTextureNum;
			m_fTimeAcc = 0.f;
		}
	}

	//if (m_bBoko)
	//{
	//	_matrix Matrix;
	//	Matrix = XMLoadFloat4x4(&m_SocketMatrix);

	//	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Matrix.r[0]);
	//	m_pTransformCom->Set_State(CTransform::STATE_UP, Matrix.r[1]);
	//	m_pTransformCom->Set_State(CTransform::STATE_LOOK, Matrix.r[2]);
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Matrix.r[3] + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * 2.f + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 0.3f);
	//}

	//m_pTransformCom->LookAt(m_pCamera_Player->Get_Position());

	m_pTransformCom->Set_Scaled(XMVectorSet(1.2f, 1.2f, 1.2f, 1.f));
	m_pSPHERE->Update(m_pTransformCom->Get_WorldMatrix());
}

void CObject_Fire::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONLIGHT, this);


	m_pCamera_Player->Get_WorldMatrix();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_pCamera_Player->Get_WorldMatrix().r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_pCamera_Player->Get_WorldMatrix().r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_pCamera_Player->Get_WorldMatrix().r[2]);

}

HRESULT CObject_Fire::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom) 
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));

	if (FAILED(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTextureNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

#ifdef _DEBUG
	//m_pSPHERE->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CObject_Fire::SetUp_Components()
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
	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_Object_Fire"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	/* For.Com_Collider_SPHERE*/
	CCollider::COLLIDER_DESC ColliderDesc;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale =  _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"), (CComponent**)&m_pSPHERE, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}


CObject_Fire * CObject_Fire::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	CObject_Fire* pInstance = new CObject_Fire(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Error : Failed Create Object_Fire"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CObject_Fire::Clone(void * pArg)
{
	CObject_Fire* pInstance = new CObject_Fire(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Error : Failed Cloned Object_Fire"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Fire::Free()
{
	__super::Free();

	Safe_Release(m_pCamera_Player);
	Safe_Release(m_pSPHERE);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}

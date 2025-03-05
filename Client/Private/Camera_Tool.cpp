#include "stdafx.h"
#include "..\Public\Camera_Tool.h"
#include "GameInstance.h"

CCamera_Tool::CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Tool::CCamera_Tool(const CCamera_Tool & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Tool::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Tool::NativeConstruct(void * pArg)
{


	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Tool::Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}
	
	_long		MouseMove = 0;

	if (pGameInstance->Get_DIMButtonState(CInput_Device::DIMB_WHEEL))
	{
		if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
		}

		if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
		}
	}


	if (FAILED(__super::Bind_TransformMatrices()))
		return;

	Safe_Release(pGameInstance);

}

void CCamera_Tool::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Tool::Render()
{
	return S_OK;
}

CCamera_Tool * CCamera_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CCamera_Tool*	pInstance = new CCamera_Tool(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CCamera_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Tool::Clone(void * pArg)
{
	CCamera_Tool*	pInstance = new CCamera_Tool(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Created : CCamera_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Tool::Free()
{
	__super::Free();


}

#include "..\Public\Light.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CLight::NativeConstruct(const LIGHTDESC& LightDesc)
{
	m_LightDesc = LightDesc;	
	return S_OK;
}

HRESULT CLight::Render(CVIBuffer_Rect* pVIBuffer, CShader * pShader)
{

	/* 빛 정보를 셰이더 전역변수에 던진다. */
	if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(1);

	pVIBuffer->Render();

	return S_OK;
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(LightDesc)))
	{
		MSGBOX(TEXT("Failed to Created : Light"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}

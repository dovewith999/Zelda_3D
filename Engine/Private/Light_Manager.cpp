#include "..\Public\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex) const
{
	if (iIndex >= m_Lights.size())
		return nullptr;

	return m_Lights[iIndex]->Get_LightDesc();
}

void CLight_Manager::Set_LightDiffuse(_uint iIndex, _float4 vDiffuse)
{
	
	m_Lights[iIndex]->Set_LightDiffuse(vDiffuse);
	
}

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pDeviceContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render_Lights(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	for (auto& pLight : m_Lights)
	{
		pLight->Render(pVIBuffer, pShader);
	}

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}

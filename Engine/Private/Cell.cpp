#include "..\Public\Cell.h"
#include "VIBuffer_Triangle.h"
#include "Shader.h"
#include "PipeLine.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CCell::NativeConstruct(const _float3 * pPoint, _uint iIndex, OPTION eCellOption)
{
	memcpy(&m_vPoint, pPoint, sizeof(_float3) * POINT_END);

	XMStoreFloat3(&m_vLine[LINE_AB], XMLoadFloat3(&m_vPoint[POINT_B]) - XMLoadFloat3(&m_vPoint[POINT_A]));
	XMStoreFloat3(&m_vLine[LINE_BC], XMLoadFloat3(&m_vPoint[POINT_C]) - XMLoadFloat3(&m_vPoint[POINT_B]));
	XMStoreFloat3(&m_vLine[LINE_CA], XMLoadFloat3(&m_vPoint[POINT_A]) - XMLoadFloat3(&m_vPoint[POINT_C]));

	m_iIndex = iIndex;

	m_eCellOption = eCellOption;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

#ifdef _DEBUG
	if (FAILED(Ready_Debug_Buffer()))
	{
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

_bool CCell::Compare_Points(const _float3 * pSourPoint, const _float3 * pDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(pSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(pDestPoint)))
		{
			return true;
		}

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(pDestPoint)))
		{
			return true;
		}
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(pSourPoint)))
	{

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(pSourPoint)))
	{

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(pDestPoint)))
			return true;
	}

	return _bool();
}

_bool CCell::isIn(const _float3 * pPosition, _int * pNeighborIndex)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector vSourDir = XMVector3Normalize(XMLoadFloat3(pPosition) - XMLoadFloat3(&m_vPoint[i]));
		_vector vDestDir = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0.f, m_vLine[i].x, 0.f));

		if (0 < XMVectorGetX(XMVector3Dot(vSourDir, vDestDir)))
		{
			*pNeighborIndex = m_iNeigborIndex[i];

			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
HRESULT CCell::Render(CShader * pShader)
{
	if (nullptr == m_pVIBuffer)
	{
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4));
	if (OPTION_JUMP == m_eCellOption)
	{
		pShader->Set_RawValue("g_vColor", &_float4(0.f, 0.f, 1.f, 1.f), sizeof(_float4));
	}
	else if (OPTION_NON == m_eCellOption)
	{
		pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 1.f, 1.f), sizeof(_float4));
	}
	else if (OPTION_CLIMB == m_eCellOption)
	{
		pShader->Set_RawValue("g_vColor", &_float4(1.f, 1.f, 0.f, 1.f), sizeof(_float4));
	}

	else if (OPTION_CLIMB_HIGH == m_eCellOption)
	{
		pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 1.f, 1.f), sizeof(_float4));
	}

	else if (OPTION_OBJECT == m_eCellOption)
	{
		pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));
	}

	else if(OPTION_NORMAL == m_eCellOption)
	{
		pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));
	}
	CPipeLine* pPipeLine = CPipeLine::Get_Instance();

	Safe_AddRef(pPipeLine);

	pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));

	Safe_Release(pPipeLine);

	pShader->Begin(0);

	m_pVIBuffer->Render();

	return S_OK;

}

HRESULT CCell::Ready_Debug_Buffer()
{
	m_pVIBuffer = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext, m_vPoint);
	if (nullptr == m_pVIBuffer)
	{
		return E_FAIL;
	}

	return S_OK;
}
#endif // DEBUG


CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _float3 * pPoints, _uint iIndex, OPTION eCellOption)
{
	CCell* pInstance = new CCell(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct(pPoints, iIndex, eCellOption)))
	{
		MSGBOX(TEXT("Error : Failed Created Cell"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif // _DEBUG

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}

#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "VIBuffer_Triangle.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_iCurrentCellIndex(rhs.m_iCurrentCellIndex)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // _DEBUG

{
	for (auto& pCell : m_Cells)
	{
		Safe_AddRef(pCell);
	}

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // _DEBUG

}

HRESULT CNavigation::NativeConstruct_Prototype(const _tchar * pNavigationDataFilePath)
{
	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (nullptr == hFile)
	{
		return E_FAIL;
	}

	while (true)
	{
		_float3 vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		CCell::OPTION Option;

		ReadFile(hFile, &Option, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}

		if (476 == m_iNumCell)
		{
			Option = CCell::OPTION_NON;
		}

		CCell* pCell = CCell::Create(m_pDevice, m_pDeviceContext, vPoints, m_Cells.size(), Option);
		if (nullptr == pCell)
		{
			return E_FAIL;
		}

		m_Cells.push_back(pCell);
		++m_iNumCell;
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbor()))
	{
		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(Ready_Debug()))
	{
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CNavigation::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
	{
		m_iCurrentCellIndex = ((NAVIGATIONDESC*)pArg)->iCurrentIndex;
	}
	//if (475 == m_iCurrentCellIndex)
	//{
	//	m_Cells[m_iCurrentCellIndex]->Set_Option(CCell::OPTION_NON);
	//}

	

	return S_OK;
}

_bool CNavigation::Move_OnNavigation(const _float3 * pPosition)
{
	_int iNeigborIndex = -1;

	//셀 내부에서 움직였을 경우 true를 반환하여 움직일 수 있게 한다.
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(pPosition, &iNeigborIndex)) 
	{
		return true;
	}

	else
	{
		// 이웃된 셀이 있을 경우 그 셀로 이동시키고 실제로도 움직일 수 있게 한다.
		if (-1 != iNeigborIndex)
		{

			while (true)
			{
				if (-1 == iNeigborIndex)
				{
					return false;
				}

			/*	if (CCell::OPTION_NORMAL == m_Cells[m_iCurrentCellIndex]->Get_Option())
				{
					if (CCell::OPTION_NON == m_Cells[iNeigborIndex]->Get_Option())
					{
						return false;
					}
				}*/

				if (true == m_Cells[iNeigborIndex]->isIn(pPosition, &iNeigborIndex))
				{
					m_iPreCellIndex = m_iCurrentCellIndex;
					m_iCurrentCellIndex = iNeigborIndex;
					break;
				}
			}
	
			return true;
		}

		//이웃된 셀이 없을 경우 애니메이션은 재생되지만 실제 Pos값이 변경되지 못하게 한다. -> 못나가게 한다.
		else
		{
			return false;
		}
	}
}

_vector CNavigation::Compute_Height(_fvector vWorldPos)
{
	_matrix matWorldMatrix = XMLoadFloat4x4(&m_Cells[m_iCurrentCellIndex]->Get_WorldMatrix());

	_vector vPosition = dynamic_cast<CVIBuffer_Triangle*>(m_Cells[m_iCurrentCellIndex]->Get_ViBuffer())->Compute_Height(vWorldPos, matWorldMatrix);

	return vPosition;
}

_uint CNavigation::Get_CellOption() const
{
	return m_Cells[m_iCurrentCellIndex]->Get_Option();
}

_uint CNavigation::Get_PreCellOption() const
{
	return m_Cells[m_iPreCellIndex]->Get_Option();
}

void CNavigation::Find_RealCell(const _float3 * pPosition)
{
	_int iNeigborIndex = -1;

	while (true)
	{
		if (true == m_Cells[m_iCurrentCellIndex]->isIn(pPosition, &iNeigborIndex))
		{
			//if(m_Cells[m_iCurrentCellIndex])

			//_vector vPos = XMVectorSet(pPosition->x, pPosition->y, pPosition->z, 1.f);

			//if (pPosition->y > XMVectorGetY(Compute_Height(vPos)) + 0.5f)
			//{
			//	++m_iCurrentCellIndex;
			//}

			//else
			//{
			//	break;
			//}

			break;
		}

		else
		{
			++m_iCurrentCellIndex;
		}
	}
}

void CNavigation::Find_CellOption(_uint m_iOptionID)
{
	_int iNeigborIndex = -1;

	while (true)
	{
		if (m_iOptionID == static_cast<_uint>(m_Cells[m_iCurrentCellIndex]->Get_Option()))
		{
			break;
		}

		else
		{
			++m_iCurrentCellIndex;
		}
	}
}

HRESULT CNavigation::SetUp_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
			{
				continue;
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell->Get_Index());
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell->Get_Index());
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell->Get_Index());
			}
		}
	}

	return S_OK;
}

#ifdef _DEBUG
HRESULT CNavigation::Ready_Debug()
{
	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElement);

	if (nullptr == m_pShader)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CNavigation::Render()
{
	if (nullptr == m_pShader)
	{
		return E_FAIL;
	}

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (-1 == m_iCurrentCellIndex)
	{
		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
			{
				pCell->Render(m_pShader);
			}
		}
	}

	else
	{
		WorldMatrix._42 = 0.1f;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

		m_pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

		m_Cells[m_iCurrentCellIndex]->Render(m_pShader);
	}

	return S_OK;
}
#endif // DEBUG



CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pNavigationDataFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pNavigationDataFilePath)))
	{
		MSGBOX(TEXT("Error : Failed Created Navigation_Prototype"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation* pInstance = new CNavigation(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Error : Failed Cloned Navigation_Prototype"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
	{
		Safe_Release(pCell);
	}

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // _DEBUG


}

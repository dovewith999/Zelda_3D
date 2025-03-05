#include "..\Public\Picking.h"
#include "GameInstance.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}

HRESULT CPicking::NativeConstruct(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, HWND hWnd, _uint iWinCX, _uint iWinCY)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	m_iWinCX = iWinCX;
	m_iWinCY = iWinCY;

	m_hWnd = hWnd;
	
	return S_OK;
}

void CPicking::Transform_ToWorldSpace()
{
	POINT		ptMouse;

	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	_vector		vMousePos;

	vMousePos = XMVectorSet(_float(((_float)ptMouse.x / (m_iWinCX >> 1)) - 1), _float(((_float)ptMouse.y / (m_iWinCY >> 1)) * -1 + 1), 0.f, 1.f);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_matrix	ProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix);

	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrix);

	_vector	vRayDir, vRayPos;
	vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.0);
	vRayDir = XMVectorSetW(vMousePos, 0.f);

	_matrix	ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	XMStoreFloat4(&m_vWorldRayDir, XMVector3TransformNormal(vRayDir, ViewMatrix));
	XMStoreFloat4(&m_vWorldRayPos, XMVector3TransformCoord(vRayPos, ViewMatrix));

	Safe_Release(pGameInstance);
}

bool CPicking::Picking(CVIBuffer * pVIBuffer, CTransform * pTransform, _float4* pOut)
{
	_vector		vRayPos = XMVector3TransformCoord(XMLoadFloat4(&m_vWorldRayPos), pTransform->Get_WorldMatrixInverse());
	_vector		vRayDir = XMVector3TransformNormal(XMLoadFloat4(&m_vWorldRayDir), pTransform->Get_WorldMatrixInverse());

	vRayDir = XMVector3Normalize(vRayDir);

	const _float3* pVerticesPos = pVIBuffer->Get_VerticesPos();

	void* pIndices = pVIBuffer->Get_Indices();

	_uint	iNumFaces = pVIBuffer->Get_NumPrimitive();

	DXGI_FORMAT		eFormat = pVIBuffer->Get_IndexFormat();

	_uint		iIndexSize = eFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;

	for (_uint i = 0; i < iNumFaces; ++i)
	{
		_uint		iIndices[3] = { 0 };

		for (_uint j = 0; j < 3; ++j)
		{
			if (iIndexSize == 2)
				iIndices[j] = *(_ushort*)((((_byte*)pIndices) + (iIndexSize * 3) * i) + iIndexSize * j);
			else
				iIndices[j] = *(_ulong*)((((_byte*)pIndices) + (iIndexSize * 3) * i) + iIndexSize * j);
		}

		_float	fDist;


		if(TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pVerticesPos[iIndices[0]]), XMLoadFloat3(&pVerticesPos[iIndices[1]]), XMLoadFloat3(&pVerticesPos[iIndices[2]]), fDist))
		{
			_vector	vPickPos = vRayPos + vRayDir * fDist;

			  XMStoreFloat4(&(*pOut), XMVector3TransformCoord(vPickPos, pTransform->Get_WorldMatrix()));

			return true;
		}

	}

	return false;
}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}

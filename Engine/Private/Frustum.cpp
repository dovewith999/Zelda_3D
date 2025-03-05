#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum);

CFrustum::CFrustum()
{
}


HRESULT CFrustum::NativeConstruct()
{
	m_vProjPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vProjPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vProjPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vProjPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vProjPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vProjPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vProjPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vProjPoints[7] = _float3(-1.f, -1.f, 1.f);


	return S_OK;
}

void CFrustum::Transform_ToWorldSpace()
{
	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	Safe_AddRef(pPipeLine);

	_matrix ProjMatrixInv = XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	_matrix ViewMatrixInv = XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	for (_uint i = 0; i < 8; ++i)
	{
		XMStoreFloat3(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vProjPoints[i]), ProjMatrixInv));
		XMStoreFloat3(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoints[i]), ViewMatrixInv));
	}

	XMStoreFloat4(&m_WorldPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&m_vWorldPoints[1]), XMLoadFloat3(&m_vWorldPoints[5]), XMLoadFloat3(&m_vWorldPoints[6])));
	XMStoreFloat4(&m_WorldPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&m_vWorldPoints[4]), XMLoadFloat3(&m_vWorldPoints[0]), XMLoadFloat3(&m_vWorldPoints[3])));
	XMStoreFloat4(&m_WorldPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&m_vWorldPoints[4]), XMLoadFloat3(&m_vWorldPoints[5]), XMLoadFloat3(&m_vWorldPoints[1])));
	XMStoreFloat4(&m_WorldPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&m_vWorldPoints[3]), XMLoadFloat3(&m_vWorldPoints[2]), XMLoadFloat3(&m_vWorldPoints[6])));
	XMStoreFloat4(&m_WorldPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&m_vWorldPoints[5]), XMLoadFloat3(&m_vWorldPoints[4]), XMLoadFloat3(&m_vWorldPoints[7])));
	XMStoreFloat4(&m_WorldPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&m_vWorldPoints[0]), XMLoadFloat3(&m_vWorldPoints[1]), XMLoadFloat3(&m_vWorldPoints[2])));

	Safe_Release(pPipeLine);
}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	_matrix WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrix);

	_vector vLocalPoints[8];

	for (_uint i = 0; i < 8; ++i)
	{
		vLocalPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoints[i]), WorldMatrixInv);
	}

	XMStoreFloat4(&m_LocalPlanes[0], XMPlaneFromPoints(vLocalPoints[1], vLocalPoints[5], vLocalPoints[6]));
	XMStoreFloat4(&m_LocalPlanes[1], XMPlaneFromPoints(vLocalPoints[4], vLocalPoints[0], vLocalPoints[3]));
	XMStoreFloat4(&m_LocalPlanes[2], XMPlaneFromPoints(vLocalPoints[4], vLocalPoints[5], vLocalPoints[1]));
	XMStoreFloat4(&m_LocalPlanes[3], XMPlaneFromPoints(vLocalPoints[3], vLocalPoints[2], vLocalPoints[6]));
	XMStoreFloat4(&m_LocalPlanes[4], XMPlaneFromPoints(vLocalPoints[5], vLocalPoints[4], vLocalPoints[7]));
	XMStoreFloat4(&m_LocalPlanes[5], XMPlaneFromPoints(vLocalPoints[0], vLocalPoints[1], vLocalPoints[2]));

}

_bool CFrustum::isIn_FrustumInWorldSpace(_fvector vWorldPos, _float fRadian)
{
	for (_uint i = 0; i < 6; ++i)
	{
		// 평면의 방적식은 방향벡터의 내적 계산의 식과 동일한 계산법이기 때문에 내적인 Dot의 이름이 붙었다.
		if (fRadian < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlanes[i]), vWorldPos)))
		{
			return false;
		}
	}
	return true;
}

_bool CFrustum::isIn_FrustumInLocalSpace(_fvector vLocalPos, _float fRadian)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRadian < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_LocalPlanes[i]), vLocalPos)))
		{
			return false;
		}
	}
	return true;
}

void CFrustum::Free()
{
}

#include "stdafx.h"
#include "..\Public\Camera_Logo.h"

CCamera_Logo::CCamera_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Logo::CCamera_Logo(const CCamera_Logo & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Logo::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	return S_OK;
}

HRESULT CCamera_Logo::NativeConstruct(void * pArg)
{
	return S_OK;
}

void CCamera_Logo::Tick(_float fTimeDelta)
{
}

void CCamera_Logo::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Logo::Render()
{
	return S_OK;
}

CCamera_Logo * CCamera_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC & TransformDesc)
{
	return nullptr;
}

CGameObject * CCamera_Logo::Clone(void * pArg)
{
	return nullptr;
}

void CCamera_Logo::Free()
{
}

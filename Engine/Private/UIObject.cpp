#include "..\Public\UIObject.h"

CUIObject::CUIObject(ID3D11Device * pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUIObject::CUIObject(const CUIObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUIObject::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC & TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIObject::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIObject::Tick(_float fTimeDelta)
{
}

void CUIObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
	return S_OK;
}

void CUIObject::Free()
{
	__super::Free();
}

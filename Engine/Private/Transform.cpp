#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_TransformDesc(rhs.m_TransformDesc)
{

}

_float3 CTransform::Get_Scaled() const
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[0])), 
		XMVectorGetX(XMVector3Length(WorldMatrix.r[1])), 
		XMVectorGetX(XMVector3Length(WorldMatrix.r[2])));
}

void CTransform::Set_TransformDesc(const TRANSFORMDESC & TransformDesc)
{
	m_TransformDesc = TransformDesc;
}

void CTransform::Set_Scaled(_fvector vScale)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * XMVectorGetX(vScale));
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * XMVectorGetY(vScale));
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * XMVectorGetZ(vScale));
}

HRESULT CTransform::NativeConstruct_Prototype(const TRANSFORMDESC & TransformDesc)
{
	m_TransformDesc = TransformDesc;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

HRESULT CTransform::Go_Straight(_double TimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr != pNaviCom)
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, vPosition);

		if (false == pNaviCom->Move_OnNavigation(&vPos))
		{
			return S_OK;
		}

	}

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Backward(_double TimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr != pNaviCom)
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, vPosition);

		if (false == pNaviCom->Move_OnNavigation(&vPos))
		{
			return S_OK;
		}

	}

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Right(_double TimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr != pNaviCom)
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, vPosition);

		if (false == pNaviCom->Move_OnNavigation(&vPos))
		{
			return S_OK;
		}

	}

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Left(_double TimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr != pNaviCom)
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, vPosition);

		if (false == pNaviCom->Move_OnNavigation(&vPos))
		{
			return S_OK;
		}

	}

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	//_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * TimeDelta);
	_vector		RotationQuaternion = XMQuaternionRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * TimeDelta);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(RotationQuaternion);
	// XMVector4Transform(vRight, RotationMatrix);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	_vector		RotationQuaternion = XMQuaternionRotationAxis(vAxis, fRadian);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(RotationQuaternion);

	_float3		vScale = Get_Scaled();

	// XMVector4Transform(vRight, RotationMatrix);
	vRight = XMVector3TransformNormal(vRight, RotationMatrix) * vScale.x;
	vUp = XMVector3TransformNormal(vUp, RotationMatrix) * vScale.y;
	vLook = XMVector3TransformNormal(vLook, RotationMatrix) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::LookAt(_fvector vAt)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	_vector		vLook = vAt - vPosition;

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);	
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);

	return S_OK;
}

_vector CTransform::Back_Pos()
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector vUp = Get_State(CTransform::STATE_UP);

	vPosition -= XMVector3Normalize(vLook) * 5.f;

	vPosition += XMVector3Normalize(vUp) * 2.f;

	return vPosition;
}

_vector CTransform::Front_Pos()
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vLook) * 0.7f;

	//vPosition += XMVector3Normalize(vUp) * 2.f;

	return vPosition;
}

HRESULT CTransform::LookAtAxis(_fvector vAxis)
{
	_vector vLook = vAxis;

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3 vScale = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);

	return S_OK;
}

HRESULT CTransform::Go_Axis(_double TimeDelta, _fvector vAxis, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vDir = vAxis;

	vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;


	if (nullptr != pNaviCom)
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, vPosition);

		if (false == pNaviCom->Move_OnNavigation(&vPos))
		{
			return S_OK;
		}

	}


	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Bind_OnShader(CShader * pShader, const char * pConstantName)
{
	if (nullptr == pShader)
		return E_FAIL;

	_float4x4		WorldMatrix; 	
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));

	return pShader->Set_RawValue(pConstantName, &WorldMatrix, sizeof(_float4x4));	
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const TRANSFORMDESC& TransformDesc)
{
	CTransform*		pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}

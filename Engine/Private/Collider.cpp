#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
#ifdef _DEBUG
	, m_pBasicEffect(rhs.m_pBasicEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_pBatch(rhs.m_pBatch)
#endif // DEBUG

{

#ifdef _DEBUG
	Safe_AddRef(rhs.m_pInputLayout);
#endif // _DEBUG

	if (TYPE_AABB == m_eType)
	{
		m_pAABB = new BoundingBox(*rhs.m_pAABB);
		m_pAABB_World = new BoundingBox(*rhs.m_pAABB_World);
	}

	else if (TYPE_OBB == m_eType)
	{
		m_pOBB = new BoundingOrientedBox(*rhs.m_pOBB);
		m_pOBB_World = new BoundingOrientedBox(*rhs.m_pOBB_World);
	}

	else if (TYPE_SPHERE == m_eType)
	{
		m_pSPHERE = new BoundingSphere(*rhs.m_pSPHERE);
		m_pSPHERE_World = new BoundingSphere(*rhs.m_pSPHERE_World);
	}

}

HRESULT CCollider::NativeConstruct_Prototype(TYPE eType)
{
	m_eType = eType;

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		m_pAABB_World = new BoundingBox();
		break;

	case TYPE_OBB:
		m_pOBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		m_pOBB_World = new BoundingOrientedBox();
		break;

	case TYPE_SPHERE:
		m_pSPHERE = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		m_pSPHERE_World = new BoundingSphere();
		break;
	}
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	if (nullptr == m_pBatch)
	{
		MSGBOX(TEXT("Error : m_pBatch is NULL"));
		return E_FAIL;
	}

	m_pBasicEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pBasicEffect)
	{
		MSGBOX(TEXT("Error : m_pBasicEffect is NULL"));
		return E_FAIL;
	}

	m_pBasicEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;

	size_t iShaderByteCodeLength = 0;

	m_pBasicEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
	{
		MSGBOX(TEXT("Error : Failed Created InputLayout"));
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CCollider::NativeConstruct(void * pArg)
{
	if (nullptr == pArg)
	{
		return S_OK;
	}

	COLLIDER_DESC ColliderDesc;
	memcpy(&ColliderDesc, pArg, sizeof(COLLIDER_DESC));
	memcpy(&m_eDesc, pArg, sizeof(COLLIDER_DESC));

	_matrix TransformMatrix;
	_matrix ScaleMatrix, RotationMatrixX, RotationMatrixY, RotationMatrixZ, TranslationMatrix;

	ScaleMatrix = XMMatrixScaling(ColliderDesc.vScale.x, ColliderDesc.vScale.y, ColliderDesc.vScale.z);
	RotationMatrixX = XMMatrixRotationX(ColliderDesc.vAngle.x);
	RotationMatrixY = XMMatrixRotationY(ColliderDesc.vAngle.y);
	RotationMatrixZ = XMMatrixRotationZ(ColliderDesc.vAngle.z);

	TranslationMatrix = XMMatrixTranslation(ColliderDesc.vPosition.x, ColliderDesc.vPosition.y, ColliderDesc.vPosition.z);

	if (TYPE_OBB == m_eType)
	{
		TransformMatrix = RotationMatrixX * RotationMatrixY * RotationMatrixZ;
	}

	else
	{
		TransformMatrix = ScaleMatrix  * TranslationMatrix;
	}

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB->Transform(*m_pAABB, TransformMatrix);
		break;
	case TYPE_OBB:
		m_pOBB->Center = ColliderDesc.vPosition;

		m_pOBB->Extents.x = ColliderDesc.vScale.x * 0.5f;
		m_pOBB->Extents.y = ColliderDesc.vScale.y * 0.5f;
		m_pOBB->Extents.z = ColliderDesc.vScale.z * 0.5f;

		m_pOBB->Transform(*m_pOBB, TransformMatrix);
		break;
	case TYPE_SPHERE:
		m_pSPHERE->Transform(*m_pSPHERE, TransformMatrix);
		break;
	}

	return S_OK;
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	Safe_AddRef(pPipeLine);

	m_pBasicEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pBasicEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pBasicEffect->Apply(m_pDeviceContext);

	_vector		vColor = m_bIsCollision_Player == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f);

	m_pBatch->Begin();

	switch (m_eType)
	{
	case TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB_World, vColor);
		break;
	case TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB_World, vColor);
		break;
	case TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSPHERE_World, vColor);
		break;
	}

	m_pBatch->End();
	Safe_Release(pPipeLine);

	return S_OK;
}
#endif // _DEBUG
_bool CCollider::Collision_AABB(CCollider * pTargetCollider, TYPE eType)
{
	switch (eType)
	{
	case Engine::CCollider::TYPE_SPHERE:
	{
		if (m_pAABB_World->Intersects(*pTargetCollider->m_pSPHERE_World))
		{
			m_bIsCollision = true;
		}

		else
		{
			m_bIsCollision = false;
		}
	}
		break;
	case Engine::CCollider::TYPE_AABB:
	{
		if (m_pAABB_World->Intersects(*pTargetCollider->m_pAABB_World))
		{
			m_bIsCollision = true;
		}

		else
		{
			m_bIsCollision = false;
		}
	}
		break;
	case Engine::CCollider::TYPE_OBB:
	{
		if (m_pAABB_World->Intersects(*pTargetCollider->m_pOBB_World))
		{
			m_bIsCollision = true;
		}

		else
		{
			m_bIsCollision = false;
		}
	}
		break;
	}


	return m_bIsCollision;
}

_bool CCollider::Collision_OBB(CCollider * pTargetCollider, TYPE eType)
{
	switch (eType)
	{
	case Engine::CCollider::TYPE_SPHERE:
	{
		if (m_pOBB_World->Intersects(*pTargetCollider->m_pSPHERE_World))
		{
			m_bIsCollision = true;
		}

		else
		{
			m_bIsCollision = false;
		}
	}
	break;
	case Engine::CCollider::TYPE_AABB:
	{
		if (m_pOBB_World->Intersects(*pTargetCollider->m_pAABB_World))
		{
			m_bIsCollision = true;
		}

		else
		{
			m_bIsCollision = false;
		}
	}
	break;
	case Engine::CCollider::TYPE_OBB:
	{
		if (m_pOBB_World->Intersects(*pTargetCollider->m_pOBB_World))
		{
			m_bIsCollision = true;
		}

		else
		{
			m_bIsCollision = false;
		}
	}
	break;
	}

	return m_bIsCollision;
}

_bool CCollider::Collision_SPHERE(CCollider * pTargetCollider, TYPE eType)
{
	switch (eType)
	{

	case Engine::CCollider::TYPE_SPHERE:
	{
		if (m_pSPHERE_World->Intersects(*pTargetCollider->m_pSPHERE_World))
		{
			m_bIsCollision = true;
		}

		else
		{
			m_bIsCollision = false;
		}
	}
	break;
	case Engine::CCollider::TYPE_AABB:
	{
		if (m_pSPHERE_World->Intersects(*pTargetCollider->m_pAABB_World))
		{
			m_bIsCollision = true;
		}

		else
		{
			m_bIsCollision = false;
		}
	}
	break;
	case Engine::CCollider::TYPE_OBB:
	{
		if (m_pSPHERE_World->Intersects(*pTargetCollider->m_pOBB_World))
		{
			m_bIsCollision = true;
		}

		else
		{
			m_bIsCollision = false;
		}
	}
	break;

	}

	return m_bIsCollision;
}

_bool CCollider::Collision_Player(CCollider * pTargetCollider, TYPE eType)
{
	switch (eType)
	{
	case Engine::CCollider::TYPE_SPHERE:
	{
		if (m_pOBB_World->Intersects(*pTargetCollider->m_pSPHERE_World))
		{
			m_bIsCollision_Player = true;
		}

		else
		{
			m_bIsCollision_Player = false;
		}
	}
	break;
	case Engine::CCollider::TYPE_AABB:
	{
		if (m_pOBB_World->Intersects(*pTargetCollider->m_pAABB_World))
		{
			m_bIsCollision_Player = true;
		}

		else
		{
			m_bIsCollision_Player = false;
		}
	}
	break;
	case Engine::CCollider::TYPE_OBB:
	{
		if (m_pOBB_World->Intersects(*pTargetCollider->m_pOBB_World))
		{
			m_bIsCollision_Player = true;
		}

		else
		{
			m_bIsCollision_Player = false;
		}
	}
	break;
	}

	return m_bIsCollision_Player;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB->Transform(*m_pAABB_World, Remove_Rotation(WorldMatrix));
		m_vPos = m_pAABB_World->Center;
		break;
	case TYPE_OBB:
		m_pOBB->Transform(*m_pOBB_World, Make_Rotation(WorldMatrix));

		XMStoreFloat3(&m_pOBB_World->Center, XMVector3TransformNormal(XMLoadFloat3(&m_pOBB->Center), Remove_Transform(WorldMatrix)) + WorldMatrix.r[3]);
		m_pOBB_World->Extents.x *= XMVectorGetX(XMVector3Length(WorldMatrix.r[0]));
		m_pOBB_World->Extents.y *= XMVectorGetX(XMVector3Length(WorldMatrix.r[1]));
		m_pOBB_World->Extents.z *= XMVectorGetX(XMVector3Length(WorldMatrix.r[2]));

		m_vPos = m_pOBB_World->Center;
		break;

	case TYPE_SPHERE:
		m_pSPHERE->Transform(*m_pSPHERE_World, WorldMatrix);
		m_vPos = m_pSPHERE_World->Center;
		break;
	}
}

_matrix CCollider::Remove_Rotation(_fmatrix Transform)
{
	_matrix Result = Transform;

	_float fScaleX = XMVectorGetX(XMVector3Length(Transform.r[0]));
	_float fScaleY = XMVectorGetY(XMVector3Length(Transform.r[1]));
	_float fScaleZ = XMVectorGetZ(XMVector3Length(Transform.r[2]));

	Result.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * fScaleX;
	Result.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * fScaleY;
	Result.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * fScaleZ;

	return Result;
}

_matrix CCollider::Make_Rotation(_fmatrix Transform)
{

	_matrix Result = XMMatrixIdentity();

	Result.r[0] = XMVector3Normalize(Transform.r[0]);
	Result.r[1] = XMVector3Normalize(Transform.r[1]);
	Result.r[2] = XMVector3Normalize(Transform.r[2]);

	return Result;
}

_matrix CCollider::Remove_Transform(_fmatrix Transform)
{
	_matrix Result = Transform;

	Result.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	return Result;
}

CCollider * CCollider::Create(ID3D11Device * pDeivce, ID3D11DeviceContext * pDeviceContext, TYPE eType)
{
	CCollider* pInstance = new CCollider(pDeivce, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(eType)))
	{
		MSGBOX(TEXT("Error : Failed Created CCollider"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider* pInstance = new CCollider(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Error : Failed Cloned CCollider"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete(m_pAABB);
		Safe_Delete(m_pOBB);
		Safe_Delete(m_pSPHERE);

		Safe_Delete(m_pAABB_World);
		Safe_Delete(m_pOBB_World);
		Safe_Delete(m_pSPHERE_World);
#ifdef _DEBUG	
		Safe_Delete(m_pBasicEffect);
		Safe_Delete(m_pBatch);
#endif // _DEBUG
	}

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
#endif // _DEBUG

	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSPHERE);

	Safe_Delete(m_pAABB_World);
	Safe_Delete(m_pOBB_World);
	Safe_Delete(m_pSPHERE_World);
	
}

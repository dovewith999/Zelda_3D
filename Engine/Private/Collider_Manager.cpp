#include "..\Public\Collider_Manager.h"
#include "GameObject.h"
#include "Collision_Thread.h"

CCollider_Manager::CCollider_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

HRESULT CCollider_Manager::NativeConstruct_Prototype()
{
	m_pThread = CCollision_Thread::Create(m_pDevice, m_pDeviceContext);

	return S_OK;
}

HRESULT CCollider_Manager::Add_CollisionObject(OBJECTGROUP eCollisionGroup, CGameObject * pGameObject)
{
	if (eCollisionGroup >= OBJECT_END)
		return E_FAIL;

	m_CollisionList[eCollisionGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject); 

	sort(m_CollisionList[eCollisionGroup].begin(), m_CollisionList[eCollisionGroup].end(), [](CGameObject* pSour, CGameObject* Dest)
	{
		return pSour->Get_Distance() < Dest->Get_Distance();
	});

	return S_OK;
}

HRESULT CCollider_Manager::Add_Collider(OBJECTGROUP eCollisionGroup, CCollider * pCollider)
{
	m_pThread->Add_Collider((_uint)eCollisionGroup, pCollider);

	return S_OK;
}

HRESULT CCollider_Manager::Init_CollisionList()
{
	for (_uint i = 0; i < OBJECT_END; ++i)
	{
		for (auto& pTarget : m_CollisionList[i])
		{
			if (nullptr != pTarget)
			{
				Safe_Release(pTarget);
			}
		}
		m_CollisionList[i].clear();
	}

	return S_OK;

}

HRESULT CCollider_Manager::Check_Collision()
{
	m_pThread->Check_Collision();

	return S_OK;
}

CGameObject * CCollider_Manager::Get_Target()
{
	for (_uint i = 0; i < OBJECT_NPC; ++i)
	{
		for (auto& pTarget : m_CollisionList[i])
		{
			if (nullptr != pTarget)
			{
				return pTarget;
			}
		}
	}

	return nullptr;
}

CCollider_Manager * CCollider_Manager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCollider_Manager* pInstance = new CCollider_Manager(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX(TEXT("Error : Failed Create Collider_Manager"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider_Manager::Clone(void * pArg)
{
	AddRef();
	return this;
}

void CCollider_Manager::Free()
{
	__super::Free();
	
	for (_uint i = 0; i < OBJECT_END; ++i)
	{
		for (auto& pGameObject : m_CollisionList[i])
		{
			Safe_Release(pGameObject);
		}

		m_CollisionList[i].clear();
	}
	
	Safe_Release(m_pThread);
}

#include "..\Public\Collision_Thread.h"
#include "GameInstance.h"
#include "Collider.h"

CCollision_Thread::CCollision_Thread(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

_uint APIENTRY EntryFunction(void* pArg)
{
	CCollision_Thread*			pCollisioner = (CCollision_Thread*)pArg;

	EnterCriticalSection(&pCollisioner->Get_CriticalSection());

	_uint		iProgress = 0;

	iProgress = pCollisioner->Collision_Check_Bomb_Rock();

	if (0 == iProgress)
	{
		LeaveCriticalSection(&pCollisioner->Get_CriticalSection());

		return iProgress;
	}

	iProgress = pCollisioner->Collision_Check_WaterPot_MagRock();

	if (0 == iProgress)
	{
		LeaveCriticalSection(&pCollisioner->Get_CriticalSection());

		return iProgress;
	}

	iProgress = pCollisioner->Collision_Check_Door_Lock();

	if (0 == iProgress)
	{
		LeaveCriticalSection(&pCollisioner->Get_CriticalSection());

		return iProgress;
	}

	iProgress = pCollisioner->Collision_Check_Fire_FireBrazier();

	if (0 == iProgress)
	{
		LeaveCriticalSection(&pCollisioner->Get_CriticalSection());

		return iProgress;
	}
	

	iProgress = pCollisioner->Collision_Check_Cube();

	if (0 == iProgress)
	{
		LeaveCriticalSection(&pCollisioner->Get_CriticalSection());

		return iProgress;
	}

	iProgress = pCollisioner->Collision_Check_MagmaZet_MagRock();

	if (0 == iProgress)
	{
		LeaveCriticalSection(&pCollisioner->Get_CriticalSection());

		return iProgress;
	}

	LeaveCriticalSection(&pCollisioner->Get_CriticalSection());

	return iProgress;
}


HRESULT CCollision_Thread::NativeConstruct()
{
	//CoInitializeEx(nullptr, 0);

	//InitializeCriticalSection(&m_CriticalSection);

	//m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryFunction, this, 0, nullptr);
	//if (0 == m_hThread)
	//	return E_FAIL;

	return S_OK;

}

HRESULT CCollision_Thread::Add_Collider(_uint iObjectID, CCollider * pCollider)
{
	m_ObjectColliderList[(OBJECTGROUP)iObjectID].push_back(pCollider);
	Safe_AddRef(pCollider);

	return S_OK;
}

_uint CCollision_Thread::Collision_Check_Bomb_Rock()
{
	if (!m_ObjectColliderList[OBJECT_ROCK].empty() && !m_ObjectColliderList[OBJECT_BOMB].empty())
	{
		if (0 < m_ObjectColliderList[OBJECT_BOMB].size() && 0 < m_ObjectColliderList[OBJECT_BOMB].size())
		{
			for (auto& pSourCube = m_ObjectColliderList[OBJECT_ROCK].begin(); pSourCube != m_ObjectColliderList[OBJECT_ROCK].end();)
			{
				for (auto& pDestCube = m_ObjectColliderList[OBJECT_BOMB].begin(); pDestCube != m_ObjectColliderList[OBJECT_BOMB].end();)
				{

					if ((*pSourCube)->Collision_SPHERE((*pDestCube), CCollider::TYPE_SPHERE))
					{
						(*pSourCube)->Set_Collision_OBB(true);
						break;
					}

					else
					{
						(*pSourCube)->Set_Collision_OBB(false);
					}

					++pDestCube;
				}

				++pSourCube;
			}

			for (auto& pClear : m_ObjectColliderList[OBJECT_ROCK])
			{
				if (nullptr != pClear)
				{
					Safe_Release(pClear);

				}

			}

			for (auto& pClear : m_ObjectColliderList[OBJECT_BOMB])
			{
				if (nullptr != pClear)
				{
					Safe_Release(pClear);

				}
			}

			m_ObjectColliderList[OBJECT_ROCK].clear();
			m_ObjectColliderList[OBJECT_BOMB].clear();
		}
	}

	else
	{
		if (!m_ObjectColliderList[OBJECT_ROCK].empty())
		{
			for (auto& pCollider : m_ObjectColliderList[OBJECT_ROCK])
			{
				Safe_Release(pCollider);
			}

			m_ObjectColliderList[OBJECT_ROCK].clear();
		}

		if (!m_ObjectColliderList[OBJECT_BOMB].empty())
		{
			for (auto& pCollider : m_ObjectColliderList[OBJECT_BOMB])
			{
				Safe_Release(pCollider);
			}


			m_ObjectColliderList[OBJECT_BOMB].clear();
		}
	}


	return 1;
	
}

_uint CCollision_Thread::Collision_Check_WaterPot_MagRock()
{
	if (!m_ObjectColliderList[OBJECT_WATERPOT].empty() && !m_ObjectColliderList[OBJECT_MAGMA].empty())
	{
		if (0 < m_ObjectColliderList[OBJECT_WATERPOT].size() && 0 < m_ObjectColliderList[OBJECT_MAGMA].size())
		{
			for (auto& pSourCube = m_ObjectColliderList[OBJECT_WATERPOT].begin(); pSourCube != m_ObjectColliderList[OBJECT_WATERPOT].end();)
			{
				for (auto& pDestCube = m_ObjectColliderList[OBJECT_MAGMA].begin(); pDestCube != m_ObjectColliderList[OBJECT_MAGMA].end();)
				{

					if ((*pSourCube)->Collision_OBB((*pDestCube), CCollider::TYPE_OBB))
					{
						(*pSourCube)->Set_Collision_OBB(true);
						break;
					}

					else
					{
						if (!m_ObjectColliderList[OBJECT_WATERPOT].empty() && !m_ObjectColliderList[OBJECT_MAGMA].empty())
						{
							if (0 < m_ObjectColliderList[OBJECT_WATERPOT].size())
							{
								(*pSourCube)->Set_Collision_OBB(false);
							}
						}
					}

					++pDestCube;
				}

				++pSourCube;
			}

			for (auto& pClear : m_ObjectColliderList[OBJECT_WATERPOT])
			{
				if (nullptr != pClear)
				{
					Safe_Release(pClear);

				}

			}

			for (auto& pClear : m_ObjectColliderList[OBJECT_MAGMA])
			{
				if (nullptr != pClear)
				{
					Safe_Release(pClear);

				}
			}

			m_ObjectColliderList[OBJECT_WATERPOT].clear();
			m_ObjectColliderList[OBJECT_MAGMA].clear();
		}
	}

	else
	{
		if (!m_ObjectColliderList[OBJECT_WATERPOT].empty())
		{
			for (auto& pCollider : m_ObjectColliderList[OBJECT_WATERPOT])
			{
				Safe_Release(pCollider);
			}

			m_ObjectColliderList[OBJECT_WATERPOT].clear();
		}

		if (!m_ObjectColliderList[OBJECT_MAGMA].empty())
		{
			for (auto& pCollider : m_ObjectColliderList[OBJECT_MAGMA])
			{
				Safe_Release(pCollider);
			}


			m_ObjectColliderList[OBJECT_MAGMA].clear();
		}
	}


	return 1;
}

_uint CCollision_Thread::Collision_Check_Door_Lock() 
{
	if (!m_ObjectColliderList[OBJECT_DOOR].empty() && !m_ObjectColliderList[OBJECT_LOCK].empty())
	{
		for (auto& pDoor : m_ObjectColliderList[OBJECT_DOOR])
		{
			for (auto& pLock : m_ObjectColliderList[OBJECT_LOCK])
			{
				if (pDoor->Collision_OBB(pLock, CCollider::TYPE_OBB))
				{
					pDoor->Set_Collision_OBB(true);

					std::cout << "Collision" << std::endl;
					break;
				}
			}

		}

		for (auto& pClear : m_ObjectColliderList[OBJECT_DOOR])
		{
			if (nullptr != pClear)
			{
				Safe_Release(pClear);

			}

		}

		for (auto& pClear : m_ObjectColliderList[OBJECT_LOCK])
		{
			if (nullptr != pClear)
			{
				Safe_Release(pClear);

			}
		}


		m_ObjectColliderList[OBJECT_DOOR].clear();
		m_ObjectColliderList[OBJECT_LOCK].clear();
	}
	else
	{
		if (!m_ObjectColliderList[OBJECT_DOOR].empty())
		{

			for (auto& pCollider : m_ObjectColliderList[OBJECT_DOOR])
			{
				Safe_Release(pCollider);
			}

			m_ObjectColliderList[OBJECT_DOOR].clear();
		}

		if (!m_ObjectColliderList[OBJECT_LOCK].empty())
		{
			for (auto& pCollider : m_ObjectColliderList[OBJECT_LOCK])
			{
				Safe_Release(pCollider);
			}

			m_ObjectColliderList[OBJECT_LOCK].clear();
		}
	}

	return 1;
}

_uint CCollision_Thread::Collision_Check_Fire_FireBrazier()
{
	if (!m_ObjectColliderList[OBJECT_FIRE].empty() && !m_ObjectColliderList[OBJECT_FIREBRAZIER].empty())
	{
		for (auto& pFireBrazier : m_ObjectColliderList[OBJECT_FIREBRAZIER])
		{
			for (auto& pFire : m_ObjectColliderList[OBJECT_FIRE])
			{
				if (pFireBrazier->Collision_SPHERE(pFire, CCollider::TYPE_SPHERE))
				{
					pFireBrazier->Set_Collision_SPHERE(true);

					std::cout << "Collision" << std::endl;
					break;
				}
			}
		}

		//for (auto& pClear : m_ObjectColliderList[OBJECT_FIRE])
		//{
		//	if (nullptr != pClear)
		//	{
		//		Safe_Release(pClear);

		//	}

		//}

		//m_ObjectColliderList[OBJECT_FIRE].clear();

		for (auto& pClear : m_ObjectColliderList[OBJECT_FIREBRAZIER])
		{
			if (nullptr != pClear)
			{
				Safe_Release(pClear);

			}
		}

		m_ObjectColliderList[OBJECT_FIREBRAZIER].clear();
	}

	else
	{

		//if (!m_ObjectColliderList[OBJECT_FIRE].empty())
		//{
		//	for (auto& pCollider : m_ObjectColliderList[OBJECT_FIRE])
		//	{
		//		Safe_Release(pCollider);
		//	}

		//	m_ObjectColliderList[OBJECT_FIRE].clear();
		//}

		if (!m_ObjectColliderList[OBJECT_FIREBRAZIER].empty())
		{
			for (auto& pCollider : m_ObjectColliderList[OBJECT_FIREBRAZIER])
			{
				Safe_Release(pCollider);
			}

			m_ObjectColliderList[OBJECT_FIREBRAZIER].clear();
		}
	}

	return 1;
}

_uint CCollision_Thread::Collision_Check_Box_FireBrazier()
{
	return 1;
}

_uint CCollision_Thread::Collision_Check_Cube()
{
	//if (1 < m_ObjectColliderList[OBJECT_CUBE].size())
	//{
	//	for (auto& pSourCube = m_ObjectColliderList[OBJECT_CUBE].begin(); pSourCube != m_ObjectColliderList[OBJECT_CUBE].end();)
	//	{
	//		for (auto& pDestCube = m_ObjectColliderList[OBJECT_CUBE].begin(); pDestCube != m_ObjectColliderList[OBJECT_CUBE].end();)
	//		{
	//			if ((*pSourCube) == (*pDestCube))
	//			{
	//				if (pSourCube != m_ObjectColliderList[OBJECT_CUBE].end() && pDestCube != m_ObjectColliderList[OBJECT_CUBE].end())
	//				{
	//					++pDestCube;
	//				}
	//				else if(pSourCube == m_ObjectColliderList[OBJECT_CUBE].end() && pDestCube == m_ObjectColliderList[OBJECT_CUBE].end())
	//				{
	//					break;
	//				}
	//			}

	//			else
	//			{
	//				if ((*pSourCube)->Collision_OBB((*pDestCube), CCollider::TYPE_OBB))
	//				{
	//					if ((*pSourCube)->Get_Pos().y > (*pDestCube)->Get_Pos().y)
	//					{
	//						(*pSourCube)->Set_Collision_OBB(true);
	//						std::cout << "Collision" << std::endl;
	//						break;
	//					}

	//					else
	//					{
	//						(*pSourCube)->Set_Collision_OBB(false);
	//					}


	//				}

	//				if (pDestCube != m_ObjectColliderList[OBJECT_CUBE].end() && !m_ObjectColliderList[OBJECT_CUBE].empty())
	//				{
	//					++pDestCube;
	//				}
	//			}
	//			
	//		}
	//		if (0 < m_ObjectColliderList[OBJECT_CUBE].size())
	//		{
	//			if (pSourCube != m_ObjectColliderList[OBJECT_CUBE].end() && !m_ObjectColliderList[OBJECT_CUBE].empty())
	//			{
	//				++pSourCube;
	//			}
	//		}
	//	}

	//	for (auto& pClear : m_ObjectColliderList[OBJECT_CUBE])
	//	{
	//		if (nullptr != pClear)
	//		{
	//			Safe_Release(pClear);

	//		}
	//	}

	//	m_ObjectColliderList[OBJECT_CUBE].clear();
	//}

	//else
	//{
	//	for (auto& pClear : m_ObjectColliderList[OBJECT_CUBE])
	//	{
	//		if (nullptr != pClear)
	//		{
	//			Safe_Release(pClear);

	//		}
	//	}

	//	m_ObjectColliderList[OBJECT_CUBE].clear();
	//}

	if (1.f < m_ObjectColliderList[OBJECT_CUBE].size())
	{
		for (auto& pSourCube : m_ObjectColliderList[OBJECT_CUBE])
		{
			if (nullptr == pSourCube)
			{
				break;
			}

			for (auto& pDestCube : m_ObjectColliderList[OBJECT_CUBE])
			{
				if (pSourCube == pDestCube)
				{
					continue;
				}

				if (pSourCube->Collision_OBB(pDestCube, CCollider::TYPE_OBB))
				{
					if (pSourCube->Get_Pos().y > pDestCube->Get_Pos().y)
					{
						pSourCube->Set_Collision_OBB(true);
						std::cout << "Collision" << std::endl;
					}

					else
					{
						pSourCube->Set_Collision_OBB(false);
					}

					break;
				}

			}
		}

		for (auto& pClear : m_ObjectColliderList[OBJECT_CUBE])
		{
			if (nullptr != pClear)
			{
				Safe_Release(pClear);

			}		
		}

		m_ObjectColliderList[OBJECT_CUBE].clear();
	}

	else
	{
		if (!m_ObjectColliderList[OBJECT_CUBE].empty())
		{
			for (auto& pClear : m_ObjectColliderList[OBJECT_CUBE])
			{
				if (nullptr != pClear)
				{
					Safe_Release(pClear);
				}		
			}
			m_ObjectColliderList[OBJECT_CUBE].clear();
		}
	}

	return 1;
}

_uint CCollision_Thread::Collision_Check_MagmaZet_MagRock()
{

	if (!m_ObjectColliderList[OBJECT_MAGROCK].empty() && !m_ObjectColliderList[OBJECT_MAGZET].empty())
	{
		for (auto& pSourCube = m_ObjectColliderList[OBJECT_MAGROCK].begin(); pSourCube != m_ObjectColliderList[OBJECT_MAGROCK].end();)
		{
			for (auto& pDestCube = m_ObjectColliderList[OBJECT_MAGZET].begin(); pDestCube != m_ObjectColliderList[OBJECT_MAGZET].end();)
			{

				if ((*pSourCube)->Collision_OBB((*pDestCube), CCollider::TYPE_OBB))
				{
					(*pSourCube)->Set_Collision_OBB(true);


					break;
				}

				else
				{
					(*pSourCube)->Set_Collision_OBB(false);
					break;
				}


				++pDestCube;
			}

			++pSourCube;
		}

		for (auto& pClear : m_ObjectColliderList[OBJECT_MAGROCK])
		{
			if (nullptr != pClear)
			{
				Safe_Release(pClear);

			}
		}

		for (auto& pClear : m_ObjectColliderList[OBJECT_MAGZET])
		{
			if (nullptr != pClear)
			{
				Safe_Release(pClear);

			}
		}
		
		m_ObjectColliderList[OBJECT_MAGROCK].clear();
		m_ObjectColliderList[OBJECT_MAGZET].clear();
	}

	else
	{
		if (!m_ObjectColliderList[OBJECT_MAGROCK].empty())
		{
			for (auto& pCollider : m_ObjectColliderList[OBJECT_MAGROCK])
			{
				Safe_Release(pCollider);
			}

			m_ObjectColliderList[OBJECT_MAGROCK].clear();
		}

		if (!m_ObjectColliderList[OBJECT_MAGZET].empty())
		{
			for (auto& pCollider : m_ObjectColliderList[OBJECT_MAGZET])
			{
				Safe_Release(pCollider);
			}

			m_ObjectColliderList[OBJECT_MAGZET].clear();
		}
	}

	return 1;
}

void CCollision_Thread::Check_Collision()
{
	CoInitializeEx(nullptr, 0);

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryFunction, this, 0, nullptr);
	if (0 == m_hThread)
		return;
}

CCollision_Thread * CCollision_Thread::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCollision_Thread* pInstance = new CCollision_Thread(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Error : Failed Created Collisioner"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Thread::Free()
{

	for (_uint i = 0; i < OBJECT_END; ++i)
	{
		for (auto& pCollider : m_ObjectColliderList[i])
		{
			if (nullptr != pCollider)
			{
				Safe_Release(pCollider);
			}
		}
		m_ObjectColliderList[i].clear();
	}

	WaitForSingleObject(m_hThread, INFINITE);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);
}

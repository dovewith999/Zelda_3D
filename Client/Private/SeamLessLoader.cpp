#include "stdafx.h"
#include "..\Public\SeamLessLoader.h"
#include "GameInstance.h"
#include <mutex>

#include "Dungeon_Room2.h"
#include "Dungeon_Room3.h"
#include "Dungeon_Room4.h"
#include "Dungeon_Room5_OutSide.h"
#include "Dungeon_Room6.h"
#include "Dungeon_Room7.h"
#include "Dungeon_Room8.h"
#include "Dungeon_Room9.h"
#include "Dungeon_Room10.h"
#include "Dungeon_RoomFinal.h"

CSeamLess_Loader::CSeamLess_Loader(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CSeamLess_Loader::NativeConstruct()
{
	return S_OK;
}

_uint APIENTRY EntryFunctionSeamLess(void* pArg)
{
	CSeamLess_Loader*			pSeamLessLoader = (CSeamLess_Loader*)pArg;

	EnterCriticalSection(&pSeamLessLoader->Get_CriticalSection());

	_uint		iProgress = 0;
	_uint iNextRoomID = 0;

	mutex mtx;
	mtx.lock();
	iNextRoomID = pSeamLessLoader->Get_RoomID();

	iProgress = pSeamLessLoader->SeamLessLoad(iNextRoomID);
	mtx.unlock();

	
	LeaveCriticalSection(&pSeamLessLoader->Get_CriticalSection());

	return iProgress;
}


void CSeamLess_Loader::Ready_NextRoom(_uint iRoomID)
{

	m_iNextRoomID = iRoomID;

	CoInitializeEx(nullptr, 0);

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryFunctionSeamLess, this, 0, nullptr);
	if (0 == m_hThread)
		return;
}

_uint CSeamLess_Loader::SeamLessLoad(_uint iRoomID)
{

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_matrix		PivotMatrix;

#pragma region ÁÖ¼®
	///*For.Prototype_GameObject_Dungeon_Room4*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room4"),
	//	CDungeon_Room4::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
	//	return -1;

	//cout << "Room4 Prototype Create Complate" << endl;

	//PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	///* For.Prototype_Component_Model_Dungeon_Room4. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room4"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_4/", "Room_4.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
	//	return E_FAIL;

	//cout << "Room4 Modle Create Complate" << endl;


	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_State"), TEXT("Prototype_GameObject_Dungeon_Room4"))))
	//	return E_FAIL;

	//cout << "Room4 Clone Complate" << endl;

	///*For.Prototype_GameObject_Dungeon_Room5_OutSide*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room5_OutSide"),
	//	CDungeon_Room5_OutSide::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
	//	return -1;

	//cout << "Room5 Prototype Create Complate" << endl;

	//PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f) * */ XMMatrixRotationY(XMConvertToRadians(90.0f));
	///* For.Prototype_Component_Model_Dungeon_Room5_OutSide. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room5_OutSide"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_5_OutSide/", "Room_5_OutSide.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
	//	return E_FAIL;


	//cout << "Room5 Modle Create Complate" << endl;


	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_State"), TEXT("Prototype_GameObject_Dungeon_Room5_OutSide"))))
	//	return E_FAIL;

	//cout << "Room5 Clone Complate" << endl;

	///*For.Prototype_GameObject_Dungeon_Room6*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room6"),
	//	CDungeon_Room6::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
	//	return -1;

	//cout << "Room6 Prototype Create Complate" << endl;

	//PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f) * */ XMMatrixRotationY(XMConvertToRadians(90.0f));
	///* For.Prototype_Component_Model_Dungeon_Room6. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room6"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_6/", "Room_6.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
	//	return E_FAIL;


	//cout << "Room6 Modle Create Complate" << endl;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_State"), TEXT("Prototype_GameObject_Dungeon_Room6"))))
	//	return E_FAIL;

	//cout << "Room6 Clone Complate" << endl;


	///*7~10*/


	///*For.Prototype_GameObject_Dungeon_Room7*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room7"),
	//	CDungeon_Room7::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
	//	return -1;

	//cout << "Room7 Prototype Create Complate" << endl;

	//PivotMatrix = XMMatrixScaling(0.0105f, 0.0105f, 0.0105f)*  XMMatrixRotationY(XMConvertToRadians(90.0f));
	///* For.Prototype_Component_Model_Dungeon_Room7. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room7"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_7/", "Room_7.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
	//	return E_FAIL;


	//cout << "Room7 Modle Create Complate" << endl;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_State"), TEXT("Prototype_GameObject_Dungeon_Room7"))))
	//	return E_FAIL;

	//cout << "Room7 Clone Complate" << endl;

	///*For.Prototype_GameObject_Dungeon_Room8*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room8"),
	//	CDungeon_Room8::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
	//	return -1;

	//cout << "Room8 Prototype Create Complate" << endl;

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	///* For.Prototype_Component_Model_Dungeon_Room8. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room8"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_8/", "Room_8.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
	//	return E_FAIL;


	//cout << "Room8 Modle Create Complate" << endl;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_State"), TEXT("Prototype_GameObject_Dungeon_Room8"))))
	//	return E_FAIL;

	//cout << "Room8 Clone Complate" << endl;

	///*For.Prototype_GameObject_Dungeon_Room9*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room9"),
	//	CDungeon_Room9::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
	//	return -1;

	//cout << "Room9 Prototype Create Complate" << endl;

	//PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f) * */ XMMatrixRotationY(XMConvertToRadians(90.0f));
	///* For.Prototype_Component_Model_Dungeon_Room9. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room9"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_9/", "Room_9.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
	//	return E_FAIL;


	//cout << "Room9 Modle Create Complate" << endl;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_State"), TEXT("Prototype_GameObject_Dungeon_Room9"))))
	//	return E_FAIL;

	//cout << "Room9 Clone Complate" << endl;

	///*For.Prototype_GameObject_Dungeon_Room10*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room10"),
	//	CDungeon_Room10::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
	//	return -1;

	//cout << "Room10 Prototype Create Complate" << endl;

	//PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f) * */ XMMatrixRotationY(XMConvertToRadians(90.0f));
	///* For.Prototype_Component_Model_Dungeon_Room10. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room10"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_10/", "Room_10.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
	//	return E_FAIL;


	//cout << "Room10 Modle Create Complate" << endl;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_State"), TEXT("Prototype_GameObject_Dungeon_Room10"))))
	//	return E_FAIL;

	//cout << "Room10 Clone Complate" << endl;

#pragma endregion

	switch (iRoomID)
	{
	//case 2:
	case 2:
		/*For.Prototype_GameObject_Dungeon_Room2*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room2"),
			CDungeon_Room2::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
			return -1;

		cout << "Room2 Prototype Create Complate" << endl;


		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
		/* For.Prototype_Component_Model_Dungeon_Room2. */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room2"),
			CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_2/", "Room_2.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
			return E_FAIL;

		cout << "Room2 Modle Create Complate" << endl;


		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_2"), TEXT("Prototype_GameObject_Dungeon_Room2"))))
			return E_FAIL;


		cout << "Room2 Clone Complate" << endl;

		break;
	//case 3:
	case 3:
		/*For.Prototype_GameObject_Dungeon_Room3*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room3"),
			CDungeon_Room3::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
			return -1;

		cout << "Room3 Prototype Create Complate" << endl;


		PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
		/* For.Prototype_Component_Model_Dungeon_Room3. */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room3"),
			CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_3/", "Room_3.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
			return E_FAIL;


		cout << "Room3 Modle Create Complate" << endl;

		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_3"), TEXT("Prototype_GameObject_Dungeon_Room3"))))
			return E_FAIL;


		cout << "Room3 Clone Complate" << endl;

		if (FAILED(pGameInstance->Release_GameObject(LEVEL_DUNGEON, TEXT("Layer_Room_0"))))
		{
			return E_FAIL;
		}
		cout << "Room0 Delete Complate" << endl;

		break;
	case 5:

		/*For.Prototype_GameObject_Dungeon_Room4*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room4"),
			CDungeon_Room4::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
			return -1;

		cout << "Room4 Prototype Create Complate" << endl;

		PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
		/* For.Prototype_Component_Model_Dungeon_Room4. */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room4"),
			CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_4/", "Room_4.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
			return E_FAIL;

		cout << "Room4 Modle Create Complate" << endl;


		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_4"), TEXT("Prototype_GameObject_Dungeon_Room4"))))
			return E_FAIL;

		cout << "Room4 Clone Complate" << endl;
		break;
	case 6:

		/*For.Prototype_GameObject_Dungeon_Room5_OutSide*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room5_OutSide"),
			CDungeon_Room5_OutSide::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
			return -1;

		cout << "Room5 Prototype Create Complate" << endl;

		PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f) * */ XMMatrixRotationY(XMConvertToRadians(90.0f));
		/* For.Prototype_Component_Model_Dungeon_Room5_OutSide. */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room5_OutSide"),
			CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_5_OutSide/", "Room_5_OutSide.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
			return E_FAIL;


		cout << "Room5 Modle Create Complate" << endl;


		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_5"), TEXT("Prototype_GameObject_Dungeon_Room5_OutSide"))))
			return E_FAIL;

		cout << "Room5 Clone Complate" << endl;

		if (FAILED(pGameInstance->Release_GameObject(LEVEL_DUNGEON, TEXT("Layer_Room_2"))))
		{
			return E_FAIL;
		}
		cout << "Room2 Delete Complate" << endl;

		if (FAILED(pGameInstance->Release_GameObject(LEVEL_DUNGEON, TEXT("Layer_Room_3"))))
		{
			return E_FAIL;
		}
		cout << "Room3 Delete Complate" << endl;

		break;
	case 7:

		/*For.Prototype_GameObject_Dungeon_Room6*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room6"),
			CDungeon_Room6::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
			return -1;

		cout << "Room6 Prototype Create Complate" << endl;

		PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f) * */ XMMatrixRotationY(XMConvertToRadians(90.0f));
		/* For.Prototype_Component_Model_Dungeon_Room6. */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room6"),
			CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_6/", "Room_6.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
			return E_FAIL;


		cout << "Room6 Modle Create Complate" << endl;

		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_6"), TEXT("Prototype_GameObject_Dungeon_Room6"))))
			return E_FAIL;

		cout << "Room6 Clone Complate" << endl;
		break;

		break;
	case 8:
		/*For.Prototype_GameObject_Dungeon_Room7*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room7"),
			CDungeon_Room7::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
			return -1;

		cout << "Room7 Prototype Create Complate" << endl;

		PivotMatrix = XMMatrixScaling(0.0105f, 0.0105f, 0.0105f)*  XMMatrixRotationY(XMConvertToRadians(90.0f));
		/* For.Prototype_Component_Model_Dungeon_Room7. */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room7"),
			CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_7/", "Room_7.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
			return E_FAIL;


		cout << "Room7 Modle Create Complate" << endl;

		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_7"), TEXT("Prototype_GameObject_Dungeon_Room7"))))
			return E_FAIL;

		cout << "Room7 Clone Complate" << endl;

		break;
	case 9:

		/*For.Prototype_GameObject_Dungeon_Room8*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room8"),
			CDungeon_Room8::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
			return -1;

		cout << "Room8 Prototype Create Complate" << endl;

		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
		/* For.Prototype_Component_Model_Dungeon_Room8. */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room8"),
			CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_8/", "Room_8.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
			return E_FAIL;


		cout << "Room8 Modle Create Complate" << endl;

		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_8"), TEXT("Prototype_GameObject_Dungeon_Room8"))))
			return E_FAIL;

		cout << "Room8 Clone Complate" << endl;
	
		break;
	case 10:
		/*For.Prototype_GameObject_Dungeon_Room9*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room9"),
			CDungeon_Room9::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
			return -1;

		cout << "Room9 Prototype Create Complate" << endl;

		PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f) * */ XMMatrixRotationY(XMConvertToRadians(90.0f));
		/* For.Prototype_Component_Model_Dungeon_Room9. */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room9"),
			CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_9/", "Room_9.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
			return E_FAIL;


		cout << "Room9 Modle Create Complate" << endl;

		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_9"), TEXT("Prototype_GameObject_Dungeon_Room9"))))
			return E_FAIL;

		cout << "Room9 Clone Complate" << endl;
		if (FAILED(pGameInstance->Release_GameObject(LEVEL_DUNGEON, TEXT("Layer_Room_6"))))
		{
			return E_FAIL;
		}
		cout << "Room6 Delete Complate" << endl;

		break;
	case 11:
		/*For.Prototype_GameObject_Dungeon_Room10*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room10"),
			CDungeon_Room10::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
			return -1;

		cout << "Room10 Prototype Create Complate" << endl;

		PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f) * */ XMMatrixRotationY(XMConvertToRadians(90.0f));
		/* For.Prototype_Component_Model_Dungeon_Room10. */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room10"),
			CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_10/", "Room_10.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
			return E_FAIL;


		cout << "Room10 Modle Create Complate" << endl;

		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_10"), TEXT("Prototype_GameObject_Dungeon_Room10"))))
			return E_FAIL;

		cout << "Room10 Clone Complate" << endl;


		if (FAILED(pGameInstance->Release_GameObject(LEVEL_DUNGEON, TEXT("Layer_Room_5"))))
		{
			return E_FAIL;
		}
		cout << "Room5 Delete Complate" << endl;

		break;

	default:
		break;
	}


	//cout << "ID of Next Room is " << iRoomID << endl;

	Safe_Release(pGameInstance);

	return 1;
}

CSeamLess_Loader * CSeamLess_Loader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSeamLess_Loader* pInstance = new CSeamLess_Loader(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Error : Failed Created SeamLessLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSeamLess_Loader::Free()
{
	
	WaitForSingleObject(m_hThread, INFINITE);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);
	
	CloseHandle(m_hThread);
}

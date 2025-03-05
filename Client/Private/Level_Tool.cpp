#include "stdafx.h"
#include "..\Public\Level_Tool.h"
#include "GameInstance.h"
#include "Camera_Tool.h"
#include "Tool.h"

#define FIRE_BRAZIER 0
#define DOOR 1
#define BOMB 2
#define BOX 3
#define CUBE 4 
#define ROCK 5
#define MAGMA 6
#define WATERPOT 7
#define WOODPLANKS 8
#define BRIDGE 9
#define LOCKDOOR 10
#define BOKOBLIN 11
#define TRIGGERLADDER 12
#define TRIGGERWALL 13
#define TRIGGERHANG 14
#define WOODDOOR 15
#define STONEBRIDGE 16
#define TRESUREBOXSHORT 17
#define MAGMAZET 18
#define TRIGGERBOSS 19

CLevel_Tool::CLevel_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Tool::NativeConstruct()
{

	if (FAILED(__super::NativeConstruct()))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_LightDesc()))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		return E_FAIL;
	}

	//if (FAILED(Ready_Layer_Npc(TEXT("Layer_Npc"))))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(Ready_Layer_Stage(TEXT("Layer_State"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
	if (m_bMakeRoom)
	{
		m_fTimeAcc += fTimeDelta;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	pGameInstance->Transform_ToWorldSpace();

	Safe_Release(pGameInstance);
}

HRESULT CLevel_Tool::Render()
{
	if (FAILED(__super::Render()))
	{
		return E_FAIL;
	}

	SetWindowText(g_hWnd, TEXT("툴 레벨임."));

	CTool* pTool = CTool::Get_Instance();
	Safe_AddRef(pTool);
	pTool->Tool_Navigation();
	Safe_Release(pTool);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_LightDesc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	/*LightDesc.eType = tagLightDesc::TYPE_POINT;
	LightDesc.vPosition = _float4(15.f, 5.f, 15.f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
	return E_FAIL;*/

	LightDesc.eType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinCX / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 2000.f;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Stage(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Stage_OutSet_Island"))))
		return E_FAIL;*/

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Stage_GTower"))))
		return E_FAIL;*/

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Stage_Dragon_Island"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room0"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room1"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room2"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room3"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room4"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room5_OutSide"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room6"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room7"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room8"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room9"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room10"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Dungeon_RoomFinal"))))
	//	return E_FAIL;

	Safe_Release(pGameInstance);
	

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Npc(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, pLayerTag, TEXT("Prototype_GameObject_Zelda"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CTool* pTool = CTool::Get_Instance();
	Safe_AddRef(pTool);

	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(TEXT("../Bin/Data/Object/ObjectMatrix_Dungeon.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (nullptr == hFile)
	{
		return E_FAIL;
	}

	while (true)
	{
		_float4x4 WorldMatrix;
		_uint ObjID;

		ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		ReadFile(hFile, &ObjID, sizeof(_uint), &dwByte, nullptr);

		switch (ObjID)
		{
		case FIRE_BRAZIER:
			if (5 > iNumFireBrazier)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_FireBrazier"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}

				++iNumFireBrazier;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}		
			break;

		case DOOR:
			if (16 > iNumDoor)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Door"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				++iNumDoor;

				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}
			break;

		case BOMB:
			if (4 > iNumBomb)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Bomb"), &WorldMatrix)))
				//{
				//	return E_FAIL;

				//}
				++iNumBomb;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}
			break;

		case BOX:
			if (2 > iNumBox)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_TresureBox"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				++iNumBox;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}
			break;

		case CUBE:
			if (6 > iNumCube)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Cube"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				++iNumCube;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}

			break;

		case ROCK:
			if (3 > iNumRock)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Rock"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				++iNumRock;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}

			break;

		case MAGMA:
			if (5 > iNumMagma)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Magma"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}

				++iNumMagma;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}
			break;

		case WATERPOT:
			if (8 > iNumWaterPot)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_WaterPot"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				++iNumWaterPot;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}

			break;

		case WOODPLANKS:
			if (10 > m_iNumWoodPlanks)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_WoodPlanks"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				++m_iNumWoodPlanks;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}
			break;

		case BRIDGE:
			if (147 > iNumBridge)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Bridge"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				++iNumBridge;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}
			break;

		case LOCKDOOR:
			if (3 > iNumLock)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Lock"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
				++iNumLock;
			}
			break;

		case BOKOBLIN:
			if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &WorldMatrix)))
			{
				return E_FAIL;
			}
			pTool->Push_Back_ObjectID(ObjID);
			pTool->Push_Back_WorldMatrix(WorldMatrix);
			break;

		case TRIGGERLADDER:
			if (0 == iNumTriggerLadder || 2 == iNumTriggerLadder)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Trigger_Ladder"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}
			++iNumTriggerLadder;
			break;

		case TRIGGERWALL:
			break;

		case TRIGGERHANG:
			break;

		case WOODDOOR:
			if (2 > iNumWoodDoor)
			{
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_WoodDoor"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}

				++iNumWoodDoor;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
			}
			break;

		case STONEBRIDGE:
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_StoneBridge"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}
				++iNumStoneBridge;
				pTool->Push_Back_ObjectID(ObjID);
				pTool->Push_Back_WorldMatrix(WorldMatrix);
				break;

		case TRESUREBOXSHORT :
			//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_TresureBoxShort"), &WorldMatrix)))
			//{
			//	return E_FAIL;
			//}
			//++iNumStoneBridge;
			pTool->Push_Back_ObjectID(ObjID);
			pTool->Push_Back_WorldMatrix(WorldMatrix);

			break;

		case MAGMAZET:
			//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_MagmaZet"), &WorldMatrix)))
			//{
			//	return E_FAIL;
			//}
			pTool->Push_Back_ObjectID(ObjID);
			pTool->Push_Back_WorldMatrix(WorldMatrix);

			break;

		case TRIGGERBOSS :
			//if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Trigger_Boss"), &WorldMatrix)))
			//{
			//	return E_FAIL;
			//}
			pTool->Push_Back_ObjectID(ObjID);
			pTool->Push_Back_WorldMatrix(WorldMatrix);
			break;
		}

		//로드할 때 세이브 데이터에 있던 데이터들을 다시 세이브 데이터에 넣어 다시 저장될 수 있도록 하는 코드

		/*pTool->Push_Back_ObjectID(ObjID);
		pTool->Push_Back_WorldMatrix(WorldMatrix);*/

		if (0 == dwByte)
		{
			break;
		}


	}

	CloseHandle(hFile);


	Safe_Release(pTool);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Tool * CLevel_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Error : Failed Create Level_Tool"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();
}

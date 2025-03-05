#include "stdafx.h"
#include "..\Public\Level_Dungeon.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "Item_Sheath.h"
#include "Item_SwordHandle.h"
#include "Item_Blade.h"
#include "Item_Shield.h"
#include "Item_Boko.h"
#include "Collider_GndSword.h"
#include "Level_Loading.h"

CLevel_Dungeon::CLevel_Dungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Dungeon::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_LightDesc()))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	{
		return E_FAIL;
	}


	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	{ // 안에서 터레인 생성 중
		return E_FAIL;
	}

	/*if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	{
	return E_FAIL;
	}*/

	//if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Stage()))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
	{
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	pGameInstance->StopAll();

	pGameInstance->PlayBGM(L"Dungeon.mp3", 0.3f);
	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Dungeon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	if (GetKeyState(VK_NUMPAD2) & 0x8000)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_DUNGEON_FINAL))))
			return;
	}

	Safe_Release(pGameInstance);
	
#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG


}

HRESULT CLevel_Dungeon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	Compute_FPS();
#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_LightDesc()
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
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Camera(const _tchar * pLayerTag)
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
	CameraDesc.fFar = 1000.f;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Stage()
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Stage_OutSet_Island"))))
	return E_FAIL;*/

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Stage_GTower"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Stage_Dragon_Island"))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_0"), TEXT("Prototype_GameObject_Dungeon_Room0"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Room_1"), TEXT("Prototype_GameObject_Dungeon_Room1"))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Player(const _tchar * pLayerTag)
{

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_OBJECT_STATIC, pLayerTag, TEXT("Prototype_GameObject_Player"))))
	//	return E_FAIL;

	CItem_Sheath::ITEMDESC ItemDesc_Sheath;
	ZeroMemory(&ItemDesc_Sheath, sizeof(CItem_Sheath::ITEMDESC));

	ItemDesc_Sheath.eLevelID = LEVEL_DUNGEON;
	ItemDesc_Sheath.pLayerTag = pLayerTag;
	ItemDesc_Sheath.pComTag = TEXT("Com_Model");
	ItemDesc_Sheath.pBoneName = "cl_cl_podA                     ";

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Sheath"), &ItemDesc_Sheath)))
		return E_FAIL;

	CItem_SwordHandle::ITEMDESC ItemDesc_Handle;
	ZeroMemory(&ItemDesc_Handle, sizeof(CItem_SwordHandle::ITEMDESC));

	ItemDesc_Handle.eLevelID = LEVEL_DUNGEON;
	ItemDesc_Handle.pLayerTag = pLayerTag;
	ItemDesc_Handle.pComTag = TEXT("Com_Model");
	ItemDesc_Handle.pBoneName = "cl_cl_LhandA                   ";

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Item_SwordHandle"), TEXT("Prototype_GameObject_Item_SwordHandle"), &ItemDesc_Handle)))
		return E_FAIL;

	CItem_Blade::ITEMDESC ItemDesc_Blade;
	ZeroMemory(&ItemDesc_Blade, sizeof(CItem_Blade::ITEMDESC));

	ItemDesc_Blade.eLevelID = LEVEL_DUNGEON;
	ItemDesc_Blade.pLayerTag = pLayerTag;
	ItemDesc_Blade.pComTag = TEXT("Com_Model");
	ItemDesc_Blade.pBoneName = "cl_cl_LhandA                   ";

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Item_Blade"), TEXT("Prototype_GameObject_Item_Blade"), &ItemDesc_Blade)))
		return E_FAIL;

	CItem_Shield::ITEMDESC ItemDesc_Shield;
	ZeroMemory(&ItemDesc_Shield, sizeof(CItem_Shield::ITEMDESC));

	ItemDesc_Shield.eLevelID = LEVEL_DUNGEON;
	ItemDesc_Shield.pLayerTag = pLayerTag;
	ItemDesc_Shield.pComTag = TEXT("Com_Model");
	ItemDesc_Shield.pBoneName = "cl_cl_RhandA                   ";

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Item_Shield"), TEXT("Prototype_GameObject_Item_Shield"), &ItemDesc_Shield)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	for (_uint i = 0; i < 2; ++i)
	{
		if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_UI_AttentionBar"), &_float2(640.f, -25.f + (i * 770.f)))))
		{
			Safe_Release(pGameInstance);

			return E_FAIL;

		}
	}

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Particle_Bomb"))))
	//{
	//	Safe_Release(pGameInstance);

	//	return E_FAIL;

	//}
	//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_UI_BlackOut"), TEXT("Prototype_GameObject_UI_BlackOut"))))
	//{
	//	Safe_Release(pGameInstance);

	//	return E_FAIL;
	//}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Monster"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_NPC(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Zelda"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, pLayerTag, TEXT("Prototype_GameObject_Object_StatueCube"))))
	{
		return E_FAIL;
	}

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
		case 0:
			if (5 > iNumFireBrazier)
			{
				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_FireBrazier"), &WorldMatrix)))
				{
					return E_FAIL;
				}
				++iNumFireBrazier;
			}
			break;

		case 1:
			if (16 > iNumDoor)
			{
				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Door"), &WorldMatrix)))
				{
					return E_FAIL;
				}
				++iNumDoor;
			}
			break;

		case 2:
			if (4 > iNumBomb)
			{
				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Bomb"), &WorldMatrix)))
				{
					return E_FAIL;

				}	
				++iNumBomb;
			}
			break;

		case 3:
			if (2 > iNumBox)
			{
				/*if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_TresureBox"), &WorldMatrix)))
				{
					return E_FAIL;
				}*/

				if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_TresureBoxShort"), &WorldMatrix)))
				{
					return E_FAIL;
				}

				++iNumBox;
			}
			break;

		case 4:
			if (6 > iNumCube)
			{
				switch (iNumCube)
				{
				case 2:
					break;
				case 3:
					WorldMatrix._41 = -19.39f;
					WorldMatrix._42 = 22.5f;
					WorldMatrix._43 = 108.25f;
					break;
				case 4:
					WorldMatrix._41 = -19.39f;
					WorldMatrix._42 = 25.5f;
					WorldMatrix._43 = 108.25f;
					break;
				case 5:
					WorldMatrix._41 = -19.39f;
					WorldMatrix._42 = 28.5f;
					WorldMatrix._43 = 108.25f;
					break;
				default:
					break;
				}

				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Cube"), &WorldMatrix)))
				{
					return E_FAIL;
				}
				++iNumCube;
			}
			break;

		case 5:
			if (3 > iNumRock)
			{
				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Rock"), &WorldMatrix)))
				{
					return E_FAIL;
				}
				++iNumRock;
			}
			break;

		case 6:
			if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Magma"), &WorldMatrix)))
			{
				return E_FAIL;
			}
			break;

		case 7:
			if (8 > iNumWaterPot)
			{
				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_WaterPot"), &WorldMatrix)))
				{
					return E_FAIL;
				}
				++iNumWaterPot;
			}
			break;

		case 8:
			if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_WoodPlanks"), &WorldMatrix)))
			{
				return E_FAIL;
			}
			break;

		case 9:
			if (147 > iNumBridge)
			{
				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Bridge"), &WorldMatrix)))
				{
					return E_FAIL;
				}
				++iNumBridge;
			}
			break;

		case 10:
			if (3 > iNumLock)
			{
				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Lock"), &WorldMatrix)))
				{
					return E_FAIL;
				}
				++iNumLock;
			}
			break;
		case 11:
			/*if (3 > iNumMonster)
			{*/
				//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &WorldMatrix)))
				//{
				//	return E_FAIL;
				//}

				//CItem_Boko::ITEMDESC ItemDesc_Boko;
				//ZeroMemory(&ItemDesc_Boko, sizeof(CItem_Boko::ITEMDESC));

				//ItemDesc_Boko.eLevelID = LEVEL_DUNGEON;
				//ItemDesc_Boko.pLayerTag = TEXT("Layer_Monster");
				//ItemDesc_Boko.pComTag = TEXT("Com_Model");
				//ItemDesc_Boko.pBoneName = "bk_buki                        ";
				//ItemDesc_Boko.iIndex = iNumMonster;

				//if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Boko"), &ItemDesc_Boko)))
				//{
				//	return E_FAIL;
				//}

				//++iNumMonster;
			//}
			break;

		case 12:

			if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Trigger_Ladder"), &WorldMatrix)))
			{
				return E_FAIL;
			}
			
			break;

		case 17:
			if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_TresureBoxShort"), &WorldMatrix)))
			{
				return E_FAIL;
			}

			break;


		case 18:
			if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_MagmaZet"), &WorldMatrix)))
			{
				return E_FAIL;
			}
			break;
		}


		if (0 == dwByte)
		{
			break;
		}
	}

	_float4x4 WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._41 = -45.f;
	WorldMatrix._42 = 3.f;
	WorldMatrix._43 = 98.f;
	WorldMatrix._44 = 1.f;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Trigger_Ladder"), &WorldMatrix)))
	{
		return E_FAIL;
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}
#ifdef _DEBUG
HRESULT CLevel_Dungeon::Compute_FPS()
{
	++m_iNumDraw;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumDraw);
		m_fTimeAcc = 0.f;
		m_iNumDraw = 0;
	}

	SetWindowText(g_hWnd, m_szFPS);

	return S_OK;	
}
#endif // _DEBUG


CLevel_Dungeon * CLevel_Dungeon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Dungeon*	pInstance = new CLevel_Dungeon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Failed to Created : CLevel_Dungeon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Dungeon::Free()
{
	__super::Free();

}

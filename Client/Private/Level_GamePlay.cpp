#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "Item_Sheath.h"
#include "Item_SwordHandle.h"
#include "Item_Blade.h"
#include "Item_Shield.h"

#include "Collider_GndSword.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_GamePlay::NativeConstruct()
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
	
	if (FAILED(Ready_Layer_BossMonster(TEXT("Layer_BossMonster"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Stage(TEXT("Layer_Stage"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("게임플레이레벨임."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LightDesc()
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

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
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
	CameraDesc.fNear= 0.2f;
	CameraDesc.fFar = 2000.f;

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Stage(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Stage_OutSet_Island"))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Stage_GTower"))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Stage_Dragon_Island"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Dungeon_Room0"))))
	//	return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;


	CItem_Sheath::ITEMDESC ItemDesc_Sheath;
	ZeroMemory(&ItemDesc_Sheath, sizeof(CItem_Sheath::ITEMDESC));

	ItemDesc_Sheath.eLevelID = LEVEL_GAMEPLAY;
	ItemDesc_Sheath.pLayerTag = pLayerTag;
	ItemDesc_Sheath.pComTag = TEXT("Com_Model");
	ItemDesc_Sheath.pBoneName = "cl_cl_podA                     ";

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item_Sheath"), &ItemDesc_Sheath)))
		return E_FAIL;

	CItem_SwordHandle::ITEMDESC ItemDesc_Handle;
	ZeroMemory(&ItemDesc_Handle, sizeof(CItem_SwordHandle::ITEMDESC));

	ItemDesc_Handle.eLevelID = LEVEL_GAMEPLAY;
	ItemDesc_Handle.pLayerTag = pLayerTag;
	ItemDesc_Handle.pComTag = TEXT("Com_Model");
	ItemDesc_Handle.pBoneName = "cl_cl_LhandA                   ";

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Item_SwordHandle"), TEXT("Prototype_GameObject_Item_SwordHandle"), &ItemDesc_Handle)))
		return E_FAIL;

	CItem_Blade::ITEMDESC ItemDesc_Blade;
	ZeroMemory(&ItemDesc_Blade, sizeof(CItem_Blade::ITEMDESC));

	ItemDesc_Blade.eLevelID = LEVEL_GAMEPLAY;
	ItemDesc_Blade.pLayerTag = pLayerTag;
	ItemDesc_Blade.pComTag = TEXT("Com_Model");
	ItemDesc_Blade.pBoneName = "cl_cl_LhandA                   ";

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Item_Blade"), TEXT("Prototype_GameObject_Item_Blade"), &ItemDesc_Blade)))
		return E_FAIL;

	CItem_Shield::ITEMDESC ItemDesc_Shield;
	ZeroMemory(&ItemDesc_Shield, sizeof(CItem_Shield::ITEMDESC));

	ItemDesc_Shield.eLevelID = LEVEL_GAMEPLAY;
	ItemDesc_Shield.pLayerTag = pLayerTag;
	ItemDesc_Shield.pComTag = TEXT("Com_Model");
	ItemDesc_Shield.pBoneName = "cl_cl_RhandA                   ";

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Item_Shield"), TEXT("Prototype_GameObject_Item_Shield"), &ItemDesc_Shield)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	for(_uint i = 0; i < 2; ++i)
	{
		if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI_AttentionBar"), &_float2(640.f, -25.f + (i * 770.f)))))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;	
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Monster"))))
		return E_FAIL;
	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BossMonster(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);
/*
	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_BossMonster"))))
		return E_FAIL;*/

	//CCollider_GndSword::ITEMDESC ItemDesc_GndSword;
	//ZeroMemory(&ItemDesc_GndSword, sizeof(CCollider_GndSword::ITEMDESC));

	//ItemDesc_GndSword.eLevelID = LEVEL_GAMEPLAY;
	//ItemDesc_GndSword.pLayerTag = pLayerTag;
	//ItemDesc_GndSword.pComTag = TEXT("Com_Model");
	//ItemDesc_GndSword.pBoneName = "gnd_kenL                       ";

	//if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Collider_GndSword"), &ItemDesc_GndSword)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_NPC(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Layer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Zelda"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}

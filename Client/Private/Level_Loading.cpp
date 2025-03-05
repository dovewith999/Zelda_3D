#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Tool.h"
#include "Level_Dungeon.h"
#include "Level_Dungeon_Final.h"

#include "UI_LoadingBackGround.h"
#include "UI_LoadingCircle.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Loading::NativeConstruct(LEVEL eNextLevelID)
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;


	if (FAILED(Ready_UI_Loading(TEXT("Layer_Loading"))))
	{
		return E_FAIL;
	}

	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (true == m_pLoader->Get_isFinished())
	{
		if (GetKeyState(VK_RETURN) & 0x8000)
		{
			CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			CLevel*		pLevel = nullptr;			

			pGameInstance->Set_NextLevelID(m_eNextLevelID);

			switch (m_eNextLevelID)
			{
			case LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_TOOL:
				pLevel = CLevel_Tool::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_DUNGEON:
				pLevel = CLevel_Dungeon::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_DUNGEON_FINAL:
				pLevel = CLevel_Dungeon_Final::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL_BOSSROOM:
				break;
			}

			if (nullptr == pLevel)
				return;

			pGameInstance->Open_Level(m_eNextLevelID, pLevel);

			Safe_Release(pGameInstance);
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, m_pLoader->Get_String());

	return S_OK;
}

HRESULT CLevel_Loading::Ready_UI_Loading(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (nullptr == pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI_Loading_BackGround")))
	{
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_BackGround"),
			CUI_Loading_BackGround::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(90.0f))))))
			return -1;
	}

	if (nullptr == pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI_Loading_Circle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_Circle"),
			CUI_Loading_Circle::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(90.0f))))))
			return -1;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_UI_Loading"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Loading/UI_Loading_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_UI_Loading_BackGround"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Loading/UI_LoadingBackGround.jpg")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_UI_LoadingBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Loading/UI_LoadingBar.png")))))
		return E_FAIL;

	/* For.UI_Loading_BackGround */
	if (FAILED(pGameInstance->Add_Layer(LEVEL_LOADING, pLayerTag, TEXT("Prototype_GameObject_UI_Loading_BackGround"))))
		return E_FAIL;

	/* For.UI_Loading_Circle */
	if (FAILED(pGameInstance->Add_Layer(LEVEL_LOADING, pLayerTag, TEXT("Prototype_GameObject_UI_Loading_Circle"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevelID)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevelID)))
	{
		MSGBOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}

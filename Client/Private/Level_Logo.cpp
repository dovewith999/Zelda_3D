#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Logo::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	/* For. Layer_UI_Logo*/
	if (FAILED(Ready_Layer_UI_MainLogo(TEXT("Layer_UI_Logo"))))
	{
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);
	pGameInstance->PlayBGM(L"Logo.mp3", 0.2f);
	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= 2.f)
	{
		if (GetKeyState(VK_RETURN) & 0x8000)
		{
			CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			pGameInstance->PlaySoundW(L"PressStart.wav", SOUND_EFFECT, 0.2f);

			if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_TOOL))))
				return;

			Safe_Release(pGameInstance);
		}
	}

	
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("로고레벨임."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI_MainLogo(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/* For.UI_LogoBackGround */
	if (FAILED(pGameInstance->Add_Layer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_UI_LogoBackGround"))))
		return E_FAIL;

	/* For.UI_Title */
	if (FAILED(pGameInstance->Add_Layer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_UI_TitleLogo"))))
		return E_FAIL;

	/* For.UI_SubLogo */
	if (FAILED(pGameInstance->Add_Layer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_UI_SubLogo"))))
		return E_FAIL;

	/* For.UI_PressStart */
	if (FAILED(pGameInstance->Add_Layer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_UI_PressStart"))))
		return E_FAIL;

	/* For.UI_MainLogo */
	if (FAILED(pGameInstance->Add_Layer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_UI_MainLogo"))))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}

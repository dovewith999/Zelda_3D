#include "..\Public\GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pPicking(CPicking::Get_Instance())
	, m_pFrustum(CFrustum::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pSound_Manager(CSound_Manager::Get_Instance())
{
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pSound_Manager);
}


HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const CGraphic_Device::GRAPHICDEVDESC& GraphicDesc, ID3D11Device** ppOutDevice, ID3D11DeviceContext** ppOutDeviceContext)
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	/* 그래픽 장치 초기화 */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppOutDevice, ppOutDeviceContext)))
		return E_FAIL;

	/* 입력장치 초기화. */
	if (FAILED(m_pInput_Device->Initialize_Input_Device(hInstance, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 사운드장치 초기화. */

	/* 각각의 매니져들의 예약. */
	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pPicking->NativeConstruct(*ppOutDevice, *ppOutDeviceContext, GraphicDesc.hWnd, GraphicDesc.iWinCX, GraphicDesc.iWinCY)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(*ppOutDevice, *ppOutDeviceContext)))
	{
		return E_FAIL;
	}


	m_pSound_Manager->Initialize_Manager();

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Update_InputDeviceState();

	m_pObject_Manager->Tick(fTimeDelta);

	m_pPipeLine->Tick();

	m_pObject_Manager->Late_Tick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
}

HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();	
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager || 
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);

	return S_OK;
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}


_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_long CGameInstance::Get_DIMMoveState(CInput_Device::DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

_byte CGameInstance::Get_DIMButtonState(CInput_Device::DIMB eMouseButtonID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMButtonState(eMouseButtonID);
}

_vector CGameInstance::Get_MousePos()
{
	if (nullptr == m_pInput_Device)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	return m_pInput_Device->Get_MousePos();
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);	
}

_float CGameInstance::Compute_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Compute_TimeDelta(pTimerTag);
	
}

HRESULT CGameInstance::Open_Level(_uint iLevelID, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelID, pNewLevel);
}

const _uint & CGameInstance::Get_NextLevelID() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	if (nullptr == m_pLevel_Manager)
	{
		return 0;
	}

	return m_pLevel_Manager->Get_NextLevelID();
}

void CGameInstance::Set_NextLevelID(_uint iNextLevelID)
{
	if (nullptr == m_pLevel_Manager)
	{
		return;
	}

	m_pLevel_Manager->Set_NextLevelID(iNextLevelID);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_Layer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Layer(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

CGameObject * CGameInstance::Find_GameObject(_uint iLavelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		return nullptr;
	}

	return m_pObject_Manager->Find_GameObject(iLavelIndex, pLayerTag);
}

HRESULT CGameInstance::Release_GameObject(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		return E_FAIL;
	}

	if (FAILED(m_pObject_Manager->Release_GameObject(iLevelIndex, pLayerTag)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CComponent * CGameInstance::Get_ComponentPtr(_uint iLevelID, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
	{
		return nullptr;
	}

	return m_pObject_Manager->Get_ComponentPtr(iLevelID, pLayerTag, pComponentTag, iIndex);
}

CLayer * CGameInstance::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		return nullptr;
	}

	return m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Find_Prototype(const _tchar * pPrototypeTag)
{
	if (nullptr == m_pObject_Manager)
	{
		return nullptr;
	}

	return m_pObject_Manager->Find_Prototype(pPrototypeTag);
}

_uint CGameInstance::Get_Size(_uint iLavelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		return 0;
	}

	return m_pObject_Manager->Get_Size(iLavelIndex, pLayerTag);
}


HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

_float4 CGameInstance::Get_CamPositionFloat4() const
{
	if (nullptr == m_pPipeLine)
		return _float4(0.f, 0.f, 0.f, 0.f);

	return m_pPipeLine->Get_CamPositionFloat4();	
}

_vector CGameInstance::Get_CamPositionVector() const
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_CamPositionVector();
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eStateType);	
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eStateType);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eStateType);
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex) const
{
	if (nullptr == m_pLight_Manager)
		return nullptr; 

	return m_pLight_Manager->Get_LightDesc(iIndex);	
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pDeviceContext, LightDesc);
}

void CGameInstance::Set_LightDiffuse(_uint iIndex, _float4 vDiffuse)
{
	if (nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->Set_LightDiffuse(iIndex, vDiffuse);
}

void CGameInstance::Transform_ToWorldSpace()
{
	if (nullptr == m_pPicking)
	{
		return;
	}

	m_pPicking->Transform_ToWorldSpace();
}

bool CGameInstance::Picking(CVIBuffer * pVIBuffer, CTransform * pTransform, _float4 * pOut)
{
	if (nullptr == m_pPicking)
	{
		return false;
	}

	return m_pPicking->Picking(pVIBuffer, pTransform, pOut);
}

_bool CGameInstance::isIn_FrustumInWorldSpace(_fvector vWorldPos, _float fRadius)
{
	if (nullptr == m_pFrustum)
	{
		return false;
	}

	return m_pFrustum->isIn_FrustumInWorldSpace(vWorldPos, fRadius);
}

void CGameInstance::PlaySound(TCHAR * pSoundKey, CHANNELID eID, float fVolume)
{
	if (nullptr == m_pSound_Manager)
	{
		return;
	}

	m_pSound_Manager->PlaySoundW(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(TCHAR * pSoundKey, float fVolume)
{
	if (nullptr == m_pSound_Manager)
	{
		return;
	}
	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
	{
		return;
	}
	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	if (nullptr == m_pSound_Manager)
	{
		return;
	}
	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	if (nullptr == m_pSound_Manager)
	{
		return;
	}

	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

void CGameInstance::Release_Engine()
{	
	CGameInstance::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();	

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CPicking::Get_Instance()->Destroy_Instance();

	CFrustum::Get_Instance()->Destroy_Instance();

	CPipeLine::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();

	CLight_Manager::Get_Instance()->Destroy_Instance();

	CTarget_Manager::Get_Instance()->Destroy_Instance();

	CGraphic_Device::Get_Instance()->Destroy_Instance();	

	
}

void CGameInstance::Free()
{
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pPicking);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);	
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}

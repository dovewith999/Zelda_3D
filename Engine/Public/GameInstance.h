#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Picking.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Frustum.h"
#include "Target_Manager.h"
#include "Sound_Manager.h"


BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
public:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* ��ġ�� ����� �̿��ϱ����Ͽ� �ʼ������� ����(�ʱ�ȭ)�Ǿ���� �ϵ��� �����ϳ�. */
	HRESULT Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const CGraphic_Device::GRAPHICDEVDESC& GraphicDesc, ID3D11Device** ppOutDevice, ID3D11DeviceContext** ppOutDeviceContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Render_Engine();
	HRESULT Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */	
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Input_Device */
	_byte Get_DIKeyState(_ubyte byKeyID);
	_long Get_DIMMoveState(CInput_Device::DIMM eMouseMoveID);
	_byte Get_DIMButtonState(CInput_Device::DIMB eMouseButtonID);
	_vector Get_MousePos();

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_float Compute_TimeDelta(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelID, class CLevel* pNewLevel);
	const _uint& Get_NextLevelID() const;
	void Set_NextLevelID(_uint iNextLevelID);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Layer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	class CGameObject* Find_GameObject(_uint iLavelIndex, const _tchar* pLayerTag);
	HRESULT Release_GameObject(_uint iLevelIndex, const _tchar* pLayerTag);
	class CComponent* Get_ComponentPtr(_uint iLevelID, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	_uint Get_Size(_uint iLavelIndex, const _tchar* pLayerTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);


public: /* For.PipeLine */
	_float4 Get_CamPositionFloat4() const;
	_vector Get_CamPositionVector() const;
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATETYPE eStateType);
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATETYPE eStateType);

public: /* For.Light */
	const LIGHTDESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	void Set_LightDiffuse(_uint iIndex, _float4 vDiffuse);

public: /* For.Picking */
	void Transform_ToWorldSpace();
	bool Picking(class CVIBuffer* pVIBuffer, class CTransform* pTransform, _float4* pOut);

public: /* For.Frustum */
	_bool isIn_FrustumInWorldSpace(_fvector vWorldPos, _float fRadius = 0.f);

public: /* For.Sound_Manager */
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	CGraphic_Device*			m_pGraphic_Device = nullptr;
	CInput_Device*				m_pInput_Device = nullptr;
	CTimer_Manager*				m_pTimer_Manager = nullptr;
	CLevel_Manager*				m_pLevel_Manager = nullptr;
	CObject_Manager*			m_pObject_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;
	CLight_Manager*				m_pLight_Manager = nullptr;
	CPicking*					m_pPicking = nullptr;
	CFrustum*					m_pFrustum = nullptr;
	CTarget_Manager*			m_pTarget_Manager = nullptr;
	CSound_Manager*				m_pSound_Manager = nullptr;

public:
	static void Release_Engine();
	virtual void Free() override;
};

END
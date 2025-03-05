#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CTool;
class CSeamLess_Loader;
class CLevel_Tool final : public CLevel
{
private:
	explicit CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT NativeConstruct();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_LightDesc();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Stage(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	//HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	//HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Npc(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Object(const _tchar* pLayerTag);
	//HRESULT Ready_Layer_BossMonster(const _tchar* pLayerTag);
	//HRESULT Ready_Layer_NPC(const _tchar* pLayerTag);

private: // 오브젝트 카운트 용
	_uint iNumCube = 0;
	_uint iNumBridge = 0;
	_uint iNumBox = 0;
	_uint iNumFireBrazier = 0;
	_uint iNumRock = 0;
	_uint iNumBomb = 0;
	_uint iNumBFlower = 0;
	_uint iNumDoor = 0;
	_uint iNumWaterPot = 0;
	_uint iNumLock = 0;
	_uint iNumTriggerLadder = 0;
	_uint iNumWoodDoor = 0;
	_uint iNumStoneBridge = 0;
	_uint iNumMagma = 0;
	_uint m_iNumWoodPlanks = 0;

private:
	_bool m_bMakeRoom = true;
	_float m_fTimeAcc = 0.f;

public:
	static CLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};
END
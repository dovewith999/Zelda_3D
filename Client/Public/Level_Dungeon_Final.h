#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CLevel_Dungeon_Final final : public CLevel
{
private:
	explicit CLevel_Dungeon_Final(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Dungeon_Final() = default;

public:
	virtual HRESULT NativeConstruct();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_LightDesc();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Stage(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_NPC(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Object(const _tchar* pLayerTag);

#ifdef _DEBUG
private:
	_tchar			m_szFPS[MAX_PATH] = TEXT("");
	_uint			m_iNumDraw = 0;
	_float			m_fTimeAcc = 0.f;
#endif // _DEBUG

#ifdef _DEBUG
private:
	HRESULT Compute_FPS();
#endif // _DEBUG

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

public:
	static CLevel_Dungeon_Final* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END


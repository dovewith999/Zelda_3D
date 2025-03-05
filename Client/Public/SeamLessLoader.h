#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CSeamLess_Loader final : public CBase
{

private:
	explicit CSeamLess_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CSeamLess_Loader() = default;

public:
	CRITICAL_SECTION Get_CriticalSection() const {
		return m_CriticalSection;
	}

	const _tchar* Get_String() const {
		return m_szLoadingText;
	}

	_bool Get_isFinished() const {
		return m_isFinished;
	}

	_uint Get_RoomID() const
	{
		return m_iNextRoomID;
	}

public:
	HRESULT NativeConstruct();

public:
	void Ready_NextRoom(_uint iRoomID);
	_uint SeamLessLoad(_uint iRoomID);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CriticalSection;

private:
	_uint m_iNextRoomID = 0;

	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");
	_bool				m_isFinished = false;

public:
	static CSeamLess_Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END
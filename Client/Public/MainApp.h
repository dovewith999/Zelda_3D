#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CCollider_Manager;
class CGameInstance;
END

BEGIN(Client)
class CTool;
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT NativeConstruct();
	void Tick(_double TimeDelta);
	HRESULT Render();

private:
	CGameInstance*				m_pGameInstance = nullptr;
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	CRenderer*					m_pRenderer = nullptr;
	CCollider_Manager*			m_pColliderManager = nullptr;
	CTool*						m_pTool = nullptr;

public:
	HRESULT Open_Level(LEVEL eLevel);
	HRESULT Ready_Prototype_Component_Static();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END
#pragma once

#include "Base.h"

/* 클라이언트에서 정의할 레벨 클래스들의 부모가 되는 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	explicit CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT NativeConstruct();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;


public:
	virtual void Free() override;
};

END
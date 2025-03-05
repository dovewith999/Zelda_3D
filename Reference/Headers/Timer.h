#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
public:
	CTimer();
	virtual ~CTimer() = default;

public:
	HRESULT NativeConstruct();
	_double Compute_TimeDelta();

private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;

	_double				m_TimeDelta = 0.0;

public:
	static CTimer* Create();
	virtual void Free() override;
};

END


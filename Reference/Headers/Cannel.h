#pragma once
#include "Base.h"

BEGIN(Engine)
class CCannel final : public CBase
{
private:
	CCannel();
	virtual ~CCannel() override = default;

public:
	HRESULT NativeConstruct();

public:
	static CCannel* Create();
	virtual void Free() override;
};
END

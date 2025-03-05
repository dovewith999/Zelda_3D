#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

/* ��ü�� ���۷��� ī��Ʈ�� �����Ѵ�. */

class ENGINE_DLL CBase abstract
{	
protected:
	CBase() = default;
	virtual ~CBase() = default;

public:
	unsigned long AddRef(); /* ���۷���ī��Ʈ�� ������Ű��. */
	unsigned long Release();/* ���۷���ī��Ʈ�� ���ҽ�Ų��. or �����Ѵ�. */

private:
	unsigned long			m_dwRefCnt = 0;

public:
	virtual void Free() = 0;

};

END



#pragma once

#include "Base.h"

/* 레벨 변경하는 기능 : 기존 레벨에서 사용하던 자원들을 정리한다. */
/* 현재 보여주는 레벨을 보관한다.  */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
public:
	CLevel_Manager() = default;
	virtual ~CLevel_Manager() = default;

public:
	/* 씬을 교체한다. */
	HRESULT Open_Level(_uint iLevelID, class CLevel* pNewLevel);
	void Tick(_float fTimeDelta);
	HRESULT Render();	

public:
	const _uint& Get_NextLevelID() const
	{
		return m_iNextLevelID;
	}

public:
	void Set_NextLevelID(_uint iNextLevelID)
	{
		m_iNextLevelID = iNextLevelID;
	}

private:
	_uint					m_iLevelID = 0;
	_uint					m_iNextLevelID = 0;
	class CLevel*			m_pCurrentLevel = nullptr;

public:
	virtual void Free() override;
	
};

END
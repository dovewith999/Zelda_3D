#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag, _uint iIndex);

public:
	HRESULT NativeConstruct();

public:
	list<class CGameObject*> Get_ObjectList()
	{
		return m_GameObjects;
	}

	_uint Get_Size() const
	{
		return m_GameObjects.size();
	}

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	class CGameObject* Find_GameObject(); // list의 제일 뒤의 오브젝트 반환
	HRESULT Release_GameObject(); // list의 제일 뒤의 오브젝트 삭제
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

private:
	list<class CGameObject*>			m_GameObjects;
	typedef list<class CGameObject*>	GAMEOBJECTS;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END
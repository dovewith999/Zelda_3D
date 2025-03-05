#pragma once

#include "Base.h"

BEGIN(Engine)
class CCollider;
class CCollider_Manager;
class ENGINE_DLL CCollisioner final : public CBase
{
public:
	enum OBJECTGROUP
	{
		OBJECT_BOSS,
		OBJECT_MONSTER,
		OBJECT_NPC,
		OBJECT_WATERPOT,
		OBJECT_MAGMA,
		OBJECT_BOMB,
		OBJECT_ROCK,
		OBJECT_NORMAL,
		OBJECT_END
	};

private:
	explicit CCollisioner(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCollisioner() = default;

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

public:
	HRESULT NativeConstruct();

public:
	_uint Collision_Check_Bomb_Rock();
	void Check_Collision();
	HRESULT Add_Collider(OBJECTGROUP eCollisionGroup, CCollider* pCollider);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CriticalSection;

	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");
	_bool				m_isFinished = false;

private:
	vector<CCollider*> m_ObjectColliderList[OBJECT_END];

private:
	CCollider_Manager* m_pCollider_Manager = nullptr;

public:
	static CCollisioner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END
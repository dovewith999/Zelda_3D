#pragma once

#include "Base.h"

BEGIN(Engine)
class CCollider;
class CCollider_Manager;
class ENGINE_DLL CCollision_Thread final : public CBase
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
		OBJECT_DOOR,
		OBJECT_LOCK,
		OBJECT_FIRE,
		OBJECT_FIREBRAZIER,
		OBJECT_CUBE,
		OBJECT_MAGROCK,
		OBJECT_MAGZET,
		OBJECT_WOODPLANKS,
		OBJECT_NORMAL,
		OBJECT_END
	};

private:
	explicit CCollision_Thread(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCollision_Thread() = default;

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
	HRESULT Add_Collider(_uint iObjectID, CCollider* pCollider);

public:
	_uint Collision_Check_Bomb_Rock();
	_uint Collision_Check_WaterPot_MagRock();
	_uint Collision_Check_Door_Lock();
	_uint Collision_Check_Fire_FireBrazier();
	_uint Collision_Check_Box_FireBrazier();
	_uint Collision_Check_Cube();
	_uint Collision_Check_MagmaZet_MagRock();

public:
	void Check_Collision();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CriticalSection;

	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");
	_bool				m_isFinished = false;

private:
	list<CCollider*> m_ObjectColliderList[OBJECT_END];
	//vector<CCollider*> m_ObjectColliderList[OBJECT_END];

public:
	static CCollision_Thread* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END
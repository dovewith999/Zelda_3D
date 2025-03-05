#pragma once

#include "Component.h"

BEGIN(Engine)
class CCollision_Thread;
class CCollider;
class ENGINE_DLL CCollider_Manager final : public CComponent
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
		OBEJCT_MAGROCK,
		OBJECT_MAGZET,
		OBJECT_WOODPLANKS,
		OBJECT_NORMAL,
		OBJECT_END
	};

private:
	CCollider_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCollider_Manager() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;

public:
	HRESULT Add_CollisionObject(OBJECTGROUP eCollisionGroup, class CGameObject* pGameObject);
	HRESULT Add_Collider(OBJECTGROUP eCollisionGroup, CCollider* pCollider);
	HRESULT Init_CollisionList();
	HRESULT Check_Collision();
	class CGameObject* Get_Target();

private:
	vector<class CGameObject*> m_CollisionList[OBJECT_END];

	CCollision_Thread* m_pThread = nullptr;

public:
	static CCollider_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END


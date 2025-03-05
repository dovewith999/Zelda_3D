#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };

public:
	typedef struct tagColliderDesc
	{
		_float3 vScale;
		_float3 vAngle;
		_float3 vPosition;

	}COLLIDER_DESC;

private:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() override = default;


public: /*Get.FuncTion*/
	_bool Get_Collision() const
	{
		return m_bIsCollision;
	}

	_bool Get_Collision_AABB() const
	{
		return m_bIsCollision_AABB;
	}

	_bool Get_Collision_OBB() const
	{
		return m_bIsCollision_OBB;
	}

	_bool Get_Collision_SPHERE() const
	{
		return m_bIsCollision_SPHERE;
	}
	_float3 Get_Pos() const
	{
		return m_vPos;
	}


public:
	void Set_Collision_OBB(_bool bResult)
	{
		m_bIsCollision_OBB = bResult;
	}

	void Set_Collision_SPHERE(_bool bResult)
	{
		m_bIsCollision_SPHERE = bResult;
	}

public:
	HRESULT NativeConstruct_Prototype(TYPE eType);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	void Update(_fmatrix WorldMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

public:
	_bool Collision_AABB(CCollider* pTargetCollider, TYPE eType);
	_bool Collision_OBB(CCollider* pTargetCollider, TYPE eType);
	_bool Collision_SPHERE(CCollider* pTargetCollider, TYPE eType);
	_bool Collision_Player(CCollider* pTargetCollider, TYPE eType);

private:
	TYPE m_eType = TYPE_END;

	BoundingBox* m_pAABB = nullptr;
	BoundingOrientedBox* m_pOBB = nullptr;
	BoundingSphere* m_pSPHERE = nullptr;

	BoundingBox* m_pAABB_World = nullptr;
	BoundingOrientedBox* m_pOBB_World = nullptr;
	BoundingSphere* m_pSPHERE_World = nullptr;

private:
	_bool m_bIsCollision = false;
	_bool m_bIsCollision_AABB = false;
	_bool m_bIsCollision_OBB = false;
	_bool m_bIsCollision_SPHERE = false;
	_bool m_bIsCollision_Player = false;

private:
	_float3 m_vPos;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pBasicEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

#endif // _DEBUG

private:
	_matrix Remove_Rotation(_fmatrix Transform);
	_matrix Make_Rotation(_fmatrix Transform);
	_matrix Remove_Transform(_fmatrix Transform);
	COLLIDER_DESC m_eDesc;

public:
	static CCollider* Create(ID3D11Device* pDeivce, ID3D11DeviceContext* pDeviceContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END


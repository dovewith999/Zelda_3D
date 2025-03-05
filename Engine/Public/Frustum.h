#pragma once
#include "Base.h"

BEGIN(Engine)
class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
public:
	CFrustum();
	virtual ~CFrustum() override = default;
public:
	HRESULT NativeConstruct();
	void Transform_ToWorldSpace();
	void Transform_ToLocalSpace(_fmatrix WorldMatrix);

public:
	_bool isIn_FrustumInWorldSpace(_fvector vWorldPos, _float fRadian);
	_bool isIn_FrustumInLocalSpace(_fvector vLocalPos, _float fRadian);

private:
	_float3 m_vProjPoints[8]; // 투영 스페이스에서의 절두체의 꼭짓점 8개의 좌표를 가질 배열
	_float3 m_vWorldPoints[8]; // 월스 스페이스 에서의 절두체의 꼭짓점 8개의 좌표를 가질 배열

	_float4 m_WorldPlanes[6]; // 월드 스페이스에서의 절두체의 6면을 가질 배열
	_float4 m_LocalPlanes[6]; // 오브젝트의 로컬스페이스에서의 절두체의 6면을 가질 배열
public:
	virtual void Free() override;
};
END


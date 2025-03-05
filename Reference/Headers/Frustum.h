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
	_float3 m_vProjPoints[8]; // ���� �����̽������� ����ü�� ������ 8���� ��ǥ�� ���� �迭
	_float3 m_vWorldPoints[8]; // ���� �����̽� ������ ����ü�� ������ 8���� ��ǥ�� ���� �迭

	_float4 m_WorldPlanes[6]; // ���� �����̽������� ����ü�� 6���� ���� �迭
	_float4 m_LocalPlanes[6]; // ������Ʈ�� ���ý����̽������� ����ü�� 6���� ���� �迭
public:
	virtual void Free() override;
};
END


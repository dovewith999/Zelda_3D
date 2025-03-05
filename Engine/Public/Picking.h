#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)

public:
	CPicking();
	virtual ~CPicking() override = default;

public:
	HRESULT NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hWnd, _uint iWinCX, _uint iWinCY);
	void Transform_ToWorldSpace();
	bool Picking(class CVIBuffer* pVIBuffer, class CTransform* pTransform, _float4* pOut);

private:
	HWND				m_hWnd;
	_uint				m_iWinCX;
	_uint				m_iWinCY;

private:
	_float4				m_vWorldRayDir, m_vWorldRayPos;

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

public:
	virtual void Free() override;
};
END


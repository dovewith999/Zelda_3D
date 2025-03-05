#pragma once
#include "GAmeObject.h"

BEGIN(Engine)
class ENGINE_DLL CUIObject abstract: public CGameObject
{
protected:
	explicit CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUIObject(const CUIObject& rhs);
	virtual ~CUIObject() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc) override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

//protected:
//	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
//	_float4x4			m_ProjMatrix;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};
END

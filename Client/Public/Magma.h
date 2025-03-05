#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
class CCollider_Manager;
END

BEGIN(Client)
class CPlayer;
class CMagma final : public CGameObject
{
protected:
	explicit CMagma(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMagma(const CMagma& rhs);
	virtual ~CMagma() override = default;


public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc) override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

private:/* Component */
	CTexture*						m_pTextureCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CVIBuffer_Rect*					m_pVIBufferCom = nullptr;
	CCollider*						m_pOBB = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;
	
private:/* 포인터 객체 변수 */
	CPlayer* m_pPlayer = nullptr;

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;

public:
	HRESULT SetUp_Components();

public:
	_float Get_Distance() const;


public:
	static CMagma* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


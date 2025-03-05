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
class CCamera_Player;
class CObject_Fire final : public CGameObject
{
protected:
	explicit CObject_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CObject_Fire(const CObject_Fire& rhs);
	virtual ~CObject_Fire() override = default;


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
	CCollider*						m_pSPHERE = nullptr;
	CCollider_Manager*				m_pCollider_Manager = nullptr;

public:
	void Set_SocketMatrix(_matrix Matrix)
	{
		XMStoreFloat4x4(&m_SocketMatrix, Matrix);
	}

	void Set_Boko(_bool bResult)
	{
		m_bBoko = bResult;
	}

private:/* 포인터 객체 변수 */
	CCamera_Player* m_pCamera_Player = nullptr;

private:
	/*_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;*/
	_uint m_iTextureNum = 0;
	_float m_fTimeAcc = 0.f;
	_float4x4			m_SocketMatrix;
	_bool m_bBoko = false;

public:
	HRESULT SetUp_Components();

public:
	static CObject_Fire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


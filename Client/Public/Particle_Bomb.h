#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_PointInstance;
END

BEGIN(Client)

class CParticle_Bomb final : public CGameObject
{
private:
	CParticle_Bomb(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CParticle_Bomb(const CParticle_Bomb& rhs);
	virtual ~CParticle_Bomb() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTexture*						m_pTextureCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CVIBuffer_PointInstance*		m_pVIBufferCom = nullptr;


public:
	HRESULT SetUp_Components();

public:
	static CParticle_Bomb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

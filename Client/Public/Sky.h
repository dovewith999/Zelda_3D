#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CShader;
class CVIBuffer_Cube;
END

BEGIN(Client)
class CSky final : public CGameObject
{
private:
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSky(const CSky& rhs);
	virtual ~CSky() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc) override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CVIBuffer_Cube* m_pVIBufferCom = nullptr;

private:
	_float m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4 m_ProjMatrix;

public:
	HRESULT SetUp_Components();

public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


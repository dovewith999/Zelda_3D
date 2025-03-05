#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCell;
class CShader;
class CTexture;
class CRenderer;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{

public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_FILTER, TYPE_BRUSH, TYPE_END };

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_NumPoint()
	{
		if (1 == m_iNumPoint)
		{
			m_iNumPoint = 0;
		}
		else if(0 == m_iNumPoint)
		{
			m_iNumPoint = 2;
			if (!m_Cells.empty())
			{
				m_Cells.pop_back();
			}
		}

		else if (2 == m_iNumPoint)
		{
			m_iNumPoint = 1;
		}
	}

	void Set_Scale(CGameObject* pTarget);
	void Set_Position(CGameObject* pTarget);

public:
	void Pop_Back_Object();

private:
	CTexture*						m_pTextureCom[TYPE_END] = { nullptr };
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CVIBuffer_Terrain*				m_pVIBufferCom = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;
	CShader* m_pShader = nullptr;
	CGameObject* m_pTarget = nullptr;

private:
	_uint					m_iPassIndex = 0;
	ID3D11ShaderResourceView*		m_pFilterSRV = nullptr;

private:
	_float m_fTimeAcc = 0.f;
	_float4 m_vBrushPos;

private:
	_float3		m_vPoints[3];
	_uint		m_iNumPoint = 0;
	_uint		m_iNumCell = 0;
	vector<CCell*> m_Cells;
	typedef vector<CCell*> CELLS;

private:
	int m_iObjectID = 0;
	_float m_fRotationX = 0.f;
	_float m_fRotationY = 0.f;
	_float m_fRotationZ = 0.f;
	_bool m_bRotation = false;

private:
	_float m_fScaleX = 0.f;
	_float m_fScaleY = 0.f;
	_float m_fScaleZ = 0.f;

private:
	_float m_fPositionX_Plus = 0.f;
	_float m_fPositionY_Plus = 0.f;
	_float m_fPositionZ_Plus = 0.f;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT Create_FilterTexture();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
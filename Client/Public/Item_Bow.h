#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CHierarchyNode;
END

BEGIN(Client)
class CItem_Bow final : public CGameObject
{

public:
	typedef struct tagItemDesc
	{
		LEVEL			eLevelID;
		const _tchar*	pLayerTag;
		_uint			iIndex;
		const _tchar*	pComTag;

		const char*		pBoneName;
	}ITEMDESC;

private:
	explicit CItem_Bow(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CItem_Bow(const CItem_Bow& rhs);
	virtual ~CItem_Bow() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;

private:
	ITEMDESC						m_ItemDesc;
	CHierarchyNode*					m_pBone = nullptr;
	_float4x4						m_PivotMatrix;

private:
	HRESULT SetUp_BonePtr();
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CItem_Bow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


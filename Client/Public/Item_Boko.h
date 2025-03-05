#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CHierarchyNode;
class CCollider;
class CNavigation;
END

BEGIN(Client)
class CPlayer;
class CMonster;
class CObject_Fire;
class CItem_Boko final : public CGameObject
{
	enum WEARSTATE { WEAR_TRUE, WEAR_FALSE, WEAR_END };
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
	explicit CItem_Boko(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CItem_Boko(const CItem_Boko& rhs);
	virtual ~CItem_Boko() override = default;

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
	CCollider*						m_pOBB = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;

private:
	ITEMDESC						m_ItemDesc;
	CHierarchyNode*					m_pBone = nullptr;
	_float4x4						m_PivotMatrix;
	_bool							m_bSocket = true;
	_float4							m_vCurrentPos;
	WEARSTATE						m_eState = WEAR_END;

private:
	CPlayer* m_pPlayer = nullptr;
	CMonster* m_pMonster = nullptr;
	CObject_Fire* m_pFire = nullptr;

private:
	HRESULT SetUp_BonePtr();
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_OnTerrain();

public:
	static CItem_Boko* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END



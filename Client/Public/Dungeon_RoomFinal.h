#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)
class CDungeon_RoomFinal final : public CGameObject
{
private:
	CDungeon_RoomFinal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDungeon_RoomFinal(const CDungeon_RoomFinal& rhs);
	virtual ~CDungeon_RoomFinal() override = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC &pTransformDesc) override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;

public:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();


public:
	static CDungeon_RoomFinal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC &pTransformDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END



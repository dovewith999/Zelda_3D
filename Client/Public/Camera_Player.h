#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class CPlayer;
class CCamera_Player final : public CCamera
{
public:
	enum ACTIONLADDERSTATE { LADDER_READY, LADDER_UP, LADDER_FINAL, LADDER_END };

protected:
	explicit CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Move_Lerp_Camera(_float fTimeDelta);

public:
	void Action_Ladder(_float fTimeDelta);
	void Action_GetItem(_float fTimeDelta);

private:
	_float m_fTimeAcc = 0.f;
	_float4x4 m_CurrentPosition;

private:
	ACTIONLADDERSTATE m_eState_Ladder = LADDER_END;

private:
	CNavigation* m_pNavigationCom = nullptr;

private:

	CPlayer* m_pPlayer = nullptr;
	CTransform* m_pPlayer_Transform = nullptr;

private:
	HRESULT SetUp_Component();

public:
	static CCamera_Player* Create(ID3D11Device* pDevice , ID3D11DeviceContext* pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
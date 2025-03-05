#pragma once

#include "Component.h"

/* 1. ȭ�鿡 �׷����� ��ü���� ��Ƽ� �����Ѵ�.(������ �������.) */
/* 2. ������ ������� ��ü���� �����Լ��� ȣ�����ش�. */

BEGIN(Engine)
class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { 
		RENDER_PRIORITY, 
		RENDER_NONALPHABLEND, 
		RENDER_NONLIGHT,
		RENDER_ALPHABLEND,
		RENDER_UI, 
		RENDER_DEBUG,
		RENDER_END 
	};

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;

public:
	HRESULT Add_RenderList(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_Renderer();

#ifdef _DEBUG
public:
	HRESULT Add_DebugList(class CComponent* pDebugComponent);
	HRESULT Draw_DebugList();
#endif // _DEBUG


private:
	list<class CGameObject*>			m_RenderList[RENDER_END];
	class CTarget_Manager* m_pTarget_Manager = nullptr;


private:
	class CVIBuffer_Rect* m_pVIBuffer = nullptr;
	class CShader* m_pShader = nullptr;
	class CLight_Manager*				m_pLight_Manager = nullptr;
	_float4x4 m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

#ifdef _DEBUG
private:
	list<class CComponent*> m_DebugList;
#endif // _DEBUG


private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UI();


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};

END
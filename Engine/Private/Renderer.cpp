#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Light_Manager.h"
#include "PipeLine.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		ViewportDesc;
	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Diffuse"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Normal"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Depth"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_LimLight */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_LimLight"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shade"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Specular"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.MRT_Deferred */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_LimLight"))))
		return E_FAIL;

	/* For.MRT_LightAcc */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElement);
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_Debug_TargetDesc(TEXT("Target_Diffuse"), 75.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_TargetDesc(TEXT("Target_Normal"), 75.f, 225.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_TargetDesc(TEXT("Target_Depth"), 75.f, 375.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_TargetDesc(TEXT("Target_LimLight"), 75.f, 525.f, 150.f, 150.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug_TargetDesc(TEXT("Target_Shade"), 225.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_TargetDesc(TEXT("Target_Specular"), 225.f, 225.f, 150.f, 150.f)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Add_RenderList(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (eRenderGroup >= CRenderer::RENDER_END)
		return E_FAIL;

	m_RenderList[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);


	return S_OK;
}

HRESULT CRenderer::Draw_Renderer()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;

	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;


#ifdef _DEBUG
	//if (FAILED(Draw_DebugList()))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"))))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"))))
	//	return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugList(CComponent * pDebugComponent)
{
	m_DebugList.push_back(pDebugComponent);

	Safe_AddRef(pDebugComponent);

	return S_OK;
}

HRESULT CRenderer::Draw_DebugList()
{
	for (auto& pDebug : m_DebugList)
	{
		if (nullptr != pDebug)
			pDebug->Render();

		Safe_Release(pDebug);
	}

	m_DebugList.clear();

	return S_OK;
}

#endif // _DEBUG

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderList[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderList[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderList[RENDER_NONALPHABLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pTarget_Manager ||
		nullptr == m_pLight_Manager)
		return E_FAIL;

	/* ��ġ�� 0��°�� ShadeŸ���� �����ߴ�. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;


	/* Ÿ�� �Ŵ����κ��� �븻Ÿ���� �޾ƿͼ� ���̴��� �������´�. */
	if (FAILED(m_pShader->Set_SRV("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_SRV("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
	{
		return E_FAIL;
	}


	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	Safe_AddRef(pPipeLine);

	_float4x4 ViewMatrixInv, ProjMatrixInv;
	_float4 vCamPos;
	_float fCamFar;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))));

	vCamPos = pPipeLine->Get_CamPositionFloat4();
	fCamFar = pPipeLine->Get_Far();

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
	{
		return E_FAIL;
	}

	Safe_Release(pPipeLine);


	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	/* �� ������ŭ �簢�� ���۸� �׸���. */

	/* �׸� �簢�����۷� ���̵�Ÿ�ٿ� ����� ����ϴ� ���̴�. */
	if (FAILED(m_pLight_Manager->Render_Lights(m_pVIBuffer, m_pShader)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	/* Diffuse * Shade ����ۿ� �׸������� �Լ� */
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pShader->Set_SRV("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_SRV("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_SRV("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_SRV("g_LimLightTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pShader->Begin(3);

	m_pVIBuffer->Render();



	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderList[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderList[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	for (auto& pGameObject : m_RenderList[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderList[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderList[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderList[RENDER_UI].clear();

	return S_OK;
}


CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX(TEXT("Failed to Created : CLayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();


	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderList[i])
			Safe_Release(pGameObject);
		m_RenderList[i].clear();
	}
}

#include "..\Public\Target_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
}


ID3D11ShaderResourceView * CTarget_Manager::Get_SRV(const _tchar * pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_SRV();
}

HRESULT CTarget_Manager::Add_RenderTarget(const _tchar * pRenderTargetTag, ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT Format, _float4 vClearColor)
{

	if (nullptr != Find_RenderTarget(pRenderTargetTag))
	{
		return E_FAIL;
	}

	CRenderTarget* pRenderTarget = CRenderTarget::Create(pDevice, pDeviceContext, iSizeX, iSizeY, Format, vClearColor);
	if (nullptr == pRenderTarget)
	{
		return E_FAIL;
	}

	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pRenderTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (nullptr == pRenderTarget)
	{
		return E_FAIL;
	}

	list<CRenderTarget*>* pMrtList = Find_MRT(pMRTTag);

	if (nullptr == pMrtList)
	{
		list<CRenderTarget*> MRTList;

		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
	{
		pMrtList->push_back(pRenderTarget);
	}

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		MSGBOX(TEXT("Error : MRTList is nullptr"));
		return E_FAIL;
	}

	pDeviceContext->OMGetRenderTargets(1, &m_pPreRTV, &m_pPreDSV);
	// 바인딩 되어있던 랜더타겟뷰(백버퍼)와 뎁스스텐실뷰를 이 변수들에 집어 넣어놓는다.


	ID3D11RenderTargetView* pRTVs[8] = { nullptr };
	// 랜더타겟뷰는 최대 8개까지 바인딩 가능하므로 8개를 선언한다.

	_uint iNumViews = 0;
	for (auto& pRenderTarget : *pMRTList)
	{
		if (nullptr != pRenderTarget)
		{
			pRenderTarget->Clear();
			pRTVs[iNumViews++] = pRenderTarget->Get_RTV();
		}
	}

	pDeviceContext->OMSetRenderTargets(iNumViews, pRTVs, m_pPreDSV);
	// 새로 만든 랜더타겟와 뎁스스탠실 뷰를 장치의 iNumView번째 타겟으로 바인딩한다.
	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext * pDeviceContext)
{
	if (nullptr == pDeviceContext)
	{
		return E_FAIL;
	}

	pDeviceContext->OMSetRenderTargets(1, &m_pPreRTV, m_pPreDSV);
	//빛 연산이 끝난 후 다시 백버퍼를 장치에 바인딩한다.

	Safe_Release(m_pPreRTV);
	Safe_Release(m_pPreDSV);
	return S_OK;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug_Buffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	m_pDebugBuffer = CVIBuffer_Rect::Create(pDevice, pDeviceContext);
	if (nullptr == m_pDebugBuffer)
	{
		return E_FAIL;
	}

	_uint iNumViewports = 1;
	D3D11_VIEWPORT ViewPortDesc;
	pDeviceContext->RSGetViewports(&iNumViewports, &ViewPortDesc);
	
	m_pDebugShader = CShader::Create(pDevice, pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElement);

	if (nullptr == m_pDebugShader)
	{
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f)));
	return S_OK;
}

HRESULT CTarget_Manager::Ready_Debug_TargetDesc(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
	{
		return E_FAIL;
	}

	return pRenderTarget->Ready_Debug_Desc(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_Debug(const _tchar * pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		return E_FAIL;
	}
	_float4x4		WorldMatrix, ViewMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pDebugShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pDebugShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render_Debug(m_pDebugShader, m_pDebugBuffer);
	}

	return S_OK;
}

#endif

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pTargetTag)
{

	auto iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTagFinder(pTargetTag));
	if (iter == m_RenderTargets.end())
	{
		return nullptr;
	}

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTagFinder(pMRTTag));

	if (iter == m_MRTs.end())
	{
		return nullptr;
	}

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
		{
			Safe_Release(pRenderTarget);
		}

		Pair.second.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
	{
		Safe_Release(Pair.second);
	}

	m_RenderTargets.clear();


	Safe_Release(m_pDebugShader);
	Safe_Release(m_pDebugBuffer);
}

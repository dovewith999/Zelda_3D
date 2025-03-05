#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_ComponentPtr(const _tchar * pComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Find_Component(pComponentTag);
}

HRESULT CLayer::NativeConstruct()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

CGameObject * CLayer::Find_GameObject()
{
	return m_GameObjects.back();
} 

HRESULT CLayer::Release_GameObject()
{
	if (1 <= m_GameObjects.size())
	{
		m_GameObjects.pop_back();
	}

	else
	{
		MSGBOX(TEXT("Error : m_GameObjects is empty"));
		return E_FAIL;
	}

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(fTimeDelta);
 	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Late_Tick(fTimeDelta);
		}
	}

	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		bool bDead = (*iter)->Get_Release();
		if (bDead)
		{
			Safe_Release(*iter);
			m_GameObjects.erase(iter++);
		}
		else
			++iter;
	}

}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX(TEXT("Failed to Created : CLayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();

}


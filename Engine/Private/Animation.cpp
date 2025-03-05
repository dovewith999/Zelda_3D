#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "HierarchyNode.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_Duration(rhs.m_Duration)
	, m_TickPerSec(rhs.m_TickPerSec)
	, m_iNumChannel(rhs.m_iNumChannel)
	, m_Channels(rhs.m_Channels)
{
	strcpy_s(m_szName, rhs.m_szName);
	
	for (auto& pChannel : m_Channels)
	{
		Safe_AddRef(pChannel);
	}
}

HRESULT CAnimation::NativeConstruct(aiAnimation * pAnimation, CModel* pModel)
{

	strcpy_s(m_szName, pAnimation->mName.data);

	m_Duration = pAnimation->mDuration;
	m_TickPerSec = pAnimation->mTicksPerSecond;
	m_iNumChannel = pAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannel; ++i)
	{
		CHierarchyNode* pHierarchyNode = pModel->Find_HierarchyNode(pAnimation->mChannels[i]->mNodeName.data);
		if (nullptr == pHierarchyNode)
		{
			MSGBOX(TEXT("Error : Not Found HierarchyNode from Animation Cpp"));
			return E_FAIL;
		}

		CChannel* pChannel = CChannel::Create(pAnimation->mChannels[i], pHierarchyNode);
		if (nullptr == pChannel)
		{
			MSGBOX(TEXT("Error : pChannel is NULL from Animation.cpp"));
			return E_FAIL;
		}

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::NativeConstruct_Clone(CModel * pModel)
{
	vector<CChannel*>		Channels;

	for (auto& pPrototype : m_Channels)
	{
		CChannel*		pChannel = pPrototype->Clone();
		if (nullptr == pChannel)
			break;

		CHierarchyNode*	pNode = pModel->Find_HierarchyNode(pPrototype->Get_Name());
		pChannel->Set_HierarchyNode(pNode);

		Channels.push_back(pChannel);

		Safe_Release(pPrototype);
	}

	m_Channels.clear();

	m_Channels = Channels;

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(_double TimeDelta, _bool bLoop)
{
	m_TimeAcc += m_TickPerSec * TimeDelta;


	if (m_TimeAcc >= m_Duration)
	{
		if (bLoop)
		{
			m_TimeAcc = 0.0;
		
		}
		
		else if (!bLoop)
		{
			m_TimeAcc = m_Duration;
		}

		m_bIsFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannel; ++i)
	{
		if (true == m_bIsFinished && true == bLoop)
		{
			m_Channels[i]->Set_CurrentKeyFrameIndex(0);
		}

		m_Channels[i]->Update_TransformationMatrix(m_TimeAcc);

	}

}

void CAnimation::Animation_Finished()
{
	if (true == m_bIsFinished)
	{
		m_bIsFinished = false;
	}
}

void CAnimation::Init_KeyFrame()
{
	for (_uint i = 0; i < m_iNumChannel; ++i)
	{
		m_TimeAcc = 0.0;

		m_Channels[i]->Set_CurrentKeyFrameIndex(0);
		
		m_Channels[i]->Update_TransformationMatrix(0.0);
	}
}

HRESULT CAnimation::Linear_Interpolation_Next_Anim(_double TimeDelta, CAnimation* pNextAnim, _bool* bFinish)
{
	m_TimeAcc += TimeDelta;

	for (_uint i = 0; i < m_iNumChannel; ++i)
	{
		if (FAILED(m_Channels[i]->Linear_Interpolation_Next_Anim(m_TimeAcc, pNextAnim->Get_Channel(i)->Get_FirstKeyFrame(), bFinish)))
		{
			m_TimeAcc = 0.0;
			return E_FAIL;
		}
	}

	return S_OK;
}


CAnimation * CAnimation::Create(aiAnimation * pAnimation, CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();
	if (FAILED(pInstance->NativeConstruct(pAnimation, pModel)))
	{
		MSGBOX(TEXT("Error : Failed Created Animation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation * CAnimation::Clone_Animation(CModel * pModel)
{
	CAnimation* pInstance = new CAnimation(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pModel)))
	{
		MSGBOX(TEXT("Error : Failed Cloned Animation from Animation.cpp"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
	{
		Safe_Release(pChannel);
	}

	m_Channels.clear();
}

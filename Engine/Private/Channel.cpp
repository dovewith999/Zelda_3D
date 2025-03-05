#include "..\Public\Channel.h"
#include "HierarchyNode.h"


CChannel::CChannel()
	: m_isCloned(false)
{
}

CChannel::CChannel(const CChannel & rhs)
	: m_iCurrentKeyFrame(rhs.m_iCurrentKeyFrame)
	, m_iNumKeyFrame(rhs.m_iNumKeyFrame)
	, m_KeyFrames(rhs.m_KeyFrames)
	, m_isCloned(true)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CChannel::NativeConstruct(aiNodeAnim * pChannel, CHierarchyNode* pHierarchyNode)
{
	strcpy_s(m_szName, pChannel->mNodeName.data);

	m_pHierarchyNode = pHierarchyNode;
	Safe_AddRef(m_pHierarchyNode);

	m_iNumKeyFrame = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
	m_iNumKeyFrame = max(m_iNumKeyFrame, pChannel->mNumPositionKeys);

	_float3 vScale, vPosition;
	_float4 vRotation;

	for (_uint i = 0; i < m_iNumKeyFrame; ++i)
	{
		KEYFRAME* pKeyFrame = new KEYFRAME();
		ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

		if (pChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pChannel->mScalingKeys[i].mValue, sizeof(_float3));
			pKeyFrame->Time = pChannel->mScalingKeys[i].mTime;
		}

		if (pChannel->mNumRotationKeys > i)
		{
			vRotation.x = pChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pChannel->mRotationKeys[i].mValue.w;
			pKeyFrame->Time = pChannel->mRotationKeys[i].mTime;
		}

		if (pChannel->mNumPositionKeys > i)
		{
			memcpy(&vPosition, &pChannel->mPositionKeys[i].mValue, sizeof(_float3));
			pKeyFrame->Time = pChannel->mPositionKeys[i].mTime;
		}

		memcpy(&pKeyFrame->vScale, &vScale, sizeof(_float3));
		memcpy(&pKeyFrame->vRotation, &vRotation, sizeof(_float4));
		memcpy(&pKeyFrame->vPosition, &vPosition, sizeof(_float3));
		
		m_KeyFrames.push_back(pKeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(_double TimeDelta)
{
	_vector vScale, vRotation, vPosition;

	if (TimeDelta >= m_KeyFrames.back()->Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back()->vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back()->vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back()->vPosition);
		vPosition = XMVectorSetW(vPosition, 1);
	}

	else
	{
		while (TimeDelta >= m_KeyFrames[m_iCurrentKeyFrame + 1]->Time)
		{
			++m_iCurrentKeyFrame;
		}

		_vector vSourScale, vDestScale;
		_vector vSourRotation, vDestRotation;
		_vector vSourPosition, vDestPosition;

		_double Ratio = ((TimeDelta - m_KeyFrames[m_iCurrentKeyFrame]->Time) / (m_KeyFrames[m_iCurrentKeyFrame + 1]->Time - m_KeyFrames[m_iCurrentKeyFrame]->Time));

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame]->vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, static_cast<_float>(Ratio));
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, static_cast<_float>(Ratio));
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, static_cast<_float>(Ratio));
		vPosition = XMVectorSetW(vPosition, 1);
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	m_pHierarchyNode->Set_TransformationMatrix(TransformationMatrix);
}

HRESULT CChannel::Linear_Interpolation_Next_Anim(_double TimeDelta, KEYFRAME* pNextAnimKeyFrame, _bool* bFinish)
{
	_vector vScale, vRotation, vPosition;

	/*if (TimeDelta >= m_KeyFrames.back()->Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back()->vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back()->vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back()->vPosition);
		vPosition = XMVectorSetW(vPosition, 1);
	}*/

	/*else
	{*/

		_vector vSourScale, vDestScale;
		_vector vSourRotation, vDestRotation;
		_vector vSourPosition, vDestPosition;

		_double Ratio = (TimeDelta / 0.2f);

		if (1.0 <= Ratio)
		{
			*bFinish = false;
			return S_OK;
		}

		// Ratio가 1보다 크거나 같을 시 return해야 함
		// 다음 키프레임이 없기 때문

		//vSourScale = XMLoadFloat3(&m_KeyFrames.back()->vScale);							//  지금 실행되는 애니메이션의 마지막 키프레임의 scale
		//vSourRotation = XMLoadFloat4(&m_KeyFrames.back()->vRotation);					//  지금 실행되는 애니메이션의 마지막 키프레임의 Rotation
		//vSourPosition = XMLoadFloat3(&m_KeyFrames.back()->vPosition);					//  지금 실행되는 애니메이션의 마지막 키프레임의 Position

		XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourPosition, XMLoadFloat4x4(&m_pHierarchyNode->Get_PreTransformMatrix()));

		vDestScale = XMLoadFloat3(&pNextAnimKeyFrame->vScale);		//  넘겨받아야됨 : 다음에 실행되는 애니메이션의 첫번째 키프레임의 scale
		vDestRotation = XMLoadFloat4(&pNextAnimKeyFrame->vRotation);	//  넘겨받아야됨 : 다음에 실행되는 애니메이션의 첫번째 키프레임의 Rotation
		vDestPosition = XMLoadFloat3(&pNextAnimKeyFrame->vPosition);	//  넘겨받아야됨 : 다음에 실행되는 애니메이션의 첫번째 키프레임의 Position

		vScale = XMVectorLerp(vSourScale, vDestScale, static_cast<_float>(Ratio));
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, static_cast<_float>(Ratio));
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, static_cast<_float>(Ratio));
		vPosition = XMVectorSetW(vPosition, 1);
	//}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pHierarchyNode->Set_TransformationMatrix(TransformationMatrix);

	return S_OK;
}

CChannel * CChannel::Create(aiNodeAnim * pChannel, CHierarchyNode* pHierarchyNode)
{
	CChannel* pInstance = new CChannel();
	if (FAILED(pInstance->NativeConstruct(pChannel, pHierarchyNode)))
	{
		MSGBOX(TEXT("Error : Failed Created Channel from Channel.cpp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel * CChannel::Clone()
{
	return new CChannel(*this);
}

void CChannel::Free()
{
	if (false == m_isCloned)
	{
		for (auto& pKeyFrame : m_KeyFrames)
		{
			Safe_Delete(pKeyFrame);
		}
	}
	m_KeyFrames.clear();

	Safe_Release(m_pHierarchyNode);
}

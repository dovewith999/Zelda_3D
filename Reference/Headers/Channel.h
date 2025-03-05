#pragma once
#include "Base.h"

BEGIN(Engine)
class CHierarchyNode;
class CChannel final : public CBase
{
private:
	CChannel();
	CChannel(const CChannel& rhs);
	virtual ~CChannel() override = default;

public:
	const char* Get_Name() const 
	{
		return m_szName;
	}

	KEYFRAME* Get_FirstKeyFrame() const
	{
		return m_KeyFrames.front();
	}

public:
	void Set_CurrentKeyFrameIndex(_uint iIndex)
	{
		m_iCurrentKeyFrame = iIndex;
	}

	void Set_HierarchyNode(CHierarchyNode* pNode) 
	{
		m_pHierarchyNode = pNode;
		Safe_AddRef(m_pHierarchyNode);
	}

public:
	HRESULT NativeConstruct(aiNodeAnim* pChannel, CHierarchyNode* pHierarchyNode);
	void Update_TransformationMatrix(_double TimeDelta);
	HRESULT Linear_Interpolation_Next_Anim(_double TimeDelta, KEYFRAME* pNextAnimKeyFrame, _bool* bFinish);

private:
	char m_szName[MAX_STR] = "";

private:
	_uint m_iNumKeyFrame = 0;
	_uint m_iCurrentKeyFrame = 0;
	vector<KEYFRAME*>  m_KeyFrames;
	typedef vector<KEYFRAME*> KEYFRAMES;

private:
	CHierarchyNode* m_pHierarchyNode = nullptr;
	_bool m_isCloned = false;

public:
	static CChannel* Create(aiNodeAnim* pChannel, CHierarchyNode* pHierarchyNode);
	CChannel* Clone();
	virtual void Free() override;
};
END

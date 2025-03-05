#pragma once
#include "Base.h"

BEGIN(Engine)
class CChannel;
class CModel;
class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() override = default;

public:
	HRESULT NativeConstruct(aiAnimation* pAnimation, CModel* pModel);
	HRESULT NativeConstruct_Clone(CModel* pModel);
	void Update_TransformationMatrix(_double TimeDelta, _bool bLoop = true);
	void Animation_Finished();
	void Init_KeyFrame();
	HRESULT Linear_Interpolation_Next_Anim(_double TimeDelta, CAnimation* pNextAnim, _bool* bFinish);

public: // Get_Function
	_bool Get_AnimationState()
	{
		return m_bIsFinished;
	}

	CChannel* Get_Channel(_uint iIndex)
	{
		return m_Channels[iIndex];
	}

private:
	char	m_szName[MAX_STR] = "";
	_double m_TickPerSec = 0.0;
	_double m_Duration = 0.0;

private: // Channel을 저장하기 위한 벡터
	_uint m_iNumChannel = 0;
	vector<CChannel*> m_Channels;
	typedef vector<CChannel*> CHANNELS;
	
private:
	_double m_TimeAcc = 0.0;
	_bool m_bIsFinished = false;

public:
	static CAnimation* Create(aiAnimation* pAnimation, CModel* pModel);
	CAnimation* Clone_Animation(CModel* pModel);
	virtual void Free() override;
};
END


#include "..\Public\SafeAnimation.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "Engine_Defines.h"
#include <algorithm>
#include <cassert>

// AnimationTrack 구현
AnimationTrack::AnimationTrack() 
    : m_boneName("")
    , m_isSorted(true)  // 빈 상태에서는 정렬된 것으로 간주
{
}

AnimationTrack::AnimationTrack(const std::string& boneName)
    : m_boneName(boneName)
    , m_isSorted(true)
{
}

AnimationTrack::AnimationTrack(const AnimationTrack& other)
    : m_boneName(other.m_boneName)
    , m_keyFrames(other.m_keyFrames)
    , m_isSorted(other.m_isSorted)
{
}

AnimationTrack& AnimationTrack::operator=(const AnimationTrack& other)
{
    if (this != &other)
    {
        m_boneName = other.m_boneName;
        m_keyFrames = other.m_keyFrames;
        m_isSorted = other.m_isSorted;
    }
    return *this;
}

AnimationTrack::AnimationTrack(AnimationTrack&& other) noexcept
    : m_boneName(std::move(other.m_boneName))
    , m_keyFrames(std::move(other.m_keyFrames))
    , m_isSorted(other.m_isSorted)
{
    other.m_isSorted = true;
}

AnimationTrack& AnimationTrack::operator=(AnimationTrack&& other) noexcept
{
    if (this != &other)
    {
        m_boneName = std::move(other.m_boneName);
        m_keyFrames = std::move(other.m_keyFrames);
        m_isSorted = other.m_isSorted;
        other.m_isSorted = true;
    }
    return *this;
}

float AnimationTrack::GetDuration() const
{
    if (m_keyFrames.empty())
        return 0.0f;
    
    if (!m_isSorted)
    {
        // 정렬되지 않은 경우 전체 스캔해서 최대값 찾기
        float maxTime = 0.0f;
        for (const auto& keyFrame : m_keyFrames)
        {
            if (keyFrame.time > maxTime)
                maxTime = keyFrame.time;
        }
        return maxTime;
    }
    
    return m_keyFrames.back().time;
}

bool AnimationTrack::AddKeyFrame(const AnimationKey& keyFrame)
{
    // 새로운 키프레임이 추가되면 정렬 상태가 깨질 수 있음
    if (!m_keyFrames.empty() && keyFrame.time < m_keyFrames.back().time)
        m_isSorted = false;
    
    m_keyFrames.push_back(keyFrame);
    return true;
}

bool AnimationTrack::AddKeyFrames(const std::vector<AnimationKey>& keyFrames)
{
    if (keyFrames.empty())
        return true;
    
    m_keyFrames.reserve(m_keyFrames.size() + keyFrames.size());
    
    for (const auto& keyFrame : keyFrames)
    {
        m_keyFrames.push_back(keyFrame);
    }
    
    m_isSorted = false;  // 일괄 추가 후에는 정렬 필요
    return true;
}

void AnimationTrack::SortKeyFramesByTime()
{
    if (!m_isSorted && !m_keyFrames.empty())
    {
        std::sort(m_keyFrames.begin(), m_keyFrames.end());
        m_isSorted = true;
    }
}

bool AnimationTrack::ValidateKeyFrames() const
{
    if (m_keyFrames.empty())
        return false;
    
    // 시간 순서 검사
    for (size_t i = 1; i < m_keyFrames.size(); ++i)
    {
        if (m_keyFrames[i - 1].time > m_keyFrames[i].time)
            return false;  // 정렬되지 않음
    }
    
    // 쿼터니언 정규화 검사
    for (const auto& keyFrame : m_keyFrames)
    {
        XMVECTOR quat = XMLoadFloat4(&keyFrame.rotation);
        XMVECTOR quatLength = XMVector4Length(quat);
        float length = XMVectorGetX(quatLength);
        
        // 쿼터니언의 길이가 거의 1이어야 함
        if (std::abs(length - 1.0f) > 0.001f)
            return false;
    }
    
    return true;
}

void AnimationTrack::ClearKeyFrames()
{
    m_keyFrames.clear();
    m_isSorted = true;
}

bool AnimationTrack::InterpolateAt(float time, XMMATRIX& outTransform) const
{
    if (m_keyFrames.empty())
        return false;
    
    // 정렬 보장
    if (!m_isSorted)
    {
        const_cast<AnimationTrack*>(this)->SortKeyFramesByTime();
    }
    
    XMFLOAT3 position;
    XMFLOAT4 rotation;
    XMFLOAT3 scale;
    
    if (!InterpolateComponentsAt(time, position, rotation, scale))
        return false;
    
    // 변환 행렬 계산
    XMVECTOR posVec = XMLoadFloat3(&position);
    XMVECTOR rotVec = XMLoadFloat4(&rotation);
    XMVECTOR scaleVec = XMLoadFloat3(&scale);
    
    outTransform = XMMatrixAffineTransformation(scaleVec, XMVectorZero(), rotVec, posVec);
    return true;
}

bool AnimationTrack::InterpolateComponentsAt(float time, XMFLOAT3& outPosition, 
                                            XMFLOAT4& outRotation, XMFLOAT3& outScale) const
{
    if (m_keyFrames.empty())
        return false;
    
    // 시간이 첫 번째 키프레임보다 이른 경우
    if (time <= m_keyFrames[0].time)
    {
        outPosition = m_keyFrames[0].position;
        outRotation = m_keyFrames[0].rotation;
        outScale = m_keyFrames[0].scale;
        return true;
    }
    
    // 시간이 마지막 키프레임보다 늦은 경우
    if (time >= m_keyFrames.back().time)
    {
        const AnimationKey& lastKey = m_keyFrames.back();
        outPosition = lastKey.position;
        outRotation = lastKey.rotation;
        outScale = lastKey.scale;
        return true;
    }
    
    // 키프레임 사이의 보간
    size_t keyIndex = FindKeyFrameIndex(time);
    
    // 안전성 검사: 배열 경계 검사 (기존 버그 수정)
    if (keyIndex >= m_keyFrames.size() - 1)
    {
        // 마지막 키프레임 반환
        const AnimationKey& lastKey = m_keyFrames.back();
        outPosition = lastKey.position;
        outRotation = lastKey.rotation;
        outScale = lastKey.scale;
        return true;
    }
    
    size_t nextKeyIndex = keyIndex + 1;
    float ratio = CalculateInterpolationRatio(time, keyIndex, nextKeyIndex);
    
    // 각 컴포넌트 보간
    outPosition = InterpolatePosition(m_keyFrames[keyIndex].position, 
                                     m_keyFrames[nextKeyIndex].position, ratio);
    outRotation = InterpolateRotation(m_keyFrames[keyIndex].rotation, 
                                     m_keyFrames[nextKeyIndex].rotation, ratio);
    outScale = InterpolateScale(m_keyFrames[keyIndex].scale, 
                               m_keyFrames[nextKeyIndex].scale, ratio);
    
    return true;
}

size_t AnimationTrack::FindKeyFrameIndex(float time) const
{
    if (m_keyFrames.empty())
        return 0;
    
    // 이진 탐색으로 키프레임 인덱스 찾기
    size_t left = 0;
    size_t right = m_keyFrames.size() - 1;
    
    while (left < right)
    {
        size_t mid = left + (right - left + 1) / 2;
        
        if (m_keyFrames[mid].time <= time)
            left = mid;
        else
            right = mid - 1;
    }
    
    return left;
}

float AnimationTrack::CalculateInterpolationRatio(float time, size_t keyIndex1, size_t keyIndex2) const
{
    // 안전성 검사: 배열 경계 및 인덱스 유효성 검사
    if (keyIndex1 >= m_keyFrames.size() || keyIndex2 >= m_keyFrames.size())
        return 0.0f;
    
    float time1 = m_keyFrames[keyIndex1].time;
    float time2 = m_keyFrames[keyIndex2].time;
    
    // 나눗셈 0 방지 (기존 버그 수정)
    float timeDiff = time2 - time1;
    if (std::abs(timeDiff) < 1e-6f)  // 거의 같은 시간
        return 0.0f;
    
    float ratio = (time - time1) / timeDiff;
    
    // 비율을 0.0 ~ 1.0 범위로 클램핑
    return std::max(0.0f, std::min(1.0f, ratio));
}

XMFLOAT3 AnimationTrack::InterpolatePosition(const XMFLOAT3& pos1, const XMFLOAT3& pos2, float ratio) const
{
    XMVECTOR v1 = XMLoadFloat3(&pos1);
    XMVECTOR v2 = XMLoadFloat3(&pos2);
    XMVECTOR result = XMVectorLerp(v1, v2, ratio);
    
    XMFLOAT3 output;
    XMStoreFloat3(&output, result);
    return output;
}

XMFLOAT4 AnimationTrack::InterpolateRotation(const XMFLOAT4& rot1, const XMFLOAT4& rot2, float ratio) const
{
    XMVECTOR q1 = XMLoadFloat4(&rot1);
    XMVECTOR q2 = XMLoadFloat4(&rot2);
    
    // 정규화된 쿼터니언 보장
    q1 = XMQuaternionNormalize(q1);
    q2 = XMQuaternionNormalize(q2);
    
    // 구면 선형 보간 (SLERP)
    XMVECTOR result = XMQuaternionSlerp(q1, q2, ratio);
    
    XMFLOAT4 output;
    XMStoreFloat4(&output, result);
    return output;
}

XMFLOAT3 AnimationTrack::InterpolateScale(const XMFLOAT3& scale1, const XMFLOAT3& scale2, float ratio) const
{
    XMVECTOR v1 = XMLoadFloat3(&scale1);
    XMVECTOR v2 = XMLoadFloat3(&scale2);
    XMVECTOR result = XMVectorLerp(v1, v2, ratio);
    
    XMFLOAT3 output;
    XMStoreFloat3(&output, result);
    return output;
}

// SafeAnimation 구현
SafeAnimation::SafeAnimation()
    : m_name("")
    , m_duration(0.0f)
    , m_ticksPerSecond(24.0f)  // 기본값
    , m_isValidated(false)
{
}

bool SafeAnimation::IsValid() const
{
    if (!m_isValidated)
    {
        m_isValidated = ValidateAllTracks();
    }
    return m_isValidated;
}

HRESULT SafeAnimation::InitializeFromAssimp(const aiAnimation* pAnimation, CModel* pModel)
{
    if (!pAnimation || !pModel)
    {
        m_lastValidationError = "Invalid parameters: pAnimation or pModel is null";
        return E_INVALIDARG;
    }
    
    // 기본 정보 설정
    m_name = pAnimation->mName.C_Str();
    m_duration = static_cast<float>(pAnimation->mDuration);
    m_ticksPerSecond = static_cast<float>(pAnimation->mTicksPerSecond);
    
    if (m_ticksPerSecond <= 0.0f)
        m_ticksPerSecond = 24.0f;  // 기본값 설정
    
    // 모든 채널(트랙) 처리
    for (unsigned int i = 0; i < pAnimation->mNumChannels; ++i)
    {
        aiNodeAnim* pChannel = pAnimation->mChannels[i];
        std::string boneName = pChannel->mNodeName.C_Str();
        
        // 해당 본이 모델에 존재하는지 확인
        CHierarchyNode* pHierarchyNode = pModel->Find_HierarchyNode(boneName.c_str());
        if (!pHierarchyNode)
        {
            m_lastValidationError = "Bone not found in model: " + boneName;
            continue;  // 경고하지만 계속 진행
        }
        
        auto track = std::make_unique<AnimationTrack>(boneName);
        
        // Assimp 키프레임 데이터를 통합된 키프레임으로 변환
        if (FAILED(ConvertAssimpChannelToTrack(pChannel, track.get())))
        {
            m_lastValidationError = "Failed to convert Assimp channel: " + boneName;
            continue;
        }
        
        // 트랙 추가
        if (!AddTrack(boneName, std::move(track)))
        {
            m_lastValidationError = "Failed to add track: " + boneName;
            return E_FAIL;
        }
    }
    
    // 데이터 정규화 및 좌표계 변환
    NormalizeAnimationData();
    ConvertCoordinateSystem();
    
    m_isValidated = false;  // 재검증 필요
    return S_OK;
}

HRESULT SafeAnimation::InitializeManual(const std::string& name, float duration, float ticksPerSecond)
{
    if (name.empty() || duration <= 0.0f || ticksPerSecond <= 0.0f)
    {
        m_lastValidationError = "Invalid parameters for manual initialization";
        return E_INVALIDARG;
    }
    
    m_name = name;
    m_duration = duration;
    m_ticksPerSecond = ticksPerSecond;
    
    m_isValidated = false;
    return S_OK;
}

bool SafeAnimation::AddTrack(const std::string& boneName, std::unique_ptr<AnimationTrack> track)
{
    if (!track || boneName.empty())
        return false;
    
    // 이미 같은 이름의 트랙이 있는지 확인
    if (FindTrack(boneName))
    {
        m_lastValidationError = "Track already exists: " + boneName;
        return false;
    }
    
    m_tracks.push_back(std::move(track));
    m_isValidated = false;  // 재검증 필요
    return true;
}

AnimationTrack* SafeAnimation::FindTrack(const std::string& boneName)
{
    for (auto& track : m_tracks)
    {
        if (track->GetBoneName() == boneName)
            return track.get();
    }
    return nullptr;
}

const AnimationTrack* SafeAnimation::FindTrack(const std::string& boneName) const
{
    for (const auto& track : m_tracks)
    {
        if (track->GetBoneName() == boneName)
            return track.get();
    }
    return nullptr;
}

bool SafeAnimation::UpdateBoneTransforms(float time, const std::vector<std::string>& boneNames, 
                                       std::vector<XMMATRIX>& outTransforms) const
{
    if (!IsValid())
    {
        return false;
    }
    
    // 출력 벡터 크기 조정
    outTransforms.resize(boneNames.size());
    
    // 각 본에 대해 변환 행렬 계산
    for (size_t i = 0; i < boneNames.size(); ++i)
    {
        const AnimationTrack* track = FindTrack(boneNames[i]);
        
        if (track && !track->InterpolateAt(time, outTransforms[i]))
        {
            // 보간 실패 시 항등 행렬로 설정
            outTransforms[i] = XMMatrixIdentity();
        }
        else if (!track)
        {
            // 트랙이 없으면 항등 행렬 사용
            outTransforms[i] = XMMatrixIdentity();
        }
    }
    
    return true;
}

bool SafeAnimation::GetBoneTransformAt(float time, const std::string& boneName, XMMATRIX& outTransform) const
{
    if (!IsValid())
        return false;
    
    const AnimationTrack* track = FindTrack(boneName);
    if (!track)
    {
        outTransform = XMMatrixIdentity();
        return false;
    }
    
    return track->InterpolateAt(time, outTransform);
}

bool SafeAnimation::ValidateAllTracks() const
{
    if (m_tracks.empty())
    {
        m_lastValidationError = "No animation tracks";
        return false;
    }
    
    for (const auto& track : m_tracks)
    {
        if (!track->ValidateKeyFrames())
        {
            m_lastValidationError = "Invalid keyframes in track: " + track->GetBoneName();
            return false;
        }
    }
    
    m_lastValidationError.clear();
    return true;
}

void SafeAnimation::NormalizeAnimationData()
{
    // 모든 트랙의 키프레임을 시간순으로 정렬
    for (auto& track : m_tracks)
    {
        track->SortKeyFramesByTime();
    }
    
    // 애니메이션 전체 지속 시간 재계산 (모든 트랙 중 최대값)
    float maxDuration = 0.0f;
    for (const auto& track : m_tracks)
    {
        float trackDuration = track->GetDuration();
        if (trackDuration > maxDuration)
            maxDuration = trackDuration;
    }
    
    if (maxDuration > m_duration)
        m_duration = maxDuration;
}

void SafeAnimation::ConvertCoordinateSystem()
{
    // TODO: Assimp (오른손 좌표계) → DirectX (왼손 좌표계) 변환 구현
    // 현재는 좌표계 변환을 생략하고 추후 구현 예정
}

HRESULT SafeAnimation::ConvertAssimpChannelToTrack(const aiNodeAnim* pChannel, AnimationTrack* pTrack)
{
    if (!pChannel || !pTrack)
        return E_INVALIDARG;
    
    // 모든 키프레임 타입의 최대 개수 구하기
    unsigned int maxKeys = std::max({pChannel->mNumPositionKeys, 
                                    pChannel->mNumRotationKeys, 
                                    pChannel->mNumScalingKeys});
    
    std::vector<AnimationKey> keyFrames;
    keyFrames.reserve(maxKeys);
    
    // 통합된 키프레임 생성 (기존 버그 수정)
    for (unsigned int i = 0; i < maxKeys; ++i)
    {
        AnimationKey key;
        
        // Position 키
        if (i < pChannel->mNumPositionKeys)
        {
            const aiVectorKey& posKey = pChannel->mPositionKeys[i];
            key.position.x = posKey.mValue.x;
            key.position.y = posKey.mValue.y;
            key.position.z = posKey.mValue.z;
            key.time = static_cast<float>(posKey.mTime / m_ticksPerSecond);
        }
        else if (pChannel->mNumPositionKeys > 0)
        {
            // 마지막 Position 키 값 사용
            const aiVectorKey& lastPosKey = pChannel->mPositionKeys[pChannel->mNumPositionKeys - 1];
            key.position.x = lastPosKey.mValue.x;
            key.position.y = lastPosKey.mValue.y;
            key.position.z = lastPosKey.mValue.z;
        }
        
        // Rotation 키
        if (i < pChannel->mNumRotationKeys)
        {
            const aiQuatKey& rotKey = pChannel->mRotationKeys[i];
            key.rotation.x = rotKey.mValue.x;
            key.rotation.y = rotKey.mValue.y;
            key.rotation.z = rotKey.mValue.z;
            key.rotation.w = rotKey.mValue.w;
            key.time = static_cast<float>(rotKey.mTime / m_ticksPerSecond);
        }
        else if (pChannel->mNumRotationKeys > 0)
        {
            // 마지막 Rotation 키 값 사용
            const aiQuatKey& lastRotKey = pChannel->mRotationKeys[pChannel->mNumRotationKeys - 1];
            key.rotation.x = lastRotKey.mValue.x;
            key.rotation.y = lastRotKey.mValue.y;
            key.rotation.z = lastRotKey.mValue.z;
            key.rotation.w = lastRotKey.mValue.w;
        }
        
        // Scale 키
        if (i < pChannel->mNumScalingKeys)
        {
            const aiVectorKey& scaleKey = pChannel->mScalingKeys[i];
            key.scale.x = scaleKey.mValue.x;
            key.scale.y = scaleKey.mValue.y;
            key.scale.z = scaleKey.mValue.z;
            key.time = static_cast<float>(scaleKey.mTime / m_ticksPerSecond);
        }
        else if (pChannel->mNumScalingKeys > 0)
        {
            // 마지막 Scale 키 값 사용
            const aiVectorKey& lastScaleKey = pChannel->mScalingKeys[pChannel->mNumScalingKeys - 1];
            key.scale.x = lastScaleKey.mValue.x;
            key.scale.y = lastScaleKey.mValue.y;
            key.scale.z = lastScaleKey.mValue.z;
        }
        
        // 시간 설정 (가장 적절한 시간 사용)
        if (i < pChannel->mNumPositionKeys)
            key.time = static_cast<float>(pChannel->mPositionKeys[i].mTime / m_ticksPerSecond);
        else if (i < pChannel->mNumRotationKeys)
            key.time = static_cast<float>(pChannel->mRotationKeys[i].mTime / m_ticksPerSecond);
        else if (i < pChannel->mNumScalingKeys)
            key.time = static_cast<float>(pChannel->mScalingKeys[i].mTime / m_ticksPerSecond);
        
        keyFrames.push_back(key);
    }
    
    // 트랙에 키프레임 추가
    if (!pTrack->AddKeyFrames(keyFrames))
        return E_FAIL;
    
    return S_OK;
}

SafeAnimation* SafeAnimation::CreateFromAssimp(const aiAnimation* pAnimation, CModel* pModel)
{
    SafeAnimation* pInstance = new SafeAnimation();
    
    if (FAILED(pInstance->InitializeFromAssimp(pAnimation, pModel)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    
    return pInstance;
}

SafeAnimation* SafeAnimation::CreateEmpty(const std::string& name, float duration, float ticksPerSecond)
{
    SafeAnimation* pInstance = new SafeAnimation();
    
    if (FAILED(pInstance->InitializeManual(name, duration, ticksPerSecond)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    
    return pInstance;
}

void SafeAnimation::Free()
{
    // unique_ptr로 관리되므로 자동으로 해제됨
    m_tracks.clear();
}
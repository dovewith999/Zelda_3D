#include "..\Public\SafeCAnimation.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "Engine_Defines.h"
#include <iostream>
#include <sstream>
#include <cmath>

SafeCAnimation::SafeCAnimation()
    : m_pSafeAnimation(nullptr)
    , m_pModel(nullptr)
    , m_currentTime(0.0f)
    , m_isFinished(false)
    , m_isLooping(true)
    , m_playbackSpeed(1.0f)
    , m_isCacheValid(false)
    , m_cachedTime(-1.0f)
{
}

HRESULT SafeCAnimation::NativeConstruct(aiAnimation* pAnimation, CModel* pModel)
{
    if (!pAnimation || !pModel)
        return E_INVALIDARG;
    
    m_pModel = pModel;
    Safe_AddRef(m_pModel);
    
    // SafeAnimation 생성
    m_pSafeAnimation = std::unique_ptr<SafeAnimation>(
        SafeAnimation::CreateFromAssimp(pAnimation, pModel)
    );
    
    if (!m_pSafeAnimation)
    {
        Safe_Release(m_pModel);
        return E_FAIL;
    }
    
    // 유효성 검사
    if (!m_pSafeAnimation->IsValid())
    {
        OutputDebugStringA(("Animation validation failed: " + m_pSafeAnimation->GetLastError()).c_str());
        Safe_Release(m_pModel);
        return E_FAIL;
    }
    
    // 초기 상태 설정
    m_currentTime = 0.0f;
    m_isFinished = false;
    m_isLooping = true;
    
    // 본 이름 캐시 생성
    CacheBoneNames();
    
    return S_OK;
}

HRESULT SafeCAnimation::NativeConstruct_Clone(CModel* pModel)
{
    if (!pModel || !m_pSafeAnimation)
        return E_INVALIDARG;
    
    // 기존 모델 해제하고 새 모델 연결
    Safe_Release(m_pModel);
    m_pModel = pModel;
    Safe_AddRef(m_pModel);
    
    // 본 이름 캐시 재생성
    CacheBoneNames();
    InvalidateCache();
    
    return S_OK;
}

void SafeCAnimation::Update_TransformationMatrix(double TimeDelta, bool bLoop)
{
    if (!m_pSafeAnimation || !m_pModel)
        return;
    
    // 재생 속도 적용
    float scaledTimeDelta = static_cast<float>(TimeDelta) * m_playbackSpeed;
    m_currentTime += scaledTimeDelta;
    m_isLooping = bLoop;
    
    // 시간 클램핑 및 완료 상태 체크
    float duration = m_pSafeAnimation->GetDuration();
    bool wasFinished = m_isFinished;
    
    if (m_currentTime >= duration)
    {
        m_isFinished = true;
        
        if (bLoop)
        {
            // 루프 모드: 시간을 0으로 리셋
            m_currentTime = std::fmod(m_currentTime, duration);
            if (m_currentTime < 0.0f) 
                m_currentTime += duration;
        }
        else
        {
            // 일회성 모드: 마지막 프레임에 고정
            m_currentTime = duration;
        }
    }
    else
    {
        m_isFinished = false;
    }
    
    // 변환 행렬 적용
    ApplyTransformsToModel();
}

void SafeCAnimation::Animation_Finished()
{
    // 기존 인터페이스 호환성: 완료 플래그 리셋
    if (m_isFinished)
    {
        m_isFinished = false;
    }
}

void SafeCAnimation::Init_KeyFrame()
{
    // 애니메이션을 첫 번째 프레임으로 리셋
    m_currentTime = 0.0f;
    m_isFinished = false;
    InvalidateCache();
    
    if (m_pSafeAnimation && m_pModel)
    {
        ApplyTransformsToModel();
    }
}

HRESULT SafeCAnimation::Linear_Interpolation_Next_Anim(double TimeDelta, SafeCAnimation* pNextAnim, bool* bFinish)
{
    if (!pNextAnim || !bFinish || !m_pSafeAnimation || !pNextAnim->m_pSafeAnimation)
        return E_INVALIDARG;
    
    // TODO: 애니메이션 블렌딩 구현
    // 현재는 간단한 전환으로 구현 (추후 고도화)
    
    static const float BLEND_DURATION = 0.2f;  // 0.2초 블렌딩
    static float blendTime = 0.0f;
    
    blendTime += static_cast<float>(TimeDelta);
    
    if (blendTime >= BLEND_DURATION)
    {
        *bFinish = false;  // 블렌딩 완료
        blendTime = 0.0f;
        return S_OK;
    }
    
    float blendRatio = blendTime / BLEND_DURATION;
    
    // 간단한 블렌딩: 현재 애니메이션의 마지막 프레임과 다음 애니메이션의 첫 프레임 보간
    // 실제 구현에서는 더 정교한 블렌딩 로직 필요
    
    *bFinish = true;  // 블렌딩 중
    return S_OK;
}

void SafeCAnimation::SeekToTime(float time)
{
    if (!m_pSafeAnimation)
        return;
    
    m_currentTime = ClampAnimationTime(time, m_isLooping);
    m_isFinished = (m_currentTime >= m_pSafeAnimation->GetDuration());
    InvalidateCache();
    
    ApplyTransformsToModel();
}

std::string SafeCAnimation::GetDebugInfo() const
{
    if (!m_pSafeAnimation)
        return "Invalid animation";
    
    std::stringstream ss;
    ss << "Animation: " << m_pSafeAnimation->GetName() << "\n";
    ss << "Duration: " << m_pSafeAnimation->GetDuration() << " seconds\n";
    ss << "Current Time: " << m_currentTime << " seconds\n";
    ss << "Progress: " << (m_currentTime / m_pSafeAnimation->GetDuration() * 100.0f) << "%\n";
    ss << "Tracks: " << m_pSafeAnimation->GetTrackCount() << "\n";
    ss << "Status: " << (m_isFinished ? "Finished" : "Playing") << "\n";
    ss << "Loop: " << (m_isLooping ? "Yes" : "No") << "\n";
    ss << "Speed: " << m_playbackSpeed << "x\n";
    
    return ss.str();
}

void SafeCAnimation::PrintCurrentTransforms() const
{
    if (!m_pSafeAnimation || m_boneNames.empty())
        return;
    
    std::cout << "=== Animation Transforms at t=" << m_currentTime << " ===" << std::endl;
    
    for (size_t i = 0; i < m_boneNames.size(); ++i)
    {
        XMMATRIX transform;
        if (m_pSafeAnimation->GetBoneTransformAt(m_currentTime, m_boneNames[i], transform))
        {
            // 변환 행렬을 Position, Rotation, Scale로 분해
            XMVECTOR scale, rotation, position;
            if (XMMatrixDecompose(&scale, &rotation, &position, transform))
            {
                XMFLOAT3 pos, scl;
                XMFLOAT4 rot;
                XMStoreFloat3(&pos, position);
                XMStoreFloat4(&rot, rotation);
                XMStoreFloat3(&scl, scale);
                
                std::cout << m_boneNames[i] << ":\n";
                std::cout << "  Pos: (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
                std::cout << "  Rot: (" << rot.x << ", " << rot.y << ", " << rot.z << ", " << rot.w << ")\n";
                std::cout << "  Scl: (" << scl.x << ", " << scl.y << ", " << scl.z << ")\n";
            }
        }
    }
}

void SafeCAnimation::CacheBoneNames()
{
    m_boneNames.clear();
    
    if (!m_pModel)
        return;
    
    // 모델의 모든 본 이름을 수집
    // 실제 구현에서는 CModel에서 본 이름 리스트를 가져오는 방법 필요
    // 현재는 애니메이션 트랙에서 본 이름을 추출
    
    if (m_pSafeAnimation)
    {
        for (size_t i = 0; i < m_pSafeAnimation->GetTrackCount(); ++i)
        {
            // SafeAnimation에서 트랙 이름을 가져오는 방법이 필요
            // 현재는 임시로 빈 상태로 둠
        }
    }
}

void SafeCAnimation::InvalidateCache()
{
    m_isCacheValid = false;
    m_cachedTime = -1.0f;
}

void SafeCAnimation::ApplyTransformsToModel()
{
    if (!m_pSafeAnimation || !m_pModel || m_boneNames.empty())
        return;
    
    // 캐시 확인
    if (m_isCacheValid && std::abs(m_cachedTime - m_currentTime) < 0.001f)
        return;  // 이미 같은 시간으로 계산됨
    
    // 변환 행렬 계산
    if (!m_pSafeAnimation->UpdateBoneTransforms(m_currentTime, m_boneNames, m_transformCache))
        return;
    
    // 모델의 각 본에 변환 행렬 적용
    for (size_t i = 0; i < m_boneNames.size() && i < m_transformCache.size(); ++i)
    {
        CHierarchyNode* pNode = m_pModel->Find_HierarchyNode(m_boneNames[i].c_str());
        if (pNode)
        {
            pNode->Set_TransformationMatrix(m_transformCache[i]);
        }
    }
    
    // 계층 구조 업데이트
    // 실제 구현에서는 CModel의 계층 구조 업데이트 메서드 호출 필요
    
    // 캐시 업데이트
    m_isCacheValid = true;
    m_cachedTime = m_currentTime;
}

float SafeCAnimation::ClampAnimationTime(float time, bool bLoop) const
{
    if (!m_pSafeAnimation)
        return 0.0f;
    
    float duration = m_pSafeAnimation->GetDuration();
    
    if (duration <= 0.0f)
        return 0.0f;
    
    if (bLoop)
    {
        // 루프 모드: 시간을 0 ~ duration 범위로 순환
        float clampedTime = std::fmod(time, duration);
        if (clampedTime < 0.0f)
            clampedTime += duration;
        return clampedTime;
    }
    else
    {
        // 클램프 모드: 0 ~ duration 범위로 제한
        return std::max(0.0f, std::min(time, duration));
    }
}

SafeCAnimation* SafeCAnimation::Create(aiAnimation* pAnimation, CModel* pModel)
{
    SafeCAnimation* pInstance = new SafeCAnimation();
    
    if (FAILED(pInstance->NativeConstruct(pAnimation, pModel)))
    {
        OutputDebugStringA("Error: Failed to create SafeCAnimation\n");
        Safe_Release(pInstance);
        return nullptr;
    }
    
    return pInstance;
}

SafeCAnimation* SafeCAnimation::Clone_Animation(CModel* pModel)
{
    SafeCAnimation* pInstance = new SafeCAnimation(*this);
    
    // SafeAnimation은 복사할 수 없으므로 같은 인스턴스를 공유
    // 실제로는 SafeAnimation도 복제 가능하게 만들어야 함
    
    if (FAILED(pInstance->NativeConstruct_Clone(pModel)))
    {
        OutputDebugStringA("Error: Failed to clone SafeCAnimation\n");
        Safe_Release(pInstance);
        return nullptr;
    }
    
    return pInstance;
}

void SafeCAnimation::Free()
{
    // SafeAnimation은 unique_ptr로 자동 해제
    m_pSafeAnimation.reset();
    
    Safe_Release(m_pModel);
    
    m_boneNames.clear();
    m_transformCache.clear();
}
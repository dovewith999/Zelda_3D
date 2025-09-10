#pragma once

#include "Base.h"
#include "SafeAnimation.h"

BEGIN(Engine)

class CModel;
class CHierarchyNode;

/**
 * @brief 기존 애니메이션 시스템과 새 SafeAnimation 시스템 간의 브리지 역할
 * 
 * 이 클래스는 기존 CAnimation, CChannel 인터페이스를 유지하면서
 * 내부적으로 SafeAnimation을 사용하여 모든 버그를 수정합니다.
 * (이름 변경: AnimationAdapter → SafeCAnimation)
 */
class ENGINE_DLL SafeCAnimation final : public CBase
{
private:
    std::unique_ptr<SafeAnimation> m_pSafeAnimation;        // 새로운 안전한 애니메이션
    CModel* m_pModel;                                        // 연결된 모델
    
    // 애니메이션 상태 정보 (기존 인터페이스 호환용)
    float m_currentTime;                                     // 현재 재생 시간
    bool m_isFinished;                                       // 애니메이션 완료 플래그
    bool m_isLooping;                                        // 루프 재생 여부
    float m_playbackSpeed;                                   // 재생 속도 배율
    
    // 캐싱 관련
    mutable std::vector<std::string> m_boneNames;            // 본 이름 캐시
    mutable std::vector<XMMATRIX> m_transformCache;          // 변환 행렬 캐시
    mutable bool m_isCacheValid;                             // 캐시 유효성 플래그
    mutable float m_cachedTime;                              // 캐시된 시간

private:
    SafeCAnimation();
    SafeCAnimation(const SafeCAnimation& rhs) = delete;
    SafeCAnimation& operator=(const SafeCAnimation& rhs) = delete;
    virtual ~SafeCAnimation() = default;

public: // 기존 CAnimation 인터페이스 호환 함수들
    /**
     * @brief Assimp 데이터로 초기화 (기존 CAnimation::NativeConstruct와 동일)
     */
    HRESULT NativeConstruct(aiAnimation* pAnimation, CModel* pModel);
    
    /**
     * @brief 클론 생성 시 초기화 (기존 CAnimation::NativeConstruct_Clone과 동일)
     */
    HRESULT NativeConstruct_Clone(CModel* pModel);
    
    /**
     * @brief 애니메이션 업데이트 (기존 CAnimation::Update_TransformationMatrix와 동일)
     * @param TimeDelta 프레임 시간 간격
     * @param bLoop 루프 재생 여부
     */
    void Update_TransformationMatrix(double TimeDelta, bool bLoop = true);
    
    /**
     * @brief 애니메이션 완료 상태 리셋 (기존과 동일)
     */
    void Animation_Finished();
    
    /**
     * @brief 키프레임 초기화 (기존과 동일)
     */
    void Init_KeyFrame();
    
    /**
     * @brief 다음 애니메이션으로의 선형 보간 전환 (기존과 동일)
     */
    HRESULT Linear_Interpolation_Next_Anim(double TimeDelta, SafeCAnimation* pNextAnim, bool* bFinish);

public: // Getter 함수들 (기존 인터페이스 호환)
    /**
     * @brief 애니메이션 완료 상태 반환
     */
    bool Get_AnimationState() const { return m_isFinished; }
    
    /**
     * @brief 현재 재생 시간 반환
     */
    float GetCurrentTime() const { return m_currentTime; }
    
    /**
     * @brief 애니메이션 총 길이 반환
     */
    float GetDuration() const { return m_pSafeAnimation ? m_pSafeAnimation->GetDuration() : 0.0f; }
    
    /**
     * @brief 애니메이션 이름 반환
     */
    const std::string& GetName() const 
    { 
        static const std::string empty = "";
        return m_pSafeAnimation ? m_pSafeAnimation->GetName() : empty; 
    }

public: // 새로운 기능들
    /**
     * @brief 재생 속도 설정
     * @param speed 재생 속도 (1.0 = 정상, 2.0 = 2배속, 0.5 = 0.5배속)
     */
    void SetPlaybackSpeed(float speed) { m_playbackSpeed = std::max(0.1f, speed); }
    
    /**
     * @brief 현재 재생 속도 반환
     */
    float GetPlaybackSpeed() const { return m_playbackSpeed; }
    
    /**
     * @brief 특정 시간으로 애니메이션 점프
     * @param time 점프할 시간 (초)
     */
    void SeekToTime(float time);
    
    /**
     * @brief 애니메이션 유효성 검사
     * @return 유효하면 true
     */
    bool IsValid() const { return m_pSafeAnimation && m_pSafeAnimation->IsValid(); }
    
    /**
     * @brief 마지막 오류 메시지 반환
     */
    const std::string& GetLastError() const 
    { 
        static const std::string empty = "";
        return m_pSafeAnimation ? m_pSafeAnimation->GetLastError() : empty; 
    }

public: // 디버깅 및 시각화
    /**
     * @brief 애니메이션 정보를 문자열로 출력 (디버깅용)
     */
    std::string GetDebugInfo() const;
    
    /**
     * @brief 현재 프레임의 모든 본 변환 정보 출력 (디버깅용)
     */
    void PrintCurrentTransforms() const;

private: // 내부 헬퍼 함수들
    /**
     * @brief 모델의 모든 본 이름을 캐시에 저장
     */
    void CacheBoneNames();
    
    /**
     * @brief 변환 행렬 캐시 무효화
     */
    void InvalidateCache();
    
    /**
     * @brief 현재 시간에서의 모든 본 변환 행렬을 계산하고 모델에 적용
     */
    void ApplyTransformsToModel();
    
    /**
     * @brief 애니메이션 시간을 유효 범위로 클램핑
     * @param time 클램핑할 시간
     * @param bLoop 루프 모드 여부
     * @return 클램핑된 시간
     */
    float ClampAnimationTime(float time, bool bLoop) const;

public: // 팩토리 함수들
    /**
     * @brief Assimp 데이터로부터 SafeCAnimation 생성
     */
    static SafeCAnimation* Create(aiAnimation* pAnimation, CModel* pModel);
    
    /**
     * @brief 다른 SafeCAnimation로부터 복제 생성
     */
    SafeCAnimation* Clone_Animation(CModel* pModel);

public: // CBase 상속
    virtual void Free() override;
};

/**
 * @brief 새로운 SafeAnimation 시스템을 위한 호환 매크로들
 * 
 * 기존 코드의 수정 없이 새 시스템을 사용할 수 있도록 하는 매크로들
 */
#define SAFE_ANIMATION_CREATE(pAnim, pModel) SafeCAnimation::Create(pAnim, pModel)
#define SAFE_ANIMATION_CLONE(pInstance, pModel) pInstance->Clone_Animation(pModel)

END
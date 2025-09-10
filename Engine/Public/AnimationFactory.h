#pragma once

#include "Base.h"

BEGIN(Engine)

// 전방 선언
class CAnimation;
class SafeCAnimation;
class CModel;

/**
 * @brief 애니메이션 시스템 전환을 위한 팩토리 클래스
 * 
 * 기존 CAnimation과 새로운 SafeCAnimation 시스템 간의
 * 안전한 전환을 지원하는 팩토리 패턴 구현
 */
class ENGINE_DLL AnimationFactory
{
public:
    /**
     * @brief 애니메이션 시스템 종류
     */
    enum SystemType
    {
        SYSTEM_LEGACY,      // 기존 CAnimation 시스템
        SYSTEM_SAFE,        // 새로운 SafeCAnimation 시스템
        SYSTEM_AUTO         // 자동 선택 (설정에 따라)
    };

private:
    static SystemType s_currentSystem;     // 현재 사용 중인 시스템
    static bool s_enableSafeSystem;       // 새 시스템 활성화 여부

public:
    /**
     * @brief Assimp 데이터로부터 애니메이션 생성 (시스템 자동 선택)
     * @param pAnimation Assimp 애니메이션 데이터
     * @param pModel 모델 포인터
     * @return 생성된 애니메이션 포인터 (CAnimation* 형태로 반환)
     */
    static CAnimation* CreateAnimation(aiAnimation* pAnimation, CModel* pModel);
    
    /**
     * @brief 특정 시스템으로 애니메이션 생성
     * @param pAnimation Assimp 애니메이션 데이터
     * @param pModel 모델 포인터
     * @param systemType 사용할 시스템 종류
     * @return 생성된 애니메이션 포인터
     */
    static CAnimation* CreateAnimationWithSystem(aiAnimation* pAnimation, CModel* pModel, SystemType systemType);
    
    /**
     * @brief 애니메이션 복제
     * @param pOriginal 복제할 원본 애니메이션
     * @param pModel 새 모델 포인터
     * @return 복제된 애니메이션 포인터
     */
    static CAnimation* CloneAnimation(CAnimation* pOriginal, CModel* pModel);

public: // 시스템 설정
    /**
     * @brief 사용할 애니메이션 시스템 설정
     * @param systemType 시스템 종류
     */
    static void SetAnimationSystem(SystemType systemType);
    
    /**
     * @brief 현재 사용 중인 시스템 반환
     */
    static SystemType GetCurrentSystem() { return s_currentSystem; }
    
    /**
     * @brief 새 시스템 활성화/비활성화
     * @param enable true면 새 시스템 활성화
     */
    static void EnableSafeSystem(bool enable);
    
    /**
     * @brief 새 시스템 활성화 상태 반환
     */
    static bool IsSafeSystemEnabled() { return s_enableSafeSystem; }

public: // 유틸리티 함수들
    /**
     * @brief 애니메이션이 새 시스템으로 생성되었는지 확인
     * @param pAnimation 확인할 애니메이션
     * @return 새 시스템이면 true
     */
    static bool IsUsingSafeSystem(CAnimation* pAnimation);
    
    /**
     * @brief CAnimation을 SafeCAnimation으로 안전하게 캐스팅
     * @param pAnimation 캐스팅할 애니메이션
     * @return SafeCAnimation 포인터 (실패 시 nullptr)
     */
    static SafeCAnimation* CastToSafeAnimation(CAnimation* pAnimation);
    
    /**
     * @brief 시스템 성능 비교 테스트
     * @return 테스트 결과 문자열
     */
    static std::string RunPerformanceComparison();

public: // 디버깅 및 모니터링
    /**
     * @brief 현재 시스템 상태 정보 반환
     */
    static std::string GetSystemInfo();
    
    /**
     * @brief 애니메이션 통계 정보 반환
     */
    static std::string GetAnimationStats();
    
    /**
     * @brief 시스템 전환 로그 출력
     */
    static void LogSystemTransition(SystemType from, SystemType to);

private:
    AnimationFactory() = delete;    // 정적 클래스로 사용
    
    /**
     * @brief 레거시 시스템으로 애니메이션 생성
     */
    static CAnimation* CreateLegacyAnimation(aiAnimation* pAnimation, CModel* pModel);
    
    /**
     * @brief 새 시스템으로 애니메이션 생성 (SafeCAnimation을 CAnimation으로 포장)
     */
    static CAnimation* CreateSafeAnimation(aiAnimation* pAnimation, CModel* pModel);
    
    /**
     * @brief 시스템 호환성 확인
     */
    static bool CheckSystemCompatibility();
};

/**
 * @brief 편의를 위한 매크로들
 */
#define CREATE_ANIMATION(pAnim, pModel) AnimationFactory::CreateAnimation(pAnim, pModel)
#define CLONE_ANIMATION(pOrig, pModel) AnimationFactory::CloneAnimation(pOrig, pModel)

/**
 * @brief 시스템 전환을 위한 스코프 가드
 * 
 * 특정 스코프에서만 다른 애니메이션 시스템을 사용하고 싶을 때 사용
 */
class ENGINE_DLL AnimationSystemScope
{
private:
    AnimationFactory::SystemType m_previousSystem;

public:
    AnimationSystemScope(AnimationFactory::SystemType tempSystem);
    ~AnimationSystemScope();
};

// 편의 매크로
#define USE_SAFE_ANIMATION_SCOPE() AnimationSystemScope _scope(AnimationFactory::SYSTEM_SAFE)
#define USE_LEGACY_ANIMATION_SCOPE() AnimationSystemScope _scope(AnimationFactory::SYSTEM_LEGACY)

END
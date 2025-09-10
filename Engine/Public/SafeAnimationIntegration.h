#pragma once

#include "Engine_Defines.h"
#include <string>

/**
 * @brief 새로운 SafeAnimation 시스템의 안전한 통합을 위한 헬퍼 클래스
 * 
 * 기존 프로젝트에 새 애니메이션 시스템을 단계별로 안전하게 통합하고
 * 시스템 간 호환성을 검증하는 기능들을 제공합니다.
 */
class ENGINE_DLL SafeAnimationIntegration
{
public:
    /**
     * @brief 통합 단계
     */
    enum IntegrationPhase
    {
        PHASE_SETUP,        // 초기 설정 단계
        PHASE_TESTING,      // 테스트 단계
        PHASE_MIGRATION,    // 마이그레이션 단계
        PHASE_COMPLETE      // 완료 단계
    };
    
    /**
     * @brief 통합 결과 상태
     */
    enum IntegrationStatus
    {
        STATUS_SUCCESS,     // 성공
        STATUS_WARNING,     // 경고 있음
        STATUS_ERROR,       // 오류 발생
        STATUS_FATAL        // 치명적 오류
    };

public:
    /**
     * @brief 전체 통합 프로세스 실행
     * @return 통합 성공 여부
     */
    static bool RunFullIntegration();
    
    /**
     * @brief 단계별 통합 실행
     * @param phase 실행할 단계
     * @return 단계 실행 결과
     */
    static IntegrationStatus RunIntegrationPhase(IntegrationPhase phase);
    
    /**
     * @brief 시스템 호환성 검증
     * @return 검증 결과
     */
    static IntegrationStatus VerifySystemCompatibility();
    
    /**
     * @brief 기존 애니메이션들을 새 시스템으로 테스트
     * @return 테스트 결과
     */
    static IntegrationStatus TestExistingAnimations();

public: // 설정 및 관리
    /**
     * @brief 안전 모드 활성화 (문제 발생 시 자동으로 기존 시스템으로 롤백)
     * @param enable 활성화 여부
     */
    static void EnableSafeMode(bool enable);
    
    /**
     * @brief 현재 통합 상태 반환
     */
    static IntegrationPhase GetCurrentPhase();
    
    /**
     * @brief 통합 진행 상황 로그 출력
     */
    static void LogIntegrationProgress();

public: // 문제 해결 및 복구
    /**
     * @brief 기존 시스템으로 완전 롤백
     */
    static bool RollbackToLegacySystem();
    
    /**
     * @brief 부분적 롤백 (특정 기능만)
     */
    static bool PartialRollback();
    
    /**
     * @brief 시스템 상태 복구
     */
    static bool RestoreSystemState();

public: // 성능 및 안정성 모니터링
    /**
     * @brief 성능 비교 테스트
     * @return 비교 결과 리포트
     */
    static std::string RunPerformanceComparison();
    
    /**
     * @brief 메모리 사용량 모니터링
     * @return 메모리 사용량 리포트
     */
    static std::string GetMemoryUsageReport();
    
    /**
     * @brief 안정성 테스트 (스트레스 테스트)
     * @return 안정성 테스트 결과
     */
    static IntegrationStatus RunStabilityTest();

public: // 유틸리티
    /**
     * @brief 통합 상태를 문자열로 반환
     */
    static std::string GetStatusString(IntegrationStatus status);
    
    /**
     * @brief 통합 단계를 문자열로 반환
     */
    static std::string GetPhaseString(IntegrationPhase phase);
    
    /**
     * @brief 상세한 시스템 리포트 생성
     */
    static std::string GenerateDetailedReport();

private:
    static IntegrationPhase s_currentPhase;
    static bool s_safeMode;
    static IntegrationStatus s_lastStatus;

private:
    SafeAnimationIntegration() = delete;
    
    /**
     * @brief 시스템 초기화 검증
     */
    static IntegrationStatus VerifySystemInitialization();
    
    /**
     * @brief 기본 기능 테스트
     */
    static IntegrationStatus TestBasicFunctionality();
    
    /**
     * @brief 고급 기능 테스트
     */
    static IntegrationStatus TestAdvancedFeatures();
    
    /**
     * @brief 에러 핸들링 테스트
     */
    static IntegrationStatus TestErrorHandling();
    
    /**
     * @brief 통합 로그 출력
     */
    static void LogMessage(const std::string& message, IntegrationStatus status = STATUS_SUCCESS);
};

/**
 * @brief 통합 과정의 자동 모니터링을 위한 RAII 클래스
 */
class ENGINE_DLL IntegrationMonitor
{
private:
    std::string m_operationName;
    std::chrono::high_resolution_clock::time_point m_startTime;

public:
    IntegrationMonitor(const std::string& operationName);
    ~IntegrationMonitor();
    
    /**
     * @brief 중간 체크포인트 기록
     */
    void Checkpoint(const std::string& checkpointName);
};

// 편의 매크로
#define INTEGRATION_MONITOR(name) IntegrationMonitor _monitor(name)
#define INTEGRATION_CHECKPOINT(name) _monitor.Checkpoint(name)

END
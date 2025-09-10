#pragma once

#include "Engine_Defines.h"

/**
 * @brief SafeAnimation 시스템 데모 및 통합 테스트
 * 
 * 새로운 애니메이션 시스템의 동작을 확인하고
 * 기존 시스템과의 호환성을 검증하는 데모 클래스
 */
class ENGINE_DLL SafeAnimationDemo
{
public:
    /**
     * @brief 모든 데모 실행
     */
    static void RunAllDemos();
    
    /**
     * @brief 기본 애니메이션 보간 데모
     */
    static void DemoBasicInterpolation();
    
    /**
     * @brief 복잡한 키프레임 애니메이션 데모
     */
    static void DemoComplexAnimation();
    
    /**
     * @brief 성능 비교 데모 (기존 vs 새 시스템)
     */
    static void DemoPerformanceComparison();
    
    /**
     * @brief 기존 시스템 호환성 데모
     */
    static void DemoLegacyCompatibility();
    
    /**
     * @brief 오류 처리 및 안전성 데모
     */
    static void DemoErrorHandling();

private:
    SafeAnimationDemo() = delete;
    
    /**
     * @brief 테스트 결과 출력
     */
    static void PrintTestResult(const std::string& testName, bool success, const std::string& details = "");
    
    /**
     * @brief 성능 측정 헬퍼
     */
    static double MeasureExecutionTime(std::function<void()> func);
};
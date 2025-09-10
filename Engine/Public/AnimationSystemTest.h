#pragma once

#include "Engine_Defines.h"

/**
 * @brief 새로운 애니메이션 시스템의 통합 테스트 및 검증
 * 
 * 실제 게임에서 사용하기 전에 새 시스템이 올바르게 작동하는지
 * 확인하기 위한 테스트 함수들
 */
class ENGINE_DLL AnimationSystemTest
{
public:
    /**
     * @brief 새 애니메이션 시스템의 전체 통합 테스트 실행
     * @return 모든 테스트 성공 시 true
     */
    static bool RunIntegrationTests();
    
    /**
     * @brief CAnimation typedef가 올바르게 작동하는지 테스트
     * @return 성공 시 true
     */
    static bool TestAnimationTypedef();
    
    /**
     * @brief 애니메이션 생성 및 재생 테스트
     * @return 성공 시 true
     */
    static bool TestAnimationCreation();
    
    /**
     * @brief 기존 인터페이스 호환성 테스트
     * @return 성공 시 true
     */
    static bool TestLegacyInterface();
    
    /**
     * @brief 메모리 안전성 테스트
     * @return 성공 시 true
     */
    static bool TestMemorySafety();

private:
    AnimationSystemTest() = delete;
    
    /**
     * @brief 테스트 결과를 콘솔에 출력
     */
    static void LogTestResult(const std::string& testName, bool success, const std::string& details = "");
};
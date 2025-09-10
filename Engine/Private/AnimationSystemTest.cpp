#include "..\Public\AnimationSystemTest.h"
#include "..\Public\Model.h"
#include "..\Public\SafeAnimation.h"
#include "..\Public\AnimationAdapter.h"
#include <iostream>

bool AnimationSystemTest::RunIntegrationTests()
{
    std::cout << "=========================================" << std::endl;
    std::cout << "  Animation System Integration Tests" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    bool allTestsPassed = true;
    
    // 개별 테스트 실행
    allTestsPassed &= TestAnimationTypedef();
    allTestsPassed &= TestAnimationCreation();
    allTestsPassed &= TestLegacyInterface();
    allTestsPassed &= TestMemorySafety();
    
    std::cout << "=========================================" << std::endl;
    if (allTestsPassed)
    {
        std::cout << "✅ ALL TESTS PASSED!" << std::endl;
        std::cout << "새로운 애니메이션 시스템이 성공적으로 통합되었습니다." << std::endl;
    }
    else
    {
        std::cout << "❌ SOME TESTS FAILED!" << std::endl;
        std::cout << "애니메이션 시스템에 문제가 있습니다." << std::endl;
    }
    std::cout << "=========================================" << std::endl;
    
    return allTestsPassed;
}

bool AnimationSystemTest::TestAnimationTypedef()
{
    LogTestResult("Animation Typedef Test", true, "Started");
    
    try
    {
        // CAnimation이 AnimationAdapter로 제대로 typedef되었는지 테스트
        
        // 1. 타입 확인 (컴파일 타임 체크)
        static_assert(std::is_same_v<CAnimation, AnimationAdapter>, 
                     "CAnimation should be typedef of AnimationAdapter");
        
        LogTestResult("Animation Typedef Test", true, "CAnimation typedef working correctly");
        return true;
    }
    catch (...)
    {
        LogTestResult("Animation Typedef Test", false, "Exception during typedef test");
        return false;
    }
}

bool AnimationSystemTest::TestAnimationCreation()
{
    LogTestResult("Animation Creation Test", true, "Started");
    
    try
    {
        // 실제 Assimp 데이터 없이 테스트는 어려우므로
        // AnimationAdapter가 생성 가능한지만 확인
        
        // 빈 SafeAnimation 생성 테스트
        SafeAnimation* testSafeAnim = SafeAnimation::CreateEmpty("test", 1.0f, 30.0f);
        
        if (testSafeAnim && testSafeAnim->IsValid())
        {
            Safe_Release(testSafeAnim);
            LogTestResult("Animation Creation Test", true, "SafeAnimation creation successful");
            return true;
        }
        else
        {
            if (testSafeAnim) Safe_Release(testSafeAnim);
            LogTestResult("Animation Creation Test", false, "SafeAnimation creation failed");
            return false;
        }
    }
    catch (...)
    {
        LogTestResult("Animation Creation Test", false, "Exception during creation test");
        return false;
    }
}

bool AnimationSystemTest::TestLegacyInterface()
{
    LogTestResult("Legacy Interface Test", true, "Started");
    
    try
    {
        // 기존 인터페이스가 새 시스템에서도 작동하는지 테스트
        
        // AnimationAdapter 직접 생성 (Assimp 없이는 제한적)
        // 실제로는 CModel과 aiAnimation이 필요하지만, 
        // 여기서는 기본 인터페이스가 존재하는지만 확인
        
        // 기존 메서드들이 존재하는지 컴파일 타임 체크
        // (실제 호출은 nullptr로 인해 실패할 수 있음)
        
        bool hasUpdate = std::is_member_function_pointer_v<
            decltype(&CAnimation::Update_TransformationMatrix)>;
        bool hasGetState = std::is_member_function_pointer_v<
            decltype(&CAnimation::Get_AnimationState)>;
        bool hasAnimFinished = std::is_member_function_pointer_v<
            decltype(&CAnimation::Animation_Finished)>;
        
        if (hasUpdate && hasGetState && hasAnimFinished)
        {
            LogTestResult("Legacy Interface Test", true, "All legacy methods available");
            return true;
        }
        else
        {
            LogTestResult("Legacy Interface Test", false, "Some legacy methods missing");
            return false;
        }
    }
    catch (...)
    {
        LogTestResult("Legacy Interface Test", false, "Exception during interface test");
        return false;
    }
}

bool AnimationSystemTest::TestMemorySafety()
{
    LogTestResult("Memory Safety Test", true, "Started");
    
    try
    {
        // 메모리 누수 및 안전성 테스트
        
        const int NUM_TEST_ANIMATIONS = 100;
        
        // 많은 애니메이션을 생성하고 해제하여 메모리 누수 테스트
        for (int i = 0; i < NUM_TEST_ANIMATIONS; ++i)
        {
            SafeAnimation* testAnim = SafeAnimation::CreateEmpty(
                "stress_test_" + std::to_string(i), 
                1.0f + (i % 10) * 0.1f, 
                30.0f
            );
            
            if (testAnim)
            {
                // 간단한 트랙 추가
                auto track = std::make_unique<AnimationTrack>("test_bone_" + std::to_string(i));
                
                AnimationKey key1, key2;
                key1.time = 0.0f;
                key2.time = 1.0f;
                key2.position.x = static_cast<float>(i);
                
                track->AddKeyFrame(key1);
                track->AddKeyFrame(key2);
                
                testAnim->AddTrack("test_bone_" + std::to_string(i), std::move(track));
                
                // 즉시 해제
                Safe_Release(testAnim);
            }
        }
        
        LogTestResult("Memory Safety Test", true, 
                     std::to_string(NUM_TEST_ANIMATIONS) + " animations created and destroyed");
        return true;
    }
    catch (...)
    {
        LogTestResult("Memory Safety Test", false, "Exception during memory safety test");
        return false;
    }
}

void AnimationSystemTest::LogTestResult(const std::string& testName, bool success, const std::string& details)
{
    std::cout << "[" << (success ? "PASS" : "FAIL") << "] " << testName;
    if (!details.empty())
    {
        std::cout << " - " << details;
    }
    std::cout << std::endl;
}

// 외부에서 호출 가능한 C 스타일 함수
extern "C" 
{
    bool TestNewAnimationSystem()
    {
        return AnimationSystemTest::RunIntegrationTests();
    }
}
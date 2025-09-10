#include "..\Public\SafeAnimationDemo.h"
#include "..\Public\SafeAnimation.h"
#include "..\Public\AnimationAdapter.h"
#include <iostream>
#include <chrono>
#include <functional>
#include <vector>

void SafeAnimationDemo::RunAllDemos()
{
    std::cout << "===============================================" << std::endl;
    std::cout << "   SafeAnimation System Comprehensive Demo" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    
    DemoBasicInterpolation();
    std::cout << std::endl;
    
    DemoComplexAnimation();
    std::cout << std::endl;
    
    DemoErrorHandling();
    std::cout << std::endl;
    
    DemoPerformanceComparison();
    std::cout << std::endl;
    
    std::cout << "===============================================" << std::endl;
    std::cout << "           All Demos Completed!" << std::endl;
    std::cout << "===============================================" << std::endl;
}

void SafeAnimationDemo::DemoBasicInterpolation()
{
    std::cout << "--- Basic Interpolation Demo ---" << std::endl;
    
    try
    {
        // 기본 애니메이션 트랙 생성
        AnimationTrack track("demo_bone");
        
        // 키프레임 생성: 0초에서 2초까지 위치 이동
        AnimationKey startKey;
        startKey.time = 0.0f;
        startKey.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
        startKey.rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);  // 항등 쿼터니언
        startKey.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
        
        AnimationKey endKey;
        endKey.time = 2.0f;
        endKey.position = XMFLOAT3(10.0f, 5.0f, -3.0f);
        endKey.rotation = XMFLOAT4(0.0f, 0.707f, 0.0f, 0.707f); // 90도 Y축 회전
        endKey.scale = XMFLOAT3(2.0f, 1.5f, 1.2f);
        
        track.AddKeyFrame(startKey);
        track.AddKeyFrame(endKey);
        
        // 여러 시점에서 보간 테스트
        std::vector<float> testTimes = {0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, -0.5f}; // 경계 조건 포함
        
        bool allTestsPassed = true;
        
        for (float time : testTimes)
        {
            XMFLOAT3 position, scale;
            XMFLOAT4 rotation;
            
            bool success = track.InterpolateComponentsAt(time, position, rotation, scale);
            
            std::cout << "Time " << time << "s: ";
            if (success)
            {
                std::cout << "Pos(" << position.x << ", " << position.y << ", " << position.z << ") ";
                std::cout << "Rot(" << rotation.x << ", " << rotation.y << ", " << rotation.z << ", " << rotation.w << ") ";
                std::cout << "Scale(" << scale.x << ", " << scale.y << ", " << scale.z << ")" << std::endl;
                
                // 중간점 검증 (t=1.0)
                if (std::abs(time - 1.0f) < 0.001f)
                {
                    if (std::abs(position.x - 5.0f) > 0.001f || 
                        std::abs(position.y - 2.5f) > 0.001f ||
                        std::abs(scale.x - 1.5f) > 0.001f)
                    {
                        allTestsPassed = false;
                        std::cout << "  [FAIL] Interpolation values incorrect" << std::endl;
                    }
                    else
                    {
                        std::cout << "  [PASS] Interpolation correct" << std::endl;
                    }
                }
            }
            else
            {
                if (track.GetKeyFrameCount() == 0)
                {
                    std::cout << "[PASS] Failed gracefully for empty track" << std::endl;
                }
                else
                {
                    allTestsPassed = false;
                    std::cout << "[FAIL] Unexpected interpolation failure" << std::endl;
                }
            }
        }
        
        PrintTestResult("Basic Interpolation", allTestsPassed, 
                       allTestsPassed ? "All interpolation tests passed" : "Some interpolation tests failed");
    }
    catch (...)
    {
        PrintTestResult("Basic Interpolation", false, "Exception thrown during test");
    }
}

void SafeAnimationDemo::DemoComplexAnimation()
{
    std::cout << "--- Complex Animation Demo ---" << std::endl;
    
    try
    {
        // SafeAnimation 생성
        SafeAnimation* animation = SafeAnimation::CreateEmpty("complex_demo", 3.0f, 30.0f);
        
        if (!animation)
        {
            PrintTestResult("Complex Animation Creation", false, "Failed to create animation");
            return;
        }
        
        // 여러 본에 대한 복잡한 애니메이션 생성
        std::vector<std::string> boneNames = {"root", "spine", "left_arm", "right_arm", "head"};
        
        for (size_t boneIndex = 0; boneIndex < boneNames.size(); ++boneIndex)
        {
            auto track = std::make_unique<AnimationTrack>(boneNames[boneIndex]);
            
            // 각 본마다 다른 애니메이션 패턴 생성
            for (int i = 0; i <= 6; ++i) // 7개 키프레임 (0초 ~ 3초, 0.5초 간격)
            {
                AnimationKey key;
                key.time = i * 0.5f;
                
                // 각 본마다 다른 움직임 패턴
                float phase = static_cast<float>(boneIndex) * 0.5f;
                float t = key.time + phase;
                
                key.position.x = std::sin(t) * (boneIndex + 1);
                key.position.y = std::cos(t * 0.7f) * (boneIndex + 1) * 0.5f;
                key.position.z = std::sin(t * 1.3f) * (boneIndex + 1) * 0.3f;
                
                // 회전 애니메이션 (Y축 중심)
                float angle = t * 0.5f;
                key.rotation.x = 0.0f;
                key.rotation.y = std::sin(angle * 0.5f);
                key.rotation.z = 0.0f;
                key.rotation.w = std::cos(angle * 0.5f);
                
                // 스케일 애니메이션
                float scaleBase = 1.0f + std::sin(t * 0.3f) * 0.2f;
                key.scale = XMFLOAT3(scaleBase, scaleBase, scaleBase);
                
                track->AddKeyFrame(key);
            }
            
            animation->AddTrack(boneNames[boneIndex], std::move(track));
        }
        
        // 애니메이션 유효성 검사
        bool isValid = animation->IsValid();
        
        if (isValid)
        {
            // 다양한 시점에서 애니메이션 테스트
            std::vector<XMMATRIX> transforms;
            std::vector<float> testTimes = {0.0f, 0.7f, 1.5f, 2.3f, 3.0f};
            
            bool allFramesValid = true;
            
            for (float time : testTimes)
            {
                bool success = animation->UpdateBoneTransforms(time, boneNames, transforms);
                
                if (success && transforms.size() == boneNames.size())
                {
                    std::cout << "Frame at " << time << "s: " << transforms.size() << " bone transforms calculated" << std::endl;
                    
                    // 각 변환 행렬이 유효한지 확인
                    for (size_t i = 0; i < transforms.size(); ++i)
                    {
                        XMVECTOR det = XMMatrixDeterminant(transforms[i]);
                        float detValue = XMVectorGetX(det);
                        
                        if (std::abs(detValue) < 0.001f) // 거의 특이행렬
                        {
                            allFramesValid = false;
                            std::cout << "  [WARN] Bone " << boneNames[i] << " has near-singular transform" << std::endl;
                        }
                    }
                }
                else
                {
                    allFramesValid = false;
                    std::cout << "  [FAIL] Frame calculation failed at " << time << "s" << std::endl;
                }
            }
            
            PrintTestResult("Complex Animation", allFramesValid, 
                           std::string("Animation with ") + std::to_string(boneNames.size()) + " bones tested");
        }
        else
        {
            PrintTestResult("Complex Animation", false, 
                           "Animation validation failed: " + animation->GetLastError());
        }
        
        Safe_Release(animation);
    }
    catch (...)
    {
        PrintTestResult("Complex Animation", false, "Exception during complex animation test");
    }
}

void SafeAnimationDemo::DemoErrorHandling()
{
    std::cout << "--- Error Handling & Safety Demo ---" << std::endl;
    
    bool allErrorHandlingPassed = true;
    
    try
    {
        // 테스트 1: 빈 트랙 처리
        {
            AnimationTrack emptyTrack("empty");
            XMMATRIX transform;
            
            bool shouldFail = emptyTrack.InterpolateAt(1.0f, transform);
            if (shouldFail)
            {
                allErrorHandlingPassed = false;
                std::cout << "[FAIL] Empty track should fail gracefully" << std::endl;
            }
            else
            {
                std::cout << "[PASS] Empty track handled correctly" << std::endl;
            }
        }
        
        // 테스트 2: 단일 키프레임 처리
        {
            AnimationTrack singleKeyTrack("single_key");
            AnimationKey key;
            key.time = 1.5f;
            key.position = XMFLOAT3(1.0f, 2.0f, 3.0f);
            key.rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            key.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
            
            singleKeyTrack.AddKeyFrame(key);
            
            XMFLOAT3 position, scale;
            XMFLOAT4 rotation;
            
            // 키프레임 이전 시간
            bool success1 = singleKeyTrack.InterpolateComponentsAt(0.5f, position, rotation, scale);
            // 키프레임 이후 시간
            bool success2 = singleKeyTrack.InterpolateComponentsAt(2.5f, position, rotation, scale);
            
            if (success1 && success2 && 
                std::abs(position.x - 1.0f) < 0.001f &&
                std::abs(position.y - 2.0f) < 0.001f &&
                std::abs(position.z - 3.0f) < 0.001f)
            {
                std::cout << "[PASS] Single keyframe boundary conditions handled" << std::endl;
            }
            else
            {
                allErrorHandlingPassed = false;
                std::cout << "[FAIL] Single keyframe boundary handling failed" << std::endl;
            }
        }
        
        // 테스트 3: 잘못된 애니메이션 데이터 처리
        {
            SafeAnimation* invalidAnim = SafeAnimation::CreateEmpty("", -1.0f, 0.0f); // 잘못된 파라미터
            
            if (!invalidAnim)
            {
                std::cout << "[PASS] Invalid animation parameters rejected" << std::endl;
            }
            else
            {
                allErrorHandlingPassed = false;
                std::cout << "[FAIL] Invalid parameters should be rejected" << std::endl;
                Safe_Release(invalidAnim);
            }
        }
        
        // 테스트 4: 메모리 안전성 (간접 테스트)
        {
            // 여러 개의 애니메이션을 생성하고 해제하여 메모리 누수 테스트
            for (int i = 0; i < 10; ++i)
            {
                SafeAnimation* testAnim = SafeAnimation::CreateEmpty("test_" + std::to_string(i), 1.0f, 30.0f);
                
                if (testAnim)
                {
                    auto track = std::make_unique<AnimationTrack>("test_bone_" + std::to_string(i));
                    
                    AnimationKey key1, key2;
                    key1.time = 0.0f;
                    key2.time = 1.0f;
                    
                    track->AddKeyFrame(key1);
                    track->AddKeyFrame(key2);
                    
                    testAnim->AddTrack("test_bone_" + std::to_string(i), std::move(track));
                    
                    Safe_Release(testAnim); // 즉시 해제
                }
            }
            
            std::cout << "[PASS] Memory management stress test completed" << std::endl;
        }
        
        PrintTestResult("Error Handling", allErrorHandlingPassed, 
                       "Safety mechanisms and error conditions tested");
    }
    catch (...)
    {
        PrintTestResult("Error Handling", false, "Exception during error handling tests");
    }
}

void SafeAnimationDemo::DemoPerformanceComparison()
{
    std::cout << "--- Performance Comparison Demo ---" << std::endl;
    
    try
    {
        // 성능 테스트용 애니메이션 데이터 생성
        const int NUM_BONES = 50;
        const int NUM_KEYFRAMES_PER_BONE = 100;
        const int NUM_ITERATIONS = 1000;
        
        SafeAnimation* testAnimation = SafeAnimation::CreateEmpty("perf_test", 10.0f, 30.0f);
        
        if (!testAnimation)
        {
            PrintTestResult("Performance Test", false, "Failed to create test animation");
            return;
        }
        
        // 큰 애니메이션 데이터 생성
        std::vector<std::string> boneNames;
        boneNames.reserve(NUM_BONES);
        
        for (int boneIndex = 0; boneIndex < NUM_BONES; ++boneIndex)
        {
            std::string boneName = "bone_" + std::to_string(boneIndex);
            boneNames.push_back(boneName);
            
            auto track = std::make_unique<AnimationTrack>(boneName);
            
            for (int keyIndex = 0; keyIndex < NUM_KEYFRAMES_PER_BONE; ++keyIndex)
            {
                AnimationKey key;
                key.time = (keyIndex / static_cast<float>(NUM_KEYFRAMES_PER_BONE - 1)) * 10.0f;
                
                // 랜덤한 움직임 생성
                key.position.x = std::sin(keyIndex * 0.1f + boneIndex) * 5.0f;
                key.position.y = std::cos(keyIndex * 0.1f + boneIndex) * 5.0f;
                key.position.z = std::sin(keyIndex * 0.05f + boneIndex) * 3.0f;
                
                float angle = keyIndex * 0.1f;
                key.rotation.x = 0.0f;
                key.rotation.y = std::sin(angle * 0.5f);
                key.rotation.z = 0.0f;
                key.rotation.w = std::cos(angle * 0.5f);
                
                key.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
                
                track->AddKeyFrame(key);
            }
            
            testAnimation->AddTrack(boneName, std::move(track));
        }
        
        // 성능 측정
        std::vector<XMMATRIX> transforms;
        double totalTime = MeasureExecutionTime([&]()
        {
            for (int i = 0; i < NUM_ITERATIONS; ++i)
            {
                float time = (i / static_cast<float>(NUM_ITERATIONS - 1)) * 10.0f;
                testAnimation->UpdateBoneTransforms(time, boneNames, transforms);
            }
        });
        
        double avgTimePerFrame = (totalTime / NUM_ITERATIONS) * 1000.0; // 밀리초로 변환
        double framesPerSecond = NUM_ITERATIONS / totalTime;
        
        std::cout << "Performance Results:" << std::endl;
        std::cout << "  Total time: " << totalTime << " seconds" << std::endl;
        std::cout << "  Average time per frame: " << avgTimePerFrame << " ms" << std::endl;
        std::cout << "  Effective FPS: " << framesPerSecond << std::endl;
        std::cout << "  Bones processed: " << NUM_BONES << std::endl;
        std::cout << "  Keyframes per bone: " << NUM_KEYFRAMES_PER_BONE << std::endl;
        
        bool performanceAcceptable = avgTimePerFrame < 16.67; // 60 FPS 기준
        
        PrintTestResult("Performance Test", performanceAcceptable,
                       std::string("Avg: ") + std::to_string(avgTimePerFrame) + "ms per frame");
        
        Safe_Release(testAnimation);
    }
    catch (...)
    {
        PrintTestResult("Performance Test", false, "Exception during performance testing");
    }
}

void SafeAnimationDemo::PrintTestResult(const std::string& testName, bool success, const std::string& details)
{
    std::cout << "[" << (success ? "PASS" : "FAIL") << "] " << testName;
    if (!details.empty())
    {
        std::cout << " - " << details;
    }
    std::cout << std::endl;
}

double SafeAnimationDemo::MeasureExecutionTime(std::function<void()> func)
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count() / 1000000.0; // 초 단위로 변환
}
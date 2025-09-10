#include "..\Public\SafeAnimation.h"
#include <iostream>
#include <cassert>

// 간단한 단위 테스트 함수들
namespace SafeAnimationTest
{
    /**
     * @brief 보간 함수 테스트
     */
    void TestInterpolation()
    {
        std::cout << "Testing AnimationTrack interpolation..." << std::endl;
        
        // 테스트용 애니메이션 트랙 생성
        AnimationTrack track("test_bone");
        
        // 키프레임 추가
        AnimationKey key1;
        key1.time = 0.0f;
        key1.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
        key1.rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        key1.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
        
        AnimationKey key2;
        key2.time = 1.0f;
        key2.position = XMFLOAT3(10.0f, 10.0f, 10.0f);
        key2.rotation = XMFLOAT4(0.0f, 0.707f, 0.0f, 0.707f); // 90도 Y축 회전
        key2.scale = XMFLOAT3(2.0f, 2.0f, 2.0f);
        
        track.AddKeyFrame(key1);
        track.AddKeyFrame(key2);
        
        // 중간 지점 보간 테스트 (t = 0.5)
        XMFLOAT3 position, scale;
        XMFLOAT4 rotation;
        
        bool success = track.InterpolateComponentsAt(0.5f, position, rotation, scale);
        assert(success && "Interpolation should succeed");
        
        // 결과 검증
        assert(std::abs(position.x - 5.0f) < 0.001f && "Position X interpolation failed");
        assert(std::abs(position.y - 5.0f) < 0.001f && "Position Y interpolation failed");
        assert(std::abs(position.z - 5.0f) < 0.001f && "Position Z interpolation failed");
        
        assert(std::abs(scale.x - 1.5f) < 0.001f && "Scale X interpolation failed");
        assert(std::abs(scale.y - 1.5f) < 0.001f && "Scale Y interpolation failed");
        assert(std::abs(scale.z - 1.5f) < 0.001f && "Scale Z interpolation failed");
        
        std::cout << "Interpolation test passed!" << std::endl;
    }
    
    /**
     * @brief 경계 조건 테스트 (기존 버그 수정 검증)
     */
    void TestBoundaryConditions()
    {
        std::cout << "Testing boundary conditions..." << std::endl;
        
        AnimationTrack track("boundary_test");
        
        // 단일 키프레임만 있는 경우
        AnimationKey singleKey;
        singleKey.time = 0.5f;
        singleKey.position = XMFLOAT3(1.0f, 2.0f, 3.0f);
        singleKey.rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        singleKey.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
        
        track.AddKeyFrame(singleKey);
        
        XMMATRIX transform;
        
        // 키프레임 시간 이전 테스트
        bool success1 = track.InterpolateAt(0.0f, transform);
        assert(success1 && "Should succeed even with single keyframe");
        
        // 키프레임 시간 이후 테스트
        bool success2 = track.InterpolateAt(1.0f, transform);
        assert(success2 && "Should succeed even with single keyframe");
        
        // 빈 트랙 테스트
        AnimationTrack emptyTrack("empty");
        bool success3 = emptyTrack.InterpolateAt(0.5f, transform);
        assert(!success3 && "Empty track should fail gracefully");
        
        std::cout << "Boundary conditions test passed!" << std::endl;
    }
    
    /**
     * @brief 이진 탐색 테스트
     */
    void TestBinarySearch()
    {
        std::cout << "Testing binary search keyframe finding..." << std::endl;
        
        AnimationTrack track("search_test");
        
        // 여러 키프레임 추가
        for (int i = 0; i < 10; ++i)
        {
            AnimationKey key;
            key.time = static_cast<float>(i * 0.1f);  // 0.0, 0.1, 0.2, ..., 0.9
            key.position = XMFLOAT3(static_cast<float>(i), 0.0f, 0.0f);
            key.rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            key.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
            track.AddKeyFrame(key);
        }
        
        // 다양한 시간에서 보간 테스트
        XMFLOAT3 position, scale;
        XMFLOAT4 rotation;
        
        // 중간 지점들 테스트
        bool success1 = track.InterpolateComponentsAt(0.25f, position, rotation, scale);
        assert(success1 && "Interpolation at 0.25 should succeed");
        assert(std::abs(position.x - 2.5f) < 0.001f && "Position interpolation at 0.25 failed");
        
        bool success2 = track.InterpolateComponentsAt(0.75f, position, rotation, scale);
        assert(success2 && "Interpolation at 0.75 should succeed");
        assert(std::abs(position.x - 7.5f) < 0.001f && "Position interpolation at 0.75 failed");
        
        std::cout << "Binary search test passed!" << std::endl;
    }
    
    /**
     * @brief SafeAnimation 클래스 테스트
     */
    void TestSafeAnimation()
    {
        std::cout << "Testing SafeAnimation class..." << std::endl;
        
        // 빈 애니메이션 생성
        SafeAnimation* animation = SafeAnimation::CreateEmpty("test_animation", 2.0f, 30.0f);
        assert(animation != nullptr && "Animation creation should succeed");
        
        // 트랙 추가
        auto track = std::make_unique<AnimationTrack>("test_bone");
        
        AnimationKey key1, key2;
        key1.time = 0.0f;
        key1.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
        key1.rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        key1.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
        
        key2.time = 2.0f;
        key2.position = XMFLOAT3(20.0f, 20.0f, 20.0f);
        key2.rotation = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f); // 180도 Y축 회전
        key2.scale = XMFLOAT3(3.0f, 3.0f, 3.0f);
        
        track->AddKeyFrame(key1);
        track->AddKeyFrame(key2);
        
        bool addSuccess = animation->AddTrack("test_bone", std::move(track));
        assert(addSuccess && "Track addition should succeed");
        
        // 애니메이션 재생 테스트
        XMMATRIX transform;
        bool getSuccess = animation->GetBoneTransformAt(1.0f, "test_bone", transform);
        assert(getSuccess && "Bone transform retrieval should succeed");
        
        // 존재하지 않는 본 테스트
        bool getFailure = animation->GetBoneTransformAt(1.0f, "nonexistent_bone", transform);
        assert(!getFailure && "Nonexistent bone should fail gracefully");
        
        Safe_Release(animation);
        
        std::cout << "SafeAnimation test passed!" << std::endl;
    }
    
    /**
     * @brief 모든 테스트 실행
     */
    void RunAllTests()
    {
        std::cout << "=== SafeAnimation System Unit Tests ===" << std::endl;
        
        try
        {
            TestInterpolation();
            TestBoundaryConditions();
            TestBinarySearch();
            TestSafeAnimation();
            
            std::cout << "=== All tests passed! ===" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Test failed with exception: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Test failed with unknown exception!" << std::endl;
        }
    }
}

// 테스트 실행을 위한 함수 (외부에서 호출 가능)
extern "C" void RunSafeAnimationTests()
{
    SafeAnimationTest::RunAllTests();
}
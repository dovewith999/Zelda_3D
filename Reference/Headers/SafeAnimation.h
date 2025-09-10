#pragma once

#include "Base.h"
#include <vector>
#include <memory>
#include <string>

BEGIN(Engine)

/**
 * @brief 단일 시점의 애니메이션 키프레임 데이터
 * 
 * Position, Rotation, Scale을 하나의 시간점에 통합하여 관리
 * 기존 시스템의 키프레임 시간 불일치 문제를 해결
 */
struct ENGINE_DLL AnimationKey
{
    float time;                 // 키프레임 시간 (초 단위)
    XMFLOAT3 position;         // 위치
    XMFLOAT4 rotation;         // 회전 (쿼터니언: x, y, z, w)
    XMFLOAT3 scale;            // 스케일
    
    // 기본 생성자 - 항등 변환으로 초기화
    AnimationKey() : time(0.0f), 
                    position(0.0f, 0.0f, 0.0f),
                    rotation(0.0f, 0.0f, 0.0f, 1.0f),  // 항등 쿼터니언
                    scale(1.0f, 1.0f, 1.0f) {}
    
    // 시간 기준 정렬을 위한 비교 연산자
    bool operator<(const AnimationKey& other) const { return time < other.time; }
};

/**
 * @brief 단일 본(Bone)에 대한 애니메이션 트랙
 * 
 * 해당 본의 모든 키프레임을 시간순으로 관리하고
 * 안전한 보간 기능을 제공합니다.
 */
class ENGINE_DLL AnimationTrack final
{
private:
    std::string m_boneName;                      // 본 이름
    std::vector<AnimationKey> m_keyFrames;       // 시간순 정렬된 키프레임들
    bool m_isSorted;                             // 정렬 상태 플래그

public:
    AnimationTrack();
    explicit AnimationTrack(const std::string& boneName);
    ~AnimationTrack() = default;

    // 복사 생성자 및 할당 연산자
    AnimationTrack(const AnimationTrack& other);
    AnimationTrack& operator=(const AnimationTrack& other);

    // 이동 생성자 및 할당 연산자
    AnimationTrack(AnimationTrack&& other) noexcept;
    AnimationTrack& operator=(AnimationTrack&& other) noexcept;

public: // Getter 함수들
    const std::string& GetBoneName() const { return m_boneName; }
    size_t GetKeyFrameCount() const { return m_keyFrames.size(); }
    float GetDuration() const;
    bool IsEmpty() const { return m_keyFrames.empty(); }
    const std::vector<AnimationKey>& GetKeyFrames() const { return m_keyFrames; }

public: // 키프레임 관리
    /**
     * @brief 키프레임 추가
     * @param keyFrame 추가할 키프레임
     * @return 성공 시 true
     */
    bool AddKeyFrame(const AnimationKey& keyFrame);
    
    /**
     * @brief 여러 키프레임 일괄 추가
     * @param keyFrames 추가할 키프레임 벡터
     * @return 성공 시 true
     */
    bool AddKeyFrames(const std::vector<AnimationKey>& keyFrames);
    
    /**
     * @brief 키프레임들을 시간순으로 정렬
     */
    void SortKeyFramesByTime();
    
    /**
     * @brief 키프레임 데이터 유효성 검사
     * @return 유효하면 true
     */
    bool ValidateKeyFrames() const;
    
    /**
     * @brief 모든 키프레임 제거
     */
    void ClearKeyFrames();

public: // 보간 및 애니메이션 재생
    /**
     * @brief 지정된 시간에서의 변환 행렬 계산 (안전한 보간)
     * @param time 애니메이션 시간 (초)
     * @param outTransform 출력될 변환 행렬
     * @return 성공 시 true, 실패 시 false
     */
    bool InterpolateAt(float time, XMMATRIX& outTransform) const;
    
    /**
     * @brief 지정된 시간에서의 Position, Rotation, Scale 개별 계산
     * @param time 애니메이션 시간 (초)
     * @param outPosition 출력될 위치
     * @param outRotation 출력될 회전 (쿼터니언)
     * @param outScale 출력될 스케일
     * @return 성공 시 true, 실패 시 false
     */
    bool InterpolateComponentsAt(float time, XMFLOAT3& outPosition, 
                                XMFLOAT4& outRotation, XMFLOAT3& outScale) const;

private: // 내부 헬퍼 함수들
    /**
     * @brief 이진 탐색으로 주어진 시간에 해당하는 키프레임 인덱스 찾기
     * @param time 찾을 시간
     * @return 키프레임 인덱스 (time 이하의 가장 큰 시간을 가진 키프레임)
     */
    size_t FindKeyFrameIndex(float time) const;
    
    /**
     * @brief 두 키프레임 사이의 보간 비율 계산 (안전한 버전)
     * @param time 현재 시간
     * @param keyIndex1 첫 번째 키프레임 인덱스
     * @param keyIndex2 두 번째 키프레임 인덱스
     * @return 보간 비율 (0.0f ~ 1.0f)
     */
    float CalculateInterpolationRatio(float time, size_t keyIndex1, size_t keyIndex2) const;
    
    /**
     * @brief 위치 벡터 선형 보간
     */
    XMFLOAT3 InterpolatePosition(const XMFLOAT3& pos1, const XMFLOAT3& pos2, float ratio) const;
    
    /**
     * @brief 쿼터니언 구면 선형 보간
     */
    XMFLOAT4 InterpolateRotation(const XMFLOAT4& rot1, const XMFLOAT4& rot2, float ratio) const;
    
    /**
     * @brief 스케일 벡터 선형 보간
     */
    XMFLOAT3 InterpolateScale(const XMFLOAT3& scale1, const XMFLOAT3& scale2, float ratio) const;
};

/**
 * @brief 안전하고 현대적인 애니메이션 클래스
 * 
 * 기존 CAnimation 클래스의 모든 버그를 수정하고
 * 메모리 안전성과 성능을 개선한 새로운 애니메이션 시스템
 */
class ENGINE_DLL SafeAnimation final : public CBase
{
private:
    std::string m_name;                                          // 애니메이션 이름
    float m_duration;                                            // 총 재생 시간 (초)
    float m_ticksPerSecond;                                      // 초당 틱 수
    std::vector<std::unique_ptr<AnimationTrack>> m_tracks;       // 애니메이션 트랙들 (스마트 포인터로 메모리 안전성 보장)
    
    mutable bool m_isValidated;                                  // 유효성 검사 완료 플래그
    mutable std::string m_lastValidationError;                  // 마지막 유효성 검사 오류 메시지

private:
    SafeAnimation();
    SafeAnimation(const SafeAnimation& rhs) = delete;            // 복사 생성자 금지
    SafeAnimation& operator=(const SafeAnimation& rhs) = delete; // 복사 할당 연산자 금지
    virtual ~SafeAnimation() = default;

public: // Getter 함수들
    const std::string& GetName() const { return m_name; }
    float GetDuration() const { return m_duration; }
    float GetTicksPerSecond() const { return m_ticksPerSecond; }
    size_t GetTrackCount() const { return m_tracks.size(); }
    bool IsValid() const;
    const std::string& GetLastError() const { return m_lastValidationError; }

public: // 초기화 함수들
    /**
     * @brief Assimp 애니메이션 데이터로부터 생성
     * @param pAnimation Assimp 애니메이션 포인터
     * @param pModel 모델 포인터 (본 정보 참조용)
     * @return 성공 시 S_OK
     */
    HRESULT InitializeFromAssimp(const aiAnimation* pAnimation, class CModel* pModel);
    
    /**
     * @brief 수동으로 애니메이션 데이터 설정
     * @param name 애니메이션 이름
     * @param duration 재생 시간
     * @param ticksPerSecond 초당 틱 수
     * @return 성공 시 S_OK
     */
    HRESULT InitializeManual(const std::string& name, float duration, float ticksPerSecond);

public: // 트랙 관리
    /**
     * @brief 애니메이션 트랙 추가
     * @param boneName 본 이름
     * @param track 추가할 트랙 (unique_ptr로 소유권 이전)
     * @return 성공 시 true
     */
    bool AddTrack(const std::string& boneName, std::unique_ptr<AnimationTrack> track);
    
    /**
     * @brief 본 이름으로 트랙 찾기
     * @param boneName 본 이름
     * @return 찾은 트랙 포인터, 없으면 nullptr
     */
    AnimationTrack* FindTrack(const std::string& boneName);
    const AnimationTrack* FindTrack(const std::string& boneName) const;

public: // 애니메이션 재생
    /**
     * @brief 지정된 시간에서의 모든 본 변환 행렬 계산
     * @param time 애니메이션 시간 (초)
     * @param boneNames 본 이름 리스트
     * @param outTransforms 출력될 변환 행렬 벡터 (boneNames와 같은 순서)
     * @return 성공 시 true
     */
    bool UpdateBoneTransforms(float time, const std::vector<std::string>& boneNames, 
                             std::vector<XMMATRIX>& outTransforms) const;
    
    /**
     * @brief 지정된 시간에서의 특정 본 변환 행렬 계산
     * @param time 애니메이션 시간 (초)
     * @param boneName 본 이름
     * @param outTransform 출력될 변환 행렬
     * @return 성공 시 true
     */
    bool GetBoneTransformAt(float time, const std::string& boneName, XMMATRIX& outTransform) const;

private: // 내부 함수들
    /**
     * @brief 모든 트랙의 유효성 검사
     * @return 유효하면 true
     */
    bool ValidateAllTracks() const;
    
    /**
     * @brief 애니메이션 데이터 정규화 (시간 정렬 등)
     */
    void NormalizeAnimationData();
    
    /**
     * @brief Assimp 좌표계를 DirectX 좌표계로 변환
     */
    void ConvertCoordinateSystem();
    
    /**
     * @brief Assimp 채널을 AnimationTrack으로 변환
     * @param pChannel Assimp 채널
     * @param pTrack 출력될 트랙
     * @return 성공 시 S_OK
     */
    HRESULT ConvertAssimpChannelToTrack(const aiNodeAnim* pChannel, AnimationTrack* pTrack);

public: // 팩토리 함수
    /**
     * @brief Assimp 데이터로부터 SafeAnimation 생성
     * @param pAnimation Assimp 애니메이션
     * @param pModel 모델
     * @return 생성된 SafeAnimation 포인터
     */
    static SafeAnimation* CreateFromAssimp(const aiAnimation* pAnimation, class CModel* pModel);
    
    /**
     * @brief 빈 SafeAnimation 생성
     * @param name 애니메이션 이름
     * @param duration 재생 시간
     * @param ticksPerSecond 초당 틱 수
     * @return 생성된 SafeAnimation 포인터
     */
    static SafeAnimation* CreateEmpty(const std::string& name, float duration, float ticksPerSecond);

public: // CBase 상속
    virtual void Free() override;
};

END
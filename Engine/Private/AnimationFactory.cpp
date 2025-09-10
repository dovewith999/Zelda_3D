#include "..\Public\AnimationFactory.h"
#include "..\Public\Animation.h"
#include "..\Public\AnimationAdapter.h"
#include "..\Public\Model.h"
#include <iostream>
#include <chrono>
#include <sstream>

// 정적 멤버 초기화
AnimationFactory::SystemType AnimationFactory::s_currentSystem = AnimationFactory::SYSTEM_LEGACY;
bool AnimationFactory::s_enableSafeSystem = false;

CAnimation* AnimationFactory::CreateAnimation(aiAnimation* pAnimation, CModel* pModel)
{
    if (!pAnimation || !pModel)
        return nullptr;
    
    // 현재 설정에 따라 시스템 선택
    SystemType systemToUse = s_currentSystem;
    
    if (systemToUse == SYSTEM_AUTO)
    {
        // 자동 선택 로직: 새 시스템이 활성화되어 있으면 새 시스템 사용
        systemToUse = s_enableSafeSystem ? SYSTEM_SAFE : SYSTEM_LEGACY;
    }
    
    return CreateAnimationWithSystem(pAnimation, pModel, systemToUse);
}

CAnimation* AnimationFactory::CreateAnimationWithSystem(aiAnimation* pAnimation, CModel* pModel, SystemType systemType)
{
    if (!pAnimation || !pModel)
        return nullptr;
    
    switch (systemType)
    {
    case SYSTEM_LEGACY:
        return CreateLegacyAnimation(pAnimation, pModel);
        
    case SYSTEM_SAFE:
        return CreateSafeAnimation(pAnimation, pModel);
        
    case SYSTEM_AUTO:
        // AUTO는 이미 위에서 처리됨
        return CreateAnimation(pAnimation, pModel);
        
    default:
        // 기본값으로 레거시 시스템 사용
        return CreateLegacyAnimation(pAnimation, pModel);
    }
}

CAnimation* AnimationFactory::CloneAnimation(CAnimation* pOriginal, CModel* pModel)
{
    if (!pOriginal || !pModel)
        return nullptr;
    
    // 원본 애니메이션이 어떤 시스템으로 만들어졌는지 확인
    if (IsUsingSafeSystem(pOriginal))
    {
        // SafeCAnimation으로 캐스팅하여 복제
        SafeCAnimation* pSafeOriginal = CastToSafeAnimation(pOriginal);
        if (pSafeOriginal)
        {
            return pSafeOriginal->Clone_Animation(pModel);
        }
    }
    
    // 레거시 시스템으로 복제
    return pOriginal->Clone_Animation(pModel);
}

void AnimationFactory::SetAnimationSystem(SystemType systemType)
{
    if (s_currentSystem != systemType)
    {
        LogSystemTransition(s_currentSystem, systemType);
        s_currentSystem = systemType;
    }
}

void AnimationFactory::EnableSafeSystem(bool enable)
{
    if (s_enableSafeSystem != enable)
    {
        s_enableSafeSystem = enable;
        std::cout << "[AnimationFactory] Safe system " 
                  << (enable ? "enabled" : "disabled") << std::endl;
    }
}

bool AnimationFactory::IsUsingSafeSystem(CAnimation* pAnimation)
{
    if (!pAnimation)
        return false;
    
    // SafeCAnimation으로 dynamic_cast 시도
    SafeCAnimation* pSafeAnim = dynamic_cast<SafeCAnimation*>(pAnimation);
    return (pSafeAnim != nullptr);
}

SafeCAnimation* AnimationFactory::CastToSafeAnimation(CAnimation* pAnimation)
{
    if (!pAnimation)
        return nullptr;
    
    return dynamic_cast<SafeCAnimation*>(pAnimation);
}

std::string AnimationFactory::RunPerformanceComparison()
{
    std::stringstream result;
    result << "=== Animation System Performance Comparison ===" << std::endl;
    
    // 임시 테스트 데이터 (실제로는 유효한 aiAnimation과 CModel이 필요)
    result << "Note: Performance comparison requires valid Assimp data" << std::endl;
    result << "Current System: ";
    
    switch (s_currentSystem)
    {
    case SYSTEM_LEGACY:
        result << "Legacy CAnimation" << std::endl;
        break;
    case SYSTEM_SAFE:
        result << "Safe SafeCAnimation" << std::endl;
        break;
    case SYSTEM_AUTO:
        result << "Auto Selection" << std::endl;
        break;
    }
    
    result << "Safe System Enabled: " << (s_enableSafeSystem ? "Yes" : "No") << std::endl;
    
    return result.str();
}

std::string AnimationFactory::GetSystemInfo()
{
    std::stringstream info;
    info << "Animation Factory System Info:" << std::endl;
    info << "  Current System: ";
    
    switch (s_currentSystem)
    {
    case SYSTEM_LEGACY:
        info << "Legacy (Original CAnimation)";
        break;
    case SYSTEM_SAFE:
        info << "Safe (New SafeCAnimation)";
        break;
    case SYSTEM_AUTO:
        info << "Auto Selection";
        break;
    }
    info << std::endl;
    
    info << "  Safe System Available: " << (s_enableSafeSystem ? "Yes" : "No") << std::endl;
    info << "  Default Fallback: Legacy System" << std::endl;
    
    return info.str();
}

std::string AnimationFactory::GetAnimationStats()
{
    // TODO: 실제 통계 수집 구현
    return "Animation statistics collection not yet implemented";
}

void AnimationFactory::LogSystemTransition(SystemType from, SystemType to)
{
    std::cout << "[AnimationFactory] System transition: ";
    
    auto systemName = [](SystemType type) -> std::string {
        switch (type)
        {
        case SYSTEM_LEGACY: return "Legacy";
        case SYSTEM_SAFE: return "Safe";
        case SYSTEM_AUTO: return "Auto";
        default: return "Unknown";
        }
    };
    
    std::cout << systemName(from) << " -> " << systemName(to) << std::endl;
}

CAnimation* AnimationFactory::CreateLegacyAnimation(aiAnimation* pAnimation, CModel* pModel)
{
    // 기존 CAnimation::Create 호출
    return CAnimation::Create(pAnimation, pModel);
}

CAnimation* AnimationFactory::CreateSafeAnimation(aiAnimation* pAnimation, CModel* pModel)
{
    // SafeCAnimation 생성하여 CAnimation*로 반환
    // SafeCAnimation은 CBase를 상속받으므로 CAnimation*로 캐스팅 가능
    SafeCAnimation* pSafeAnim = SafeCAnimation::Create(pAnimation, pModel);
    
    // SafeCAnimation을 CAnimation으로 업캐스팅
    return static_cast<CAnimation*>(pSafeAnim);
}

bool AnimationFactory::CheckSystemCompatibility()
{
    // TODO: 시스템 호환성 검사 구현
    return true;
}

// AnimationSystemScope 구현
AnimationSystemScope::AnimationSystemScope(AnimationFactory::SystemType tempSystem)
    : m_previousSystem(AnimationFactory::GetCurrentSystem())
{
    AnimationFactory::SetAnimationSystem(tempSystem);
}

AnimationSystemScope::~AnimationSystemScope()
{
    AnimationFactory::SetAnimationSystem(m_previousSystem);
}
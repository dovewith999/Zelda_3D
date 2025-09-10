# CLAUDE.md

이 파일은 DirectX 11와 C++로 만드는 3D 게임 프로젝트에서 AI 코딩 에이전트(Claude Code)가 코드 작업을 수행할 때 따라야 할 가이드라인을 제공합니다.

## 프로젝트 개요

- **그래픽스 API**: DirectX 11
- **플랫폼**: Windows PC 3D 게임
- **언어**: C++17 이상
- **아키텍처**: 컴포넌트 기반 엔티티 시스템

## 개발 환경 및 빌드

### 주요 서드파티 의존성
- **DirectX SDK**: Windows SDK 포함 DirectX 11 라이브러리
- **DirectXMath**: 수학 연산 라이브러리
- **Assimp**: 3D 모델 로딩 라이브러리 (.fbx, .obj 등)
- **FMOD**: 오디오 시스템
- **Dear ImGui**: 디버깅 및 툴 UI (선택사항)

### 권장 IDE 및 도구
- **Visual Studio 2022** (최신 버전)
- **RenderDoc**: 그래픽스 디버깅
- **PIX for Windows**: DirectX 성능 분석

# AI 코딩 에이전트 행동 강령

## 기본 원칙

### 1. 소통 우선 원칙
- 요구사항의 불분명한 부분은 **반드시 사전 확인**
- 추측 기반 구현 **절대 금지**
- 구현 전 세부 계획을 사용자와 **합의 후 진행**

### 2. 반복적 개발
- 기능을 세분화하여 **점진적 구현**
- 각 반복마다 **결과물 공유 및 피드백** 수렴
- **유연한 방향 수정** 가능한 개발 프로세스

### 3. 공식 문서 참조
- DirectX 11, HLSL의 **Microsoft 공식 문서 우선 참조**
- 불확실한 내용은 공식 문서로 검증

## 필수 준수 규칙

### 컴파일 실패 대응
**연속 2회 이상 컴파일 실패 시 반드시 사용자와 상의**

상의 시 포함 사항:
- 현재 컴파일 오류 상태
- 수정 시도 이력 및 영향 범위
- 향후 접근 방법 구상

### 리소스 관리
- **모든 DirectX 리소스는 RAII 패턴 적용**
- 메모리 누수 방지를 위한 스마트 포인터 활용 필수

---

# 코딩 가이드라인

## 핵심 원칙

1. **RAII (Resource Acquisition Is Initialization)** - 자원 관리
2. **SOLID 원칙** 준수
3. **Modern C++ (C++17/20)** 기능 적극 활용
4. **DirectX 11 베스트 프랙티스** 준수

## 명명 규칙

### 클래스 및 구조체
```cpp
// ✅ 좋은 예
class GraphicsEngine          // 클래스: PascalCase
{
public:
    struct VertexData        // 내부 구조체: PascalCase
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Normal;
        DirectX::XMFLOAT2 TexCoord;
    };
    
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;           // 멤버: m_ 접두사
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    
    static constexpr uint32_t MAX_TEXTURES = 16;             // 상수: UPPER_SNAKE_CASE
};

// 인터페이스
class IRenderable            // 인터페이스: I 접두사
{
public:
    virtual ~IRenderable() = default;
    virtual void Render(const RenderContext& context) = 0;
};
```

### 함수 명명
```cpp
class Mesh
{
public:
    bool Initialize(ID3D11Device* device, const VertexData* vertices, size_t vertexCount);
    void Render(ID3D11DeviceContext* context) const;
    
    // Getter/Setter
    uint32_t GetVertexCount() const { return m_vertexCount; }
    void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { m_topology = topology; }
    
    // Bool 함수
    bool IsValid() const { return m_vertexBuffer != nullptr; }
    bool HasIndexBuffer() const { return m_indexBuffer != nullptr; }
    
private:
    void CreateVertexBuffer(ID3D11Device* device, const VertexData* vertices, size_t count);
    void CreateIndexBuffer(ID3D11Device* device, const uint32_t* indices, size_t count);
};
```

### 열거형
```cpp
// ✅ 강타입 열거형 사용
enum class ShaderType : uint8_t
{
    Vertex = 0,
    Pixel,
    Geometry,
    Compute,
    Hull,
    Domain,
    Count
};

enum class RenderPass : uint32_t
{
    Depth        = 1 << 0,
    Opaque       = 1 << 1,
    Transparent  = 1 << 2,
    UI           = 1 << 3,
    PostProcess  = 1 << 4
};
```

## DirectX 11 특화 가이드라인

### 리소스 관리
```cpp
// ✅ ComPtr 사용 (RAII)
class Texture2D
{
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    
public:
    bool Create(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format)
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        
        HRESULT hr = device->CreateTexture2D(&desc, nullptr, m_texture.GetAddressOf());
        if (FAILED(hr))
        {
            return false;
        }
        
        // SRV 생성
        hr = device->CreateShaderResourceView(m_texture.Get(), nullptr, m_srv.GetAddressOf());
        return SUCCEEDED(hr);
    }
};
```

### 셰이더 관리
```cpp
// ✅ 셰이더 래퍼 클래스
class Shader
{
private:
    Microsoft::WRL::ComPtr<ID3DBlob> m_bytecode;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    
public:
    bool CompileFromFile(ID3D11Device* device, const std::wstring& filePath, 
                        const std::string& entryPoint, const std::string& profile);
    
    void Bind(ID3D11DeviceContext* context) const
    {
        if (m_vertexShader)
        {
            context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
            context->IASetInputLayout(m_inputLayout.Get());
        }
        
        if (m_pixelShader)
        {
            context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
        }
    }
};
```

### 상수 버퍼 패턴
```cpp
// ✅ 상수 버퍼 구조체 (16바이트 정렬)
struct alignas(16) CameraConstants
{
    DirectX::XMMATRIX ViewMatrix;
    DirectX::XMMATRIX ProjectionMatrix;
    DirectX::XMMATRIX ViewProjectionMatrix;
    DirectX::XMFLOAT4 CameraPosition;
    DirectX::XMFLOAT4 CameraDirection;
};

class ConstantBuffer
{
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    uint32_t m_size;
    
public:
    template<typename T>
    bool Create(ID3D11Device* device)
    {
        static_assert(sizeof(T) % 16 == 0, "Constant buffer size must be multiple of 16 bytes");
        
        m_size = sizeof(T);
        
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = m_size;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        return SUCCEEDED(device->CreateBuffer(&desc, nullptr, m_buffer.GetAddressOf()));
    }
    
    template<typename T>
    void Update(ID3D11DeviceContext* context, const T& data)
    {
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        if (SUCCEEDED(context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        {
            memcpy(mapped.pData, &data, sizeof(T));
            context->Unmap(m_buffer.Get(), 0);
        }
    }
};
```

## 에러 처리

### HRESULT 처리 매크로
```cpp
// ✅ 에러 처리 매크로
#define DX_SAFE_CALL(call) \
    do { \
        HRESULT hr = (call); \
        if (FAILED(hr)) { \
            OutputDebugStringA("DirectX call failed: " #call "\n"); \
            return false; \
        } \
    } while(0)

// 사용 예
bool CreateRenderTarget(ID3D11Device* device, uint32_t width, uint32_t height)
{
    D3D11_TEXTURE2D_DESC desc = {};
    // ... desc 설정 ...
    
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    DX_SAFE_CALL(device->CreateTexture2D(&desc, nullptr, texture.GetAddressOf()));
    
    return true;
}
```

## 성능 최적화 가이드라인

### 렌더링 최적화
```cpp
// ✅ 드로우 콜 배치 (Draw Call Batching)
class RenderQueue
{
private:
    struct RenderItem
    {
        Mesh* mesh;
        Material* material;
        DirectX::XMMATRIX worldMatrix;
        float distance; // 정렬용
    };
    
    std::vector<RenderItem> m_opaqueItems;
    std::vector<RenderItem> m_transparentItems;
    
public:
    void Submit(Mesh* mesh, Material* material, const DirectX::XMMATRIX& world, 
               const DirectX::XMFLOAT3& cameraPos)
    {
        RenderItem item;
        item.mesh = mesh;
        item.material = material;
        item.worldMatrix = world;
        
        // 거리 계산 (투명 오브젝트 정렬용)
        DirectX::XMVECTOR worldPos = DirectX::XMVector3Transform(DirectX::XMVectorZero(), world);
        DirectX::XMVECTOR camPos = DirectX::XMLoadFloat3(&cameraPos);
        item.distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(worldPos - camPos));
        
        if (material->IsTransparent())
        {
            m_transparentItems.push_back(item);
        }
        else
        {
            m_opaqueItems.push_back(item);
        }
    }
    
    void Render(ID3D11DeviceContext* context)
    {
        // 불투명 오브젝트: 앞에서 뒤로 (Z-buffer 효율성)
        std::sort(m_opaqueItems.begin(), m_opaqueItems.end(), 
                  [](const RenderItem& a, const RenderItem& b) {
                      return a.distance < b.distance;
                  });
        
        // 투명 오브젝트: 뒤에서 앞으로 (블렌딩)
        std::sort(m_transparentItems.begin(), m_transparentItems.end(),
                  [](const RenderItem& a, const RenderItem& b) {
                      return a.distance > b.distance;
                  });
        
        // 렌더링...
    }
};
```

## 문서화 및 주석

### 헤더 파일 문서화
```cpp
/**
 * @brief DirectX 11 기반 3D 그래픽스 엔진의 핵심 렌더러 클래스
 * 
 * 이 클래스는 DirectX 11을 사용하여 3D 장면을 렌더링하는 기능을 제공합니다.
 * 메시, 텍스처, 셰이더 등의 리소스 관리와 렌더링 파이프라인을 담당합니다.
 */
class Renderer
{
public:
    /**
     * @brief 렌더러를 초기화합니다.
     * @param hwnd 렌더링 대상 윈도우 핸들
     * @param width 백버퍼 너비
     * @param height 백버퍼 높이
     * @param enableVSync 수직동기화 활성화 여부
     * @return 초기화 성공 시 true, 실패 시 false
     */
    bool Initialize(HWND hwnd, uint32_t width, uint32_t height, bool enableVSync = false);
    
    /**
     * @brief 프레임 렌더링을 시작합니다.
     * @param clearColor 백버퍼 클리어 색상
     */
    void BeginFrame(const DirectX::XMFLOAT4& clearColor = {0.0f, 0.0f, 0.0f, 1.0f});
    
    /**
     * @brief 프레임 렌더링을 종료하고 화면에 출력합니다.
     */
    void EndFrame();
};
```

### 코드 내 주석 원칙
```cpp
void Renderer::Render(const Scene& scene)
{
    // 깊이 버퍼 클리어 및 렌더 타겟 설정
    // Note: 매 프레임마다 깊이 버퍼를 클리어해야 Z-fighting 방지
    m_context->ClearDepthStencilView(m_depthStencilView.Get(), 
                                     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
                                     1.0f, 0);
    
    // 뷰포트 설정 - 백버퍼 전체 영역 사용
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_backBufferWidth);
    viewport.Height = static_cast<float>(m_backBufferHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &viewport);
    
    // 렌더 큐에 오브젝트 제출
    // 이유: 머티리얼별 배치를 통해 상태 변경 최소화
    for (const auto& object : scene.GetRenderables())
    {
        m_renderQueue.Submit(object.mesh, object.material, 
                           object.transform, scene.GetCamera().GetPosition());
    }
    
    m_renderQueue.Render(m_context.Get());
}
```

**주석 작성 가이드:**
- DirectX 상태 변경의 **이유와 목적** 설명
- 성능에 영향을 주는 부분은 **최적화 의도** 명시
- 셰이더나 렌더링 기법의 **수학적 배경** 간단히 설명
- **한국어 주석** 허용 (도메인 로직 관련)

## Assimp 애니메이션 시스템 가이드라인

### 핵심 집중 영역
Claude Code가 Assimp 애니메이션 관련 작업을 수행할 때 반드시 다음 영역에 집중해야 합니다:

#### 1. 애니메이션 보간 (Animation Interpolation)
```cpp
// ✅ 필수 구현 요소들
class BoneAnimationChannel
{
private:
    std::vector<AnimationKey> m_positionKeys;    // 위치 키프레임
    std::vector<AnimationKey> m_rotationKeys;    // 회전 키프레임 (쿼터니언)
    std::vector<AnimationKey> m_scaleKeys;       // 스케일 키프레임
    
public:
    // 선형 보간 (Linear Interpolation)
    XMFLOAT3 InterpolatePosition(float time) const;
    
    // 구면 선형 보간 (Spherical Linear Interpolation) - 회전용
    XMFLOAT4 InterpolateRotation(float time) const;
    
    // 스케일 보간
    XMFLOAT3 InterpolateScale(float time) const;
    
    // 전체 변환 행렬 계산
    XMMATRIX GetInterpolatedTransform(float time) const;
};

// 보간 알고리즘 세부 구현 가이드
// - Position/Scale: 선형 보간 (LERP) 사용
// - Rotation: 구면 선형 보간 (SLERP) 사용 필수
// - 키프레임 검색: 이진 탐색으로 최적화
// - 시간 범위 초과 시 클램핑 또는 루핑 처리
```

#### 2. 루트 애니메이션 (Root Motion) 처리
```cpp
// ✅ 루트 애니메이션 전용 시스템
class RootMotionExtractor
{
private:
    std::string m_rootBoneName;              // 보통 "Root", "Hips", "Pelvis" 등
    std::vector<XMFLOAT3> m_rootPositions;   // 루트 본의 위치 키프레임
    std::vector<XMFLOAT4> m_rootRotations;   // 루트 본의 회전 키프레임
    
public:
    // 프레임간 루트 모션 델타 계산
    XMFLOAT3 GetRootMotionDelta(float prevTime, float currentTime) const;
    
    // 루트 회전 델타 계산 (캐릭터 방향 변경용)
    XMFLOAT4 GetRootRotationDelta(float prevTime, float currentTime) const;
    
    // 루트 모션 적용 후 애니메이션에서 제거된 변환 반환
    XMMATRIX GetRootMotionRemovedTransform(float time) const;
};

// 루트 모션 처리 규칙:
// 1. 루트 본의 이동량을 캐릭터 월드 이동에 적용
// 2. 애니메이션 자체에서는 루트 본의 이동 제거 (제자리 애니메이션)
// 3. Y축 회전만 캐릭터 방향에 적용 (X, Z축 회전은 애니메이션 유지)
// 4. 루핑 애니메이션에서는 시작/끝 프레임의 연속성 보장
```

#### 3. Assimp 데이터 변환 최적화
```cpp
// ✅ Assimp → DirectX 변환 최적화 패턴
class AssimpAnimationLoader
{
public:
    // aiScene에서 애니메이션 데이터 추출
    bool LoadAnimation(const aiScene* scene, const std::string& animationName);
    
private:
    // Assimp 좌표계 → DirectX 좌표계 변환
    XMFLOAT3 ConvertPosition(const aiVector3D& pos) const
    {
        // Assimp: 오른손 좌표계 → DirectX: 왼손 좌표계
        return XMFLOAT3(pos.x, pos.y, -pos.z);  // Z축 반전
    }
    
    XMFLOAT4 ConvertRotation(const aiQuaternion& rot) const
    {
        // 쿼터니언 변환 (Z축 반전 고려)
        return XMFLOAT4(-rot.x, -rot.y, rot.z, rot.w);
    }
    
    // 키프레임 시간 정규화 (틱 → 초 단위)
    float ConvertTime(double ticks, double ticksPerSecond) const
    {
        return static_cast<float>(ticks / ticksPerSecond);
    }
    
    // 루트 본 자동 감지
    std::string DetectRootBone(const aiScene* scene) const;
};
```

### 구현 우선순위 및 주의사항

#### 필수 구현 요소 (높은 우선순위)
1. **보간 알고리즘 정확성**
   - 회전은 반드시 SLERP 사용 (LERP 사용 시 비자연스러운 애니메이션)
   - 키프레임 검색 최적화 (이진 탐색)
   - 경계 케이스 처리 (시간 범위 초과, 키프레임 부족 등)

2. **루트 모션 정확성**
   - 델타 계산 정밀도 (부동소수점 오차 최소화)
   - 루핑 애니메이션의 연속성 보장
   - 애니메이션 블렌딩 시 루트 모션 혼합

3. **좌표계 변환**
   - Assimp (오른손) → DirectX (왼손) 좌표계 변환
   - 회전 축 및 방향 일관성 유지

#### 성능 최적화 고려사항
```cpp
// ✅ 성능 최적화 패턴
class AnimationCache
{
private:
    // 자주 사용되는 보간 결과 캐싱
    mutable std::unordered_map<uint32_t, XMMATRIX> m_transformCache;
    mutable float m_lastCacheTime = -1.0f;
    
public:
    // 프레임당 한 번만 계산, 이후 캐시 사용
    XMMATRIX GetCachedTransform(uint32_t boneIndex, float time) const;
    
    // 캐시 무효화 (애니메이션 변경 시)
    void InvalidateCache() { m_transformCache.clear(); m_lastCacheTime = -1.0f; }
};

// SIMD 최적화 활용
void InterpolateBonesBatch(const std::vector<BoneAnimationChannel>& channels,
                          float time, std::vector<XMMATRIX>& outTransforms);
```

### 디버깅 및 검증 도구

#### 애니메이션 디버그 정보 출력
```cpp
// ✅ 디버깅용 함수들 (개발 단계에서 필수)
class AnimationDebugger
{
public:
    // 루트 모션 궤적 시각화
    void VisualizeRootMotionPath(const RootMotionExtractor& extractor, 
                                float duration, float step = 0.1f);
    
    // 본 변환 행렬 검증
    bool ValidateBoneTransforms(const std::vector<XMMATRIX>& transforms);
    
    // 애니메이션 키프레임 정보 출력
    void DumpAnimationInfo(const BoneAnimationChannel& channel);
    
    // 보간 품질 검사 (급격한 변화 감지)
    void CheckInterpolationQuality(const BoneAnimationChannel& channel, 
                                  float timeStep = 1.0f/60.0f);
};
```

### 에러 처리 및 폴백

#### 견고한 애니메이션 시스템 구축
```cpp
// ✅ 에러 상황 대응
class SafeAnimationPlayer
{
private:
    // 기본 T-Pose 또는 Bind Pose
    std::vector<XMMATRIX> m_bindPose;
    
public:
    // 애니메이션 실패 시 안전한 폴백
    void PlayAnimationSafe(const std::string& animName, float time)
    {
        auto* anim = GetAnimation(animName);
        if (!anim || !anim->IsValid())
        {
            // 폴백: Bind Pose 사용
            ApplyBindPose();
            LogWarning("Animation fallback to bind pose: " + animName);
            return;
        }
        
        // 정상 애니메이션 재생
        anim->Update(time);
    }
    
    // 루트 모션 안전성 검사
    XMFLOAT3 GetSafeRootMotion(float deltaTime)
    {
        auto motion = CalculateRootMotion(deltaTime);
        
        // 비정상적인 이동량 감지 (순간이동 방지)
        float motionMagnitude = XMVectorGetX(XMVector3Length(XMLoadFloat3(&motion)));
        const float MAX_MOTION_PER_FRAME = 10.0f; // 프레임당 최대 10미터
        
        if (motionMagnitude > MAX_MOTION_PER_FRAME)
        {
            LogWarning("Excessive root motion detected: " + std::to_string(motionMagnitude));
            return XMFLOAT3(0, 0, 0); // 안전한 값으로 클램핑
        }
        
        return motion;
    }
};
```

### Claude Code 작업 지침

애니메이션 관련 작업 시 다음 순서로 진행:

1. **요구사항 명확화**
   - 애니메이션 타입 확인 (루핑/원샷)
   - 루트 모션 필요 여부 확인
   - 블렌딩 요구사항 파악

2. **보간 알고리즘 우선 구현**
   - 기본 LERP/SLERP 구현
   - 키프레임 검색 최적화
   - 경계 케이스 테스트

3. **루트 모션 시스템 구축**
   - 루트 본 감지 및 추출
   - 델타 계산 로직
   - 애니메이션 정규화

4. **통합 테스트 및 최적화**
   - 성능 프로파일링
   - 메모리 사용량 최적화
   - 디버그 도구 활용

## 개발 가이드라인 요약

- **리소스 누수 방지**: 모든 DirectX 리소스는 ComPtr 사용
- **에러 처리**: HRESULT 반환값 반드시 확인
- **성능 고려**: 렌더 상태 변경 최소화, 드로우 콜 배치
- **현대 C++ 활용**: auto, range-based for, 스마트 포인터 적극 사용
- **플랫폼 특화**: Windows 전용 기능 활용 가능
- **애니메이션 정확성**: 보간 알고리즘 정밀도 및 루트 모션 처리 신뢰성 최우선
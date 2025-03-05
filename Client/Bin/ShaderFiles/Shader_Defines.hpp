// 모든 셰이더 파일들이 가질 hpp 파일 
// cpp와 h와 비슷한 관계이다. cpp = hlsl, h = hpp
// 보통 전역변수나 sampler, RenderState를 나타내는 함수들을 작성한다.

sampler		DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler		PointSampler = sampler_state
{
	filter = min_mag_mip_point;
	AddressU = wrap;
	AddressV = wrap;
};

RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
	// 프레임을 채워서(solid) 그려라, 후면 컬링 처리(back), 반시계 방향으로 그린다.
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
	// 프레임만 그리고 그 사이는 비워서 (wireframe) 그려라
};

RasterizerState RS_Sky
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;

	// Skybox 용도의 함수이다.
	// 스카이박스는 카메라가 스카이 박스 안에서 스카이 박스의 면의 안쪽을 비추고 있는 구조이기 때문에 정점의 컬링을 반대로 해주지 않으면 뒷면이라고 인식해서 그리지 않는다.
};

RasterizerState RS_NonCulling
{
	FillMode = solid;
	CullMode = none;
	FrontCounterClockwise = false;
};

DepthStencilState	DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
	// Depth버퍼와 스텐실버퍼 이용 여부이다. 스텐실 버퍼는 옵션들의 Defalut가 모두 false이기 때문에 따로 지정안해줬다.
	// 깊이테스트를 사용하겠다(true), 랜더타겟의 모든 영역에 깊이값을 적어라(all), 깊이 값이 적거나 같은 것(less_equal)을.
};

DepthStencilState	DSS_None_ZTest_And_Write
{
	DepthEnable = false;
	DepthWriteMask = zero;
	// 깊이 테스트를 끈다. 아무 영역에도 깊이값을 저장시키지 않겠다.
};

BlendState BS_None
{
	BlendEnable[0] = false;
	// 알파블렌딩 옵션을 끈다.
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	SrcBlend[0] = SRC_ALPHA;
	DestBlend[0] = INV_SRC_ALPHA;
	BlendOp[0] = Add;
	// 내가 그릴 색(SrcBlend)와 랜더 타겟에 이미 그려져있는 색(DestBlend)를 섞는다(Add)
	// [0]은 옵션을 적용시킬 랜더 타겟을 가리킨다. 0번째 랜더 타겟에 적용시키겠다는 뜻
	//[]없이 적어도 되지만 그러면 생성된 모든 랜더 타겟에 옵션을 적용시킨다는 뜻이다.
};

BlendState BS_One
{
	BlendEnable[0] = true;
	SrcBlend[0] = ONE;
	DestBlend[0] = ONE;
	BlendOp[0] = Add;
	// 알파를 안쓰고 내가 그리려는 색과 이미 그려져있는 색을 그대로 더하겠다.
};

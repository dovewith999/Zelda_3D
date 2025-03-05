
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ViewMatrixInv, g_ProjMatrixInv;

texture2D		g_Texture;
texture2D		g_NormalTexture;
texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_DepthTexture;
texture2D		g_SpecularTexture;
texture2D		g_LimLightTexture;

vector			g_vCamPosition;
float			g_fCamFar;

// 빛 연산을 하기 위한 빛 정보
float4			g_vLightDir;
float4			g_vLightPos;
float			g_fRange;
float4			g_vLightDiffuse;
float4			g_vLightAmbient;
float4			g_vLightSpecular;

float4			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
float4			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;

};

struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float fViewZ = vDepthDesc.y * g_fCamFar;

	// 보정해놓은 노말값을 다시 원상태로 보정

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	/*, 투영스페이스상의 위치. */
	/* 월드위치 * 뷰행렬 * 투영행렬 / w */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * fViewZ/* 뷰스페이스 상의 z */;

	/* 월드위치 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	
	/* 월드 위치 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	Out.vShade.xyz = saturate((g_vLightDiffuse * saturate(dot(normalize(g_vLightDir) * -1.f, vNormal))) + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector		vLook = vWorldPos - g_vCamPosition;

	float		fSpecular = saturate(dot(normalize(vReflect) * -1.f, normalize(vLook)));

	Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);
	Out.vSpecular.a = 0.f;

	return Out;
}

// 최종 완료된 디퓨즈 타겟과 셰이드 타겟을 곱해서 백버퍼에 그려주기 위한 함수
PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vLimLight = g_LimLightTexture.Sample(DefaultSampler, In.vTexUV);

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * g_fCamFar;


	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	/*, 투영스페이스상의 위치. */
	/* 월드위치 * 뷰행렬 * 투영행렬 / w */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * fViewZ/* 뷰스페이스 상의 z */;

	/* 월드위치 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드 위치 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	float fRimWidth = 0.5f;

	vector vCameraDir = normalize(g_vCamPosition - vWorldPos);

	float fRim = smoothstep(1.f -fRimWidth, 0.9f, 1 - max(0, dot(vNormal, vCameraDir)));

	Out.vColor = (vDiffuse) * (ceil(vShade * 2) / 2.f) /*+ vSpecular*/;

	//Out.vColor.xyz -= vLimLight.xyz;
	//Out.vColor.a = 1.f;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

DepthStencilState	DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState	DSS_None_ZTest_And_Write
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

BlendState BS_None
{
	BlendEnable[0] = false;
};


technique11 DefaultTechinque
{
	pass Debug
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Directional
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Blending
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

}


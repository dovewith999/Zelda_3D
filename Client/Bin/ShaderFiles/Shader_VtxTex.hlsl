#include "Shader_Defines.hpp"

// float4x4
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// UI_Title 
float				g_fBarFront = 0.f;
float				g_fBarBack = 0.f;

// UI_PressStart
float				g_fBlend = 0.f;

float				g_fAlpha = 0.f;

texture2D		g_DiffuseTexture;



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

VS_OUT VS_MAIN_RECT(VS_IN In)
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

PS_OUT PS_MAIN_RECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;	

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);	

	if (Out.vColor.a < 0.1f)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_MAIN_UI_TITLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (g_fBarFront >= In.vTexUV.x && g_fBarBack <= In.vTexUV.x)
		Out.vColor.rgb = 255.f;

	if (Out.vColor.a < 0.1f)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_MAIN_UI_PressStart(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor.xyz = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV).xyz;

	Out.vColor.a = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV).a * g_fBlend;

	if (Out.vColor.a < 0.1f)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_MAIN_UI_BLACKOUT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor.xyz = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV).xyz;

	Out.vColor.a = g_fAlpha;

	if (Out.vColor.a < 0.1f)
	{
		discard;
	}

	return Out;
}


technique11 DefaultTechinque
{
	pass Rect
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RECT();
	}	
	
	pass UI_TITLE
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UI_TITLE();
	}

	pass UI_PRESSSTART
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UI_PressStart();
	}

	pass UI_BLACKOUT
	{
		SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_RECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UI_BLACKOUT();
	}
}


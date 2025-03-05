#include "Shader_Defines.hpp"

// float4x4
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_SocketMatrix;
texture2D		g_DiffuseTexture;

vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector			g_vLightPos;
float			g_fRange = 0.f;

vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightSpecular = vector(0.15f, 0.15f, 0.15f, 1.f);

vector			g_vMtrlDiffuse;
vector			g_vMtrlAmbient = vector(0.2f, 0.2f, 0.2f, 1.f);
vector			g_vMtrlSpecular = vector(0.15f, 0.15f, 0.15f, 1.f);

vector			g_vCamPosition;



struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix).xyz;
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = vPosition;

	return Out;
}

VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix	matVP;


	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_SocketMatrix);
	vPosition = mul(vPosition, matVP);

	Out.vPosition = vPosition;
	Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix).xyz;
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal  : SV_TARGET1;
	float4		vDepth	 : SV_TARGET2;
	float4		vLimLight : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;	

	//vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	//float		fShade = max(dot(normalize(g_vLightDir.xyz) * -1.f, In.vNormal), 0.f);

	//vector		vLook = In.vWorldPos - g_vCamPosition;
	//vector		vReflect = reflect(normalize(g_vLightDir), vector(In.vNormal, 0.f));

	//float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);

	//Out.vColor.xyz = ((g_vLightDiffuse * vMtrlDiffuse) * (ceil((fShade + g_vLightAmbient * g_vMtrlAmbient) * 3) / 3)
	//	+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular).xyz;

	//Out.vColor.a = vMtrlDiffuse.a;

	//if (Out.vColor.a < 0.1f)
	//{
	//	discard;
	//}
	float fRimWidth = 0.5f;

	vector vCameraDir = normalize(g_vCamPosition - In.vWorldPos);

	float fRim = smoothstep(1.f - fRimWidth, 0.9f, 1 - max(0, dot(In.vNormal, vCameraDir)));

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);

	//Out.vLimLight = vector(-2.f, -2.f, -2.f, 0.f);

	if (Out.vDiffuse.a < 0.1f)
		discard;


	return Out;
}

PS_OUT PS_MAIN_BOMB(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float fRimWidth = 0.5f;

	vector vCameraDir = normalize(g_vCamPosition - In.vWorldPos);

	float fRim = smoothstep(1.f - fRimWidth, 0.9f, 1 - max(0, dot(In.vNormal, vCameraDir)));

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDiffuse.x = 1.f;
	Out.vDiffuse.y = 0.4f;
	Out.vDiffuse.z = 0.4f;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);


	if (Out.vDiffuse.a < 0.1f)
		discard;


	return Out;
}


technique11 DefaultTechinque
{
	pass Default
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	

	pass Socket
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass NonCulling
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Bomb
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BOMB();
	}
	
}


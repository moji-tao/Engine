#include "Common.hlsl"

Texture2D EquirectangularMap : register(t0);

cbuffer cbEnvironmentPass
{
	float4x4 gEnvironmentView;
	float4x4 gEnvironmentProj;
	float Roughness;
}

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float3 BitangentL : BITANGENT;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosL		: POSITION;
};

VertexOut VS(VertexIn vertexIn)
{
    VertexOut vertexOut;

    vertexOut.PosL = vertexIn.PosL;

	float4x4 View = gEnvironmentView;
	View[0][3] = View[1][3] = View[2][3] = 0.0f;
	
	vertexOut.PosH = mul(gEnvironmentProj, mul(View, float4(vertexIn.PosL, 1.0f)));

    return vertexOut;
}

float2 SampleSphericalMap(float3 v)
{
	const float2 InvAtan = float2(0.1591, 0.3183);
	
    float2 UV = float2(atan2(v.z, v.x), -asin(v.y));
    UV *= InvAtan;
    UV += 0.5;
    return UV;
}

float4 PS(VertexOut pixelIn) : SV_TARGET
{
	float2 UV = SampleSphericalMap(normalize(pixelIn.PosL));
	
	float3 Color = EquirectangularMap.SampleLevel(LinearClampSampler, UV, 0).rgb;

	return float4(Color, 1.0f);
}
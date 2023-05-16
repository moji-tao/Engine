#include "Common.hlsl"

Texture2D ColorBuffer;
Texture2D NormalBuffer;
Texture2D DepthBuffer;
Texture2D MetallicRoughnessGBuffer;

struct VertexIn
{
    float3 PosL : POSITION;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float2 TexC  : TEXCOORD;
};

VertexOut VS(VertexIn vertexIn)
{
    VertexOut vertexOut;

    vertexOut.PosH = float4(vertexIn.PosL, 1.0f);
    vertexOut.TexC = vertexIn.TexC;

    return vertexOut;
}

bool TraceRay(float3 rayStart, float3 rayDir, out float2 outUV)
{
    for(int i = 0; i < 10; ++i)
    {
        
    }

    return false;
}

float4 PS(VertexOut pixelIn) : SV_TARGET
{
    float3 normal = NormalBuffer.SampleLevel(PointClampSampler, pixelIn.TexC, 0).xyz;
    float3 normalV = mul((float3x3)gView, normal);
    normalV = normalize(normalV);
    float depth = DepthBuffer.SampleLevel(PointClampSampler, pixelIn.TexC, 0).r;
    float3 posV = UVToView(pixelIn.TexC, depth, gInvProj).xyz;

    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

    return color;
}
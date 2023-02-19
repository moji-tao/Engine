#include "Common.hlsl"

cbuffer cbLightView
{
    float4x4 gLightViewProj;
}

StructuredBuffer<InstanceData> gInstanceData : register(t0);

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
	float4 PosH    : SV_POSITION;
};

VertexOut VS(VertexIn vertexIn, uint instanceID : SV_InstanceID)
{
    VertexOut vertexOut;
    float4 PosW = mul(gInstanceData[instanceID].gWorld, float4(vertexIn.PosL, 1.0f));
    vertexOut.PosH = mul(gLightViewProj, PosW);

    return vertexOut;
}

void PS(VertexOut pixelIn)
{

}
#include "Common.hlsl"

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

    vertexOut.PosH = mul(gJitterViewProj, mul(gInstanceData[instanceID].gWorld, float4(vertexIn.PosL, 1.0f)));

    return vertexOut;
}

void PS(VertexOut pixelIn)
{

}
#include "Common.hlsl"

cbuffer cbLight
{
    float4x4 lightView;
    float4x4 lightProj;
    float3 lightPosition;
    float lightRange;
    uint lightType;
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
	float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
};

VertexOut VS(VertexIn vertexIn, uint instanceID : SV_InstanceID)
{
    VertexOut vertexOut;
    float4 PosW = mul(gInstanceData[instanceID].gWorld, float4(vertexIn.PosL, 1.0f));
    vertexOut.PosH = mul(lightProj, mul(lightView, PosW));
    vertexOut.PosW = PosW.xyz;

    return vertexOut;
}

[maxvertexcount(10)]
void GS(triangle VertexOut gin[3], inout TriangleStream<VertexOut> triStream)
{
    
}

float PS(VertexOut pixelIn) : SV_Depth
{
    float lightDis = length(pixelIn.PosW - lightPosition);

    lightDis /= lightRange;

    return lightDis;
}
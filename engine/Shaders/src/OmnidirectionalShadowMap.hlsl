#include "Common.hlsl"

cbuffer cbLightView
{
    float4x4 gLightView[6];
    float4x4 gLightProj;
    float3 gLightPosition;
    float gLightRange;
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
	float4 PosW     : POSITION;
};

VertexOut VS(VertexIn vertexIn, uint instanceID : SV_InstanceID)
{
    VertexOut vertexOut;
    float4 PosW = mul(gInstanceData[instanceID].gWorld, float4(vertexIn.PosL, 1.0f));
    vertexOut.PosW = PosW;

    return vertexOut;
}

struct GeometryOut
{
	float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    uint RTIndex    : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GS(triangle VertexOut gin[3], inout TriangleStream<GeometryOut> triStream)
{
    for (int i = 0; i < 6; ++i)
    {
        GeometryOut geometryOut;

        geometryOut.RTIndex = i;

        for (int j = 0; j < 3; ++j)
        {
            geometryOut.PosH = mul(gLightProj, mul(gLightView[i], gin[j].PosW));
            geometryOut.PosW = gin[j].PosW.xyz;

            triStream.Append(geometryOut);
        }
        triStream.RestartStrip();
    }
}

float PS(GeometryOut pixelIn) : SV_Depth
{
    float lightDis = length(pixelIn.PosW - gLightPosition);

    lightDis /= gLightRange;

    return lightDis;
}
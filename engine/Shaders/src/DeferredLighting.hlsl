#include "Common.hlsl"

struct DirectionalLight
{
	float4 Color;
	float3 Direction;
    float Intensity;
};

struct PointLight
{
	float4 Color;
	float3 Position;
	float Range;
    float Intensity;
};

struct SpotLight
{
	float4 Color;
	float3 Position;
	float Light;
	float Angle;
    float Intensity;
};

cbuffer cbLightCommon
{
    uint gDirectionalLightCount;
    uint gPointLightCount;
    uint gSpotLightCount;
};

Texture2D PositionGBuffer;
Texture2D NormalGBuffer;
Texture2D ColorGBuffer;
Texture2D MetallicGBuffer;

StructuredBuffer<DirectionalLight> DirectionalLightList;
StructuredBuffer<PointLight> PointLightList;
StructuredBuffer<SpotLight> SpotLightList;

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
	float4 PosH  : SV_POSITION;
    float2 TexC    : TEXCOORD;
};

VertexOut VS(VertexIn vertexIn)
{
    VertexOut vertexOut;
    vertexOut.PosH = float4(vertexIn.PosL, 1.0f);
    vertexOut.TexC = vertexIn.TexC;

    return vertexOut;
}

float4 PS(VertexOut pixelIn) : SV_TARGET
{
    float3 baseColor = ColorGBuffer.Sample(PointClampSampler, pixelIn.TexC).rgb;
    float3 normal = NormalGBuffer.Sample(PointClampSampler, pixelIn.TexC).rgb;
    float3 worldPos = PositionGBuffer.Sample(PointClampSampler, pixelIn.TexC).rgb;
    float3 metallic = MetallicGBuffer.Sample(PointClampSampler, pixelIn.TexC).rgb;

    float3 outColor = baseColor * 0.1f;

    uint i;
    for (i = 0; i < gDirectionalLightCount; ++i)
    {
        DirectionalLight light = DirectionalLightList[i];
        outColor += max(dot(normal, -light.Direction), 0.0f) * light.Color.rgb * light.Intensity * baseColor;
    
        float3 viewDir = gEyePosW - worldPos;
        float3 reflectDir = reflect(-light.Direction, normal);

        outColor += pow(max(dot(viewDir, reflectDir), 0.0f), 64) * light.Color.rgb * 0.5f * light.Intensity * baseColor;
    }

    for (i = 0; i < gPointLightCount; ++i)
    {
        PointLight light = PointLightList[i];
    }

    for (i = 0; i < gSpotLightCount; ++i)
    {
        SpotLight light = SpotLightList[i];
    }





    return float4(outColor, 1.0f);
}
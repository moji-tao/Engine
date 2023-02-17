#include "Common.hlsl"
#include "PBRUtlis.hlsl"
#include "ShadowMap.hlsl"

struct DirectionalLight
{
    float4x4 LightViewProj;
	float4 Color;
	float3 Direction;
    float Intensity;
    int ShadowMapIndex;
};

struct PointLight
{
	float4 Color;
	float3 Position;
	float Range;
    float Intensity;
    int ShadowMapIndex;
};

struct SpotLight
{
    float4x4 LightViewProj;
	float4 Color;
	float3 Position;
	float Light;
	float Angle;
    float Intensity;
    int ShadowMapIndex;
};

cbuffer cbLightCommon
{
    uint gDirectionalLightCount;
    uint gPointLightCount;
    uint gSpotLightCount;

    uint gEnableSSAO;
    uint gEnableAmbientLighting;
};

Texture2D PositionGBuffer;
Texture2D NormalGBuffer;
Texture2D ColorGBuffer;
Texture2D MetallicRoughnessGBuffer;
Texture2D EmissiveGBuffer;

TextureCube IBLIrradianceMap;
#define IBL_PREFILTER_ENVMAP_MIP_LEVEL 5
TextureCube IBLPrefilterEnvMaps[IBL_PREFILTER_ENVMAP_MIP_LEVEL];
Texture2D EnvBRDFLUT;

Texture2D SSAOBuffer;

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

float3 GetPrefilteredColor(float Roughness, float3 ReflectDir)
{
	float Level = Roughness * (IBL_PREFILTER_ENVMAP_MIP_LEVEL - 1);
	int FloorLevel = floor(Level);
	int CeilLevel = ceil(Level);

	float3 FloorSample = IBLPrefilterEnvMaps[FloorLevel].SampleLevel(LinearClampSampler, ReflectDir, 0).rgb;
	float3 CeilSample = IBLPrefilterEnvMaps[CeilLevel].SampleLevel(LinearClampSampler, ReflectDir, 0).rgb;
	
	float3 PrefilteredColor = lerp(FloorSample, CeilSample, (Level - FloorLevel));
	return PrefilteredColor;
}

float PointLightDistanceAttenuation(float3 worldPos, float3 lightPos, float lightRange)
{
    float3 vec = worldPos - lightPos;
    float dis2 = dot(vec, vec);
    float invRange = rcp(max(lightRange, 0.001f));
    float m = Square(saturate(1 - Square(dis2 * invRange * invRange)));
    float l = dis2 + 1;

    return m / l;
}

float4 PS(VertexOut pixelIn) : SV_TARGET
{
    float3 baseColor = ColorGBuffer.Sample(PointClampSampler, pixelIn.TexC).rgb;
    float3 normal = NormalGBuffer.Sample(PointClampSampler, pixelIn.TexC).rgb;
    float3 worldPos = PositionGBuffer.Sample(PointClampSampler, pixelIn.TexC).rgb;
    float shadingModel = PositionGBuffer.Sample(PointClampSampler, pixelIn.TexC).a;
    float metallic = MetallicRoughnessGBuffer.Sample(PointClampSampler, pixelIn.TexC).r;
    float roughness = MetallicRoughnessGBuffer.Sample(PointClampSampler, pixelIn.TexC).g;

	float ambientAccess = 1.0f;

    if (gEnableSSAO == 1)
    {
        ambientAccess = SSAOBuffer.Sample(PointClampSampler, pixelIn.TexC).r;
    }

    float3 outColor = float3(0.0f, 0.0f, 0.0f);

    if (shadingModel == 0.0f)
    {
        //outColor += ambientAccess * baseColor * 0.1f;
        // 环境光照
        if (gEnableAmbientLighting == 1)
        {
            float3 irradiance = IBLIrradianceMap.Sample(LinearClampSampler, normal).rgb;

            float3 refDir = reflect(normalize(gEyePosW - worldPos), normal);
            float3 prefilteredColor = GetPrefilteredColor(roughness, refDir);

            float nDotV = dot(normal, gEyePosW - worldPos);
            float2 lut = EnvBRDFLUT.Sample(LinearClampSampler, float2(nDotV, roughness)).rg;

            outColor += ambientAccess * AmbientLighting(baseColor, irradiance, prefilteredColor, lut, metallic);
        }


        // 直接光照
        uint i;
        [unroll(10)]
        for (i = 0; i < gDirectionalLightCount; ++i)
        {
            DirectionalLight light = DirectionalLightList[i];

            float visibility = 1.0f;

            if (light.ShadowMapIndex != -1)
            {
                float4 shadowPosH = mul(light.LightViewProj, float4(worldPos, 1.0f));
                
                visibility = ShadowVisibility(shadowPosH, light.ShadowMapIndex, light.Direction, normal);
            }

            float3 radiance = light.Color.rgb * light.Intensity;

            outColor += DirectLighting(radiance, -light.Direction, normal, normalize(gEyePosW - worldPos), metallic, roughness, baseColor) * visibility;
        }

        [unroll(5)]
        for (i = 0; i < gPointLightCount; ++i)
        {
            PointLight light = PointLightList[i];

            float visibility = 1.0f;

            if (light.ShadowMapIndex != -1)
            {
                float3 lightToPoint = worldPos - light.Position;
                visibility = OmnidirectionalShadowVisibility(lightToPoint, light.ShadowMapIndex, light.Range);
            }

            float3 radiance = light.Color.rgb * light.Intensity * PointLightDistanceAttenuation(worldPos, light.Position, light.Range);
            float3 lightDir = normalize(light.Position - worldPos);

            outColor += DirectLighting(radiance, lightDir, normal, normalize(gEyePosW - worldPos), metallic, roughness, baseColor) * visibility;
        }

        for (i = 0; i < gSpotLightCount; ++i)
        {
            SpotLight light = SpotLightList[i];
        }
    }
    else
    {
        outColor = baseColor;
    }

/*
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


*/

    return float4(outColor, 1.0f);
}
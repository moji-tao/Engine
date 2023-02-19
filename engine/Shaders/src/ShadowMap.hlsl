#include "Utlis.hlsl"
#include "Sampler.hlsl"

#define MAX_SHADOW_MAP_2D_NUM  10
Texture2D ShadowMaps2D[MAX_SHADOW_MAP_2D_NUM];

#define MAX_SHADOW_MAP_CUBE_NUM  5
TextureCube ShadowMapsCube[MAX_SHADOW_MAP_CUBE_NUM];

float PCF(float z, uint shadowMapIndex, float2 shadowMapUV, int radians)
{
    uint Width = 0, Height = 0, NumMips = 0;
    ShadowMaps2D[shadowMapIndex].GetDimensions(0, Width, Height, NumMips);
    float dx = 1.0f / (float)Width;

    float res = 0.0f;

    /*
    const float2 offsets[9] = 
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, dx), float2(0.0f, dx), float2(dx, dx)
    };
    */
    
    const float2 offsets[25] = 
    {
        float2(2 * -dx, 2 * -dx), float2(-dx, 2 * -dx), float2(0.0f, 2 * -dx), float2(dx, 2 * -dx), float2(2 * dx, 2 * -dx),
        float2(2 * -dx, -dx), float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx), float2(2 * dx, -dx),
        float2(2 * -dx, 0.0f), float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f), float2(2 * dx, 0.0f),
        float2(2 * -dx, dx), float2(-dx, dx), float2(0.0f, dx), float2(dx, dx), float2(2 * dx, dx),
        float2(2 * -dx, 2 * dx), float2(-dx, 2 * dx), float2(0.0f, 2 * dx), float2(dx, 2 * dx), float2(2 * dx, 2 * dx),
    };

    [unroll]
    for (int i = 0; i < 25; ++i)
    {
        res += ShadowMaps2D[shadowMapIndex].SampleCmpLevelZero(ShadowSampler, shadowMapUV + offsets[i], z);
    }

    return res / 25.0f;

/*
    [unroll]
    for (int i = 0; i < 25; ++i)
    {
        float sz = ShadowMaps2D[shadowMapIndex].Sample(LinearClampSampler, shadowMapUV + offsets[i]).r;
        if ((z) <= sz)
        {
            res += 1.0f;
        }
        else
        {
            res += 0.3f;
        }
    }
    return res / 25.0f;
*/
    //return ShadowMaps2D[shadowMapIndex].SampleCmpLevelZero(ShadowSampler, shadowMapUV, z);
/*
    float sz = ShadowMaps2D[shadowMapIndex].Sample(LinearClampSampler, shadowMapUV).r;
    if ((z - 0.0001f) <= sz)
    {
        res = 1.0f;
    }
    else
    {
        res = 0.3f;
    }

    return res;
*/
}

float ShadowVisibility(float4 pos, uint shadowMapIndex, float3 lightDir, float3 normal)
{
    pos.xyz /= pos.w;

    float2 shadowMapUV = NDCToUV(pos);

    float offsetZ = max(0.005f, 0.05f * dot(normal, -lightDir));

    return PCF(pos.z, shadowMapIndex, shadowMapUV, 1);
}

float OmnidirectionalShadowVisibility(float3 lightToPoint, uint shadowMapIndex, float range)
{
    float depth = ShadowMapsCube[shadowMapIndex].Sample(PointClampSampler, normalize(lightToPoint)).r;

    depth *= range;

    float currentDepth = length(lightToPoint);

    float visibility;

    visibility = ((currentDepth - 0.05f) < depth) ? 1.0f : 0.0f;

    return visibility;
}
#include "Utlis.hlsl"

const static float NonMetallicReflectivity = 0.04f;

float3 LambertianDiffuse(float3 color)
{
    return color / PI;
}

float GGX(float alpha2, float nDotH)
{
    float d = nDotH * nDotH * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * d * d);
}

float GeometrySchlickGGX(float nDotV, float roughness)
{
    float k = ((roughness + 1.0f) * (roughness + 1.0f)) / 8.0f;

    return nDotV / (nDotV * (1.0f - k) + k);
}

float3 FresnelSchlick(float vDotH, float3 F0)
{
    //return F0 + (1 - F0) * exp2((-5.55473 * vDotH - 6.98316) * vDotH);
    return F0 + (1 - F0) * pow(1.0f - vDotH, 5.0f);
}

float3 DiffuseBRDF(float k, float3 color)
{
    return k * LambertianDiffuse(color);
}

float3 SpecularBRDF(float3 normal, float3 lightDir, float3 viewDir, float3 F0, float roughness)
{
    float3 h = normalize(lightDir + viewDir);
    
    float alpha2 = Pow4(roughness);

    float nDotH = saturate(dot(normal, h));
    float nDotV = saturate(dot(normal, viewDir));
    float vDotH = saturate(dot(viewDir, h));
    float nDotL = saturate(dot(normal, lightDir));

    float D = GGX(alpha2, nDotH);
    float G = GeometrySchlickGGX(nDotV, roughness) * GeometrySchlickGGX(nDotL, roughness);
    float3 F = FresnelSchlick(vDotH, F0);

    return (D * G * F) / (4.0f * max(nDotV * nDotL, 0.001f));
}

float3 BRDF(float3 lightDir, float3 normal, float3 viewDir, float metallic, float roughness, float3 baseColor)
{
    float3 F0 = lerp(NonMetallicReflectivity, baseColor, metallic);

    float3 diffuseColor = DiffuseBRDF(1.0f - metallic, baseColor);

    float3 specularColor = SpecularBRDF(normal, lightDir, viewDir, F0, roughness);

    return diffuseColor + specularColor;
}

// 平行光 点光源 聚光灯
float3 DirectLighting(float3 radiance, float3 lightDir, float3 normal, float3 viewDir, float metallic, float roughness, float3 baseColor)
{
    return BRDF(lightDir, normal, viewDir, metallic, roughness, baseColor) * radiance * saturate(dot(normal, lightDir));
}

float3 EnvBRDF(float metallic, float3 baseColor, float2 lut)
{
    float3 F0 = lerp(NonMetallicReflectivity.rrr, baseColor.rgb, metallic); 
    
    return F0 * lut.x + lut.y;
}

float3 AmbientLighting(float3 baseColor, float3 radiance, float3 prefilteredColor, float2 lut, float metallic)
{
    float3 diffuseColor = DiffuseBRDF(1 - metallic, baseColor) * radiance;

    float3 specularColor = prefilteredColor * EnvBRDF(metallic, baseColor, lut);

    return diffuseColor + specularColor;
}

// 面光源
float3 AreaLighting()
{
    return float3(0.0f, 0.0f, 0.0f);
}

#ifndef __SHADER_COMMON__
#define __SHADER_COMMON__

#include "Utlis.hlsl"
#include "Sampler.hlsl"

cbuffer cbPass : register(b0)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gJitterProj;
    float4x4 gInvJitterProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
	float4x4 gPrevViewProj;
	float4x4 gJitterViewProj;
	float4x4 gInvJitterViewProj;
    float2 gJitter;
    float gNearZ;
    float gFarZ;
    float3 gEyePosW;
    float gTotalTime;
    float3 gPrevPosW;
    float gDeltaTime;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    uint gFrameCount;
};

struct MaterialData
{
    /*
	float4   DiffuseAlbedo;
	float3   FresnelR0;
	float    Roughness;
	float4x4 MatTransform;
	
    float3 EmissiveColor;
	uint ShadingModel;
    */
	uint HasAlbedoTexture;
	uint HasNormalTexture;
	uint HasMetallicTexture;
	uint HasRoughnessTexture;

	float4 AlbedoColor;
    float3 Emissive;
	float Metallic;
	float Roughness;
};
ConstantBuffer<MaterialData> cbMaterialData;

struct InstanceData
{
    float4x4 gWorld;
    float4x4 gInvWorld;
    float4x4 gPreWorld;
};

#endif //__SHADER_COMMON__
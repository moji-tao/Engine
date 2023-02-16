#include "Common.hlsl"

TextureCube SkyCubeTexture;

cbuffer cbEnvironmentPass
{
	float4x4 gEnvironmentView;
	float4x4 gEnvironmentProj;
	float Roughness;
}

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
	float4 PosH		: SV_POSITION;
	float3 PosL		: POSITION;
};

VertexOut VS(VertexIn vertexIn)
{
    VertexOut vertexOut;

    vertexOut.PosL = vertexIn.PosL;

	float4x4 View = gEnvironmentView;
	View[0][3] = View[1][3] = View[2][3] = 0.0f;
	
	vertexOut.PosH = mul(gEnvironmentProj, mul(View, float4(vertexIn.PosL, 1.0f)));

    return vertexOut;
}

float4 PS(VertexOut pixelIn) : SV_TARGET
{
    float3 irradiance = float3(0.0f, 0.0f, 0.0f);

    float3 normal = normalize(pixelIn.PosL);
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = cross(up, normal);
    up = cross(normal, right);

    float SampleDelta = 0.025f;
	float SampleCount = 0.0f;
	for (float phi = 0.0f; phi < 2.0f * PI; phi += SampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += SampleDelta)
		{
			// Spherical to cartesian (in tangent space)
			float3 TangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// Tangent space to world
			float3 SampleVec = TangentSample.x * right + TangentSample.y * up + TangentSample.z * normal;
			
			irradiance += SkyCubeTexture.Sample(AnisotropicWrapSampler, SampleVec).rgb * cos(theta) * sin(theta);
			SampleCount++;
		}
	}
	irradiance = PI * irradiance * (1.0f / SampleCount);

    return float4(irradiance, 1.0f);
}
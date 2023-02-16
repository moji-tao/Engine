#include "Common.hlsl"

TextureCube SkyCubeTexture : register(t0);

cbuffer cbEnvironmentPass
{
	float4x4 gEnvironmentView;
	float4x4 gEnvironmentProj;
	float gRoughness;
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
    float3 N = normalize(pixelIn.PosL); 
	float3 R = N;
    float3 V = R;
	
	const uint SAMPLE_COUNT = 1024u;
    float TotalWeight = 0.0;   
    float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
	
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley(i, SAMPLE_COUNT);
        float3 H  = ImportanceSampleGGX(Xi, N, gRoughness).xyz;
        float3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NoL = saturate(dot(N, L));
        if(NoL > 0.0)
        {	
            PrefilteredColor += SkyCubeTexture.Sample(AnisotropicWrapSampler, L).rgb * NoL;
            TotalWeight      += NoL;
        }
    }
	
    PrefilteredColor = PrefilteredColor / TotalWeight;

	return float4(PrefilteredColor, 1.0f);
}
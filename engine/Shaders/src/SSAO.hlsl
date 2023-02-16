#include "Common.hlsl"

Texture2D NormalGBuffer;
Texture2D DepthGBuffer;

cbuffer cbSSAO
{
    float gOcclusionRadius;
    float gSurfaceEpsilon;
    float gOcclusionFadeEnd;
    float gOcclusionFadeStart;
}

struct VertexIn
{
    float3 PosL : POSITION;
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

static const int gSampleCount = 16;

// Determines how much the point R occludes the point P as a function of DistZ.
float OcclusionFunction(float DistZ)
{
	//
	//       1.0     -------------\
	//               |           |  \
	//               |           |    \
	//               |           |      \ 
	//               |           |        \
	//               |           |          \
	//               |           |            \
	//  ------|------|-----------|-------------|---------|--> zv
	//        0     Eps       Start           End       
	//
	
	float Occlusion = 0.0f;
	if(DistZ > gSurfaceEpsilon)
	{
		float FadeLength = gOcclusionFadeEnd - gOcclusionFadeStart;	
		Occlusion = saturate( (gOcclusionFadeEnd - DistZ) / FadeLength );
	}
	
	return Occlusion;	
}

float PS(VertexOut pixelIn) : SV_TARGET
{
    float3 normalW = NormalGBuffer.SampleLevel(PointClampSampler, pixelIn.TexC, 0).rgb;
    float3 normalV = mul((float3x3)gView, normalW);
    normalV = normalize(normalV);

    float pZ = DepthGBuffer.SampleLevel(PointClampSampler, pixelIn.TexC, 0).r;
    float3 posV = UVToView(pixelIn.TexC, pZ, gInvProj).xyz;

    float occlusionSum = 0.0f;

	const float Phi = PI * (3.0f - sqrt(5.0f));
    for (int i = 0; i < gSampleCount; ++i)
    {
		// Fibonacci lattices.
		float3 offset;
		offset.y = 1 - (i / float(gSampleCount - 1)) * 2;  // y goes from 1 to - 1
		float Radius = sqrt(1.0f - offset.y * offset.y);  // Radius at y
		float Theta = Phi * i;    // Golden angle increment
		offset.x = Radius * cos(Theta);
		offset.z = Radius * sin(Theta);

        float filp = sign(dot(offset, normalV));

        float3 q = posV + filp * offset * gOcclusionRadius;

        float2 qUV = ViewToUV(float4(q, 1.0f), gProj);
        float rZ = DepthGBuffer.SampleLevel(PointClampSampler, qUV, 0).r;
        float3 posRV = UVToView(qUV, rZ, gInvProj).xyz;

        float dp = max(dot(normalV, normalize(posRV - posV)), 0.0f);

        occlusionSum += dp * OcclusionFunction(pZ - rZ);
    }


    occlusionSum /= gSampleCount;

    float ambientAccess = 1.0f - occlusionSum;

    return saturate(pow(ambientAccess, 6.0f));
}